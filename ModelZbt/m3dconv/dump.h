/*
 * m3dconv/dump.h
 *
 * Copyright (C) 2019 bzt (bztsrc@gitlab)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * @brief dump a Model 3D (file or in-memory) to stdout
 * https://gitlab.com/bztsrc/model3d
 *
 */

#include <string.h>

#ifndef dumplog
#define dumplog printf
#endif

#ifndef dumperr
#define dumperr fprintf
#endif

/**
 * Dump and validate binary model file byte-by-byte
 */
int dump_file(unsigned char *data, unsigned int size, int dump)
{
    unsigned char *out = NULL, *end = NULL, *zip = NULL, *chend, weights[8];
    char *str;
    static char *cn[] = { "int8_t", "int16_t", "float", "double" };
    static char *in[] = { "uint8_t", "uint16_t", "uint32_t", "(none)" };
    static char *pf[] = { "color ", "uint8 ", "uint16", "uint32", "float ", "map   " };
    unsigned int len, vc_s, vi_s, si_s, ci_s, ti_s, bi_s, nb_s, sk_s, fc_s, hi_s, fi_s, vd_s, vp_s, *cmap = NULL;
    int i, j, k, l, m, n, d, nv = 0, nt = 0, np = 0, nb = 0, nc = 0, nvt = 0;
    m3dcd_t *cd;

    if(!data || !size) return 1;
    if(M3D_CHUNKMAGIC(data, '3','D','M','O')) {
        dumplog("Model 3D File Header\n"
            "  Magic:  3DMO\n"
            "  Length: %u\n", ((m3dchunk_t*)data)->length);
        if(((m3dchunk_t*)data)->length != size) {
            dumperr(stderr, "ERROR: Invalid 3DMO chunk length %u (should be %u)\n",((m3dchunk_t*)data)->length,size);
            return 1;
        }
        if(M3D_CHUNKMAGIC(data, 'P','R','V','W')) {
            dumplog("\nPreview Chunk\n  Magic:  PRVW\n"
                "  Length: %u\n\n",
                ((m3dhdr_t*)data)->length);
            size -= ((m3dhdr_t*)data)->length;
            data += ((m3dhdr_t*)data)->length;
        }

        dumplog("  Data:   %scompressed", M3D_CHUNKMAGIC(data + 8, 'H','E','A','D') ? "un":"stream ");
        if(!M3D_CHUNKMAGIC(data + 8, 'H','E','A','D')) {
            zip = (unsigned char *)stbi_zlib_decode_malloc_guesssize_headerflag((const char*)data+8,
                size-8, 16384, (int*)&size, 1);
            if(!zip || !size || !M3D_CHUNKMAGIC(zip, 'H','E','A','D')) {
                dumperr(stderr, "m3dconv: uncompression error\n");
                return 1;
            }
            dumplog(" (inflated %u bytes)", size + 8);
            data = zip;
        } else {
            data += 8;
            size -= 8;
        }
        dumplog("\n\n");

        end = data + size;
        while(data < end) {
            if(M3D_CHUNKMAGIC(data, 'O','M','D','3')) {
                dumplog("End Chunk\n  Magic: OMD3\n\n");
                if(data + 4 < end) { dumperr(stderr, "ERROR: Garbage at the end of the file\n"); return 1; }
                break;
            }

            out = data + 8;
            len = *((uint32_t*)(data + 4));
            if(len < 8) {
                dumperr(stderr, "ERROR: Invalid %c%c%c%c chunk length %u\n",data[0],data[1],data[2],data[3],len);
                return 1;
            }

            if(M3D_CHUNKMAGIC(data, 'H','E','A','D')) {
                dumplog("Model Header Chunk\n  Magic:   HEAD\n"
                    "  Length:  %u\n"
                    "  Scale:   %g meter(s)\n"
                    "  Types:   0x%08x\n",
                    ((m3dhdr_t*)data)->length, ((m3dhdr_t*)data)->scale, ((m3dhdr_t*)data)->types);
                i = ((m3dhdr_t*)data)->types;
                vc_s = 1 << ((i >> 0) & 3);  /* vertex coordinate size */
                vi_s = 1 << ((i >> 2) & 3);  /* vertex index size */
                si_s = 1 << ((i >> 4) & 3);  /* string offset size */
                ci_s = 1 << ((i >> 6) & 3);  /* color index size */
                ti_s = 1 << ((i >> 8) & 3);  /* tmap index size */
                bi_s = 1 << ((i >>10) & 3);  /* bone index size */
                nb_s = 1 << ((i >>12) & 3);  /* number of bones per vertex */
                sk_s = 1 << ((i >>14) & 3);  /* skin index size */
                fc_s = 1 << ((i >>16) & 3);  /* frame counter size */
                hi_s = 1 << ((i >>18) & 3);  /* shape index size */
                fi_s = 1 << ((i >>20) & 3);  /* face index size */
                vd_s = 1 << ((i >>22) & 3);  /* voxel dimension size */
                vp_s = 1 << ((i >>24) & 3);  /* voxel pixel size */

                if(dump > 1) {
                    dumplog("    vc_t %d %-10s (vertex coordinate)\n", i & 3, cn[i & 3]); i >>= 2;
                    dumplog("    vi_t %d %-10s (vertex index)\n", i & 3, in[i & 3]); i >>= 2;
                    dumplog("    si_t %d %-10s (string offset)\n", i & 3, in[i & 3]); i >>= 2;
                    dumplog("    ci_t %d %-10s (color index)\n", i & 3, in[i & 3]); i >>= 2;
                    dumplog("    ti_t %d %-10s (texture uv index)\n", i & 3, in[i & 3]); i >>= 2;
                    dumplog("    bi_t %d %-10s (bone index)\n", i & 3, in[i & 3]); i >>= 2;
                    dumplog("    nb_t %d %d/vertex   (number of bones per vertex)\n", i & 3, nb_s); i >>= 2;
                    dumplog("    sk_t %d %-10s (skin index)\n", i & 3, in[i & 3]); i >>= 2;
                    dumplog("    fc_t %d %-10s (frame counter)\n", i & 3, in[i & 3]); i >>= 2;
                    dumplog("    hi_t %d %-10s (shape index)\n", i & 3, in[i & 3]); i >>= 2;
                    dumplog("    fi_t %d %-10s (face index)\n", i & 3, in[i & 3]); i >>= 2;
                    dumplog("    vd_t %d %-10s (voxel dim)\n", i & 3, in[i & 3] + 1); i >>= 2;
                    dumplog("    vp_t %d %-10s (voxel type)\n", i & 3, in[i & 3]); i >>= 2;
                }
                if(data[((m3dhdr_t*)data)->length - 1]) { dumperr(stderr, "ERROR: Bad chunk size\n"); return 1; }
                out = data + sizeof(m3dhdr_t);
                chend = data + ((m3dhdr_t*)data)->length;
                str = (char*)out;
                dumplog("  Model:   '%s'\n", out);
                out += strlen((char*)out) + 1;
                dumplog("  License: '%s'\n", out);
                out += strlen((char*)out) + 1;
                dumplog("  Author:  '%s'\n", out);
                out += strlen((char*)out) + 1;
                if(dump > 1) {
                    dumplog("  Description:\n%s\n  String Table:\n", out);
                    out += strlen((char*)out) + 1;
                    while(out < chend) {
                        dumplog(si_s == 1 ? "    %02x. '%s'\n" : (si_s == 2 ? "    %04x. '%s'\n" : "    %08x. '%s'\n"),
                        (uint32_t)((uintptr_t)out - (uintptr_t)data - 16), out);
                        out += strlen((char*)out) + 1;
                    }
                    if(out != chend) { dumperr(stderr, "ERROR: Bad string table size\n"); return 1; }
                }
                dumplog("\n");
            } else

            if(M3D_CHUNKMAGIC(data, 'C','M','A','P')) {
                cmap = (uint32_t*)out;
                dumplog("Color Map Chunk\n  Magic:  CMAP\n"
                    "  Length: %u\n",
                    ((m3dhdr_t*)data)->length);
                if(ci_s >= 4) { dumperr(stderr, "ERROR: There should be no color map\n"); }
                if(((m3dhdr_t*)data)->length & 3) { dumperr(stderr, "ERROR: Bad chunk size\n"); return 1; }
                nc = ((m3dhdr_t*)data)->length / 4;
                if(dump > 1) {
                    dumplog("  Data:  ");
                    for(i = 0; i < (int)(((m3dhdr_t*)data)->length/4) - 2; i++, out += 4)
                        dumplog(" #%08x", *((uint32_t*)out));
                    dumplog("\n");
                }
                dumplog("\n");
            } else

            if(M3D_CHUNKMAGIC(data, 'T','M','A','P')) {
                dumplog("Texture Map Chunk\n  Magic:  TMAP\n"
                    "  Length: %u\n",
                    ((m3dhdr_t*)data)->length);
                i = 2 * vc_s;
                nt = (((m3dhdr_t*)data)->length-8)/i;
                if(dump > 1) dumplog("  NumUV: %u, reclen %u (use -ddd to dump all)\n", nt, i);
                if((unsigned int)nt*i != (((m3dhdr_t*)data)->length-8))
                    { dumperr(stderr, "ERROR: Bad chunk size\n"); return 1; }
                if(dump > 2) {
                    for(i = 0; i < (int)((((m3dhdr_t*)data)->length - 8)/(2*vc_s)); i++) {
                        dumplog("  %5u. ", i);
                        switch(vc_s) {
                            case 1:
                                dumplog("%02x (u=%2g) %02x (u=%2g) ",
                                    out[0], (float)((uint8_t)out[0]) / 255.0,
                                    out[1], (float)((uint8_t)out[1]) / 255.0);
                                out += 2;
                            break;
                            case 2:
                                dumplog("%04x (u=%2g) %04x (v=%2g) ",
                                    *((uint16_t*)(out+0)), (float)(*((uint16_t*)(out+0))) / 65535.0,
                                    *((uint16_t*)(out+2)), (float)(*((uint16_t*)(out+2))) / 65535.0);
                                out += 4;
                            break;
                            case 4:
                                dumplog("%08x (u=%2g) %08x (v=%2g) ",
                                    *((uint32_t*)(out+0)), *((float*)(out+0)),
                                    *((uint32_t*)(out+4)), *((float*)(out+4)));
                                out += 8;
                            break;
                            case 8:
                                dumplog("(u=%2g) (v=%2g)", *((double*)(out+0)), *((double*)(out+8)));
                                out += 16;
                            break;
                        }
                        dumplog("\n");
                    }
                }
                dumplog("\n");
            } else

            if(M3D_CHUNKMAGIC(data, 'V','R','T','S')) {
                chend = data + ((m3dhdr_t*)data)->length;
                dumplog("Vertex Chunk\n  Magic:  VRTS\n"
                    "  Length: %u\n",
                    ((m3dhdr_t*)data)->length);
                if(ci_s < 4 && !cmap) { dumperr(stderr, "ERROR: there should be a color map\n"); }
                i = 4*vc_s + (ci_s!=8 ? ci_s : 0) + (sk_s!=8 ? sk_s : 0);
                nv = (((m3dhdr_t*)data)->length-8)/i;
                if(dump > 1) dumplog("  Numvertex: %u, reclen %u (use -ddd to dump all)\n", nv, i);
                if((unsigned int)(nv*i) != (((m3dhdr_t*)data)->length-8)) { dumperr(stderr, "ERROR: Bad chunk size\n"); return 1; }
                if(dump > 2) {
                    for(i = 0; out < chend && i < nv; i++) {
                        dumplog("  %5u. ", i);
                        switch(vc_s) {
                            case 1:
                                dumplog("%02x (x=%2g) %02x (y=%2g) %02x (z=%2g) %02x (w=%2g) ",
                                    out[0], (float)((int8_t)out[0]) / 127,
                                    out[1], (float)((int8_t)out[1]) / 127,
                                    out[2], (float)((int8_t)out[2]) / 127,
                                    out[3], (float)((int8_t)out[3]) / 127);
                                out += 4;
                            break;
                            case 2:
                                dumplog("%04x (x=%2g) %04x (y=%2g) %04x (z=%2g) %04x (w=%2g) ",
                                    *((int16_t*)(out+0)), (float)(*((int16_t*)(out+0))) / 32767,
                                    *((int16_t*)(out+2)), (float)(*((int16_t*)(out+2))) / 32767,
                                    *((int16_t*)(out+4)), (float)(*((int16_t*)(out+4))) / 32767,
                                    *((int16_t*)(out+6)), (float)(*((int16_t*)(out+6))) / 32767);
                                out += 8;
                            break;
                            case 4:
                                dumplog("%08x (x=%2g) %08x (y=%2g) %08x (z=%2g) %08x (w=%2g) ",
                                    *((int32_t*)(out+0)), *((float*)(out+0)),
                                    *((int32_t*)(out+4)), *((float*)(out+4)),
                                    *((int32_t*)(out+8)), *((float*)(out+8)),
                                    *((int32_t*)(out+12)), *((float*)(out+12)));
                                out += 16;
                            break;
                            case 8:
                                dumplog("(x=%2g) (y=%2g) (z=%2g) (w=%2g) ",
                                    *((double*)(out+0)), *((double*)(out+8)),
                                    *((double*)(out+16)), *((double*)(out+24)));
                                out += 32;
                            break;
                        }
                        m = 0;
                        switch(ci_s) {
                            case 1: dumplog("%02x (#%08x) ", out[0], cmap ? cmap[out[0]] : 0); m = out[0]; out++; break;
                            case 2: dumplog("%04x (#%08x) ", *((uint16_t*)out), cmap ? cmap[*((uint16_t*)out)] : 0); m = *((uint16_t*)out); out += 2; break;
                            case 4: dumplog("%08x (#%08x) ", *((uint32_t*)out), *((uint32_t*)out)); out += 4; break;
                        }
                        if(m && m >= nc) { dumplog("\n"); dumperr(stderr, "ERROR: cmap index out of bound\n"); return 1; }
                        switch(sk_s) {
                            case 1: dumplog("%02x (skin=%d) ", out[0], (int8_t)out[0]); out++; break;
                            case 2: dumplog("%04x (skin=%d) ", *((uint16_t*)out), *((int16_t*)out)); out += 2; break;
                            case 4: dumplog("%08x (skin=%d) ", *((uint32_t*)out), *((int32_t*)out)); out += 4; break;
                        }
                        dumplog("\n");
                    }
                    if(out != chend) { dumperr(stderr, "ERROR: Bad chunk size\n"); return 1; }
                }
                dumplog("\n");
            } else

            if(M3D_CHUNKMAGIC(data, 'B','O','N','E')) {
                chend = data + ((m3dhdr_t*)data)->length;
                nb = 0;
                switch(bi_s) {
                    case 1: nb = out[0]; out++; break;
                    case 2: nb = *((uint16_t*)out); out += 2; break;
                    case 4: nb = *((uint32_t*)out); out += 4; break;
                }
                l = 0;
                switch(sk_s) {
                    case 1: l = out[0]; out++; break;
                    case 2: l = *((uint16_t*)out); out += 2; break;
                    case 4: l = *((uint32_t*)out); out += 4; break;
                }
                dumplog("Skeleton Chunk\n  Magic:  BONE\n"
                    "  Length: %u\n"
                    "  #Bones: %u\n"
                    "  #Skins: %u\n",
                    ((m3dhdr_t*)data)->length, nb, l);
                if(bi_s >= 4) { dumperr(stderr, "ERROR: there should be no skeleton\n"); }
                if(!nv) { dumperr(stderr, "ERROR: bones without vertex list\n"); return 1; }
                if(((m3dhdr_t*)data)->length < 8 + nb*(bi_s + si_s + 2*vi_s) + l*bi_s)
                    { dumperr(stderr, "ERROR: Bad chunk size\n"); return 1; }
                if(dump > 1) {
                    dumplog("    Bone Hierarchy\n");
                    for(i = 0; i < nb; i++) {
                        dumplog("  %5u. ", i);
                        j = nb;
                        switch(bi_s) {
                            case 1: dumplog("%02x (parent=%d) ", out[0], (int8_t)out[0]); j = (int8_t)out[0]; out++; break;
                            case 2: dumplog("%04x (parent=%d) ", *((uint16_t*)out), *((int16_t*)out)); j = *((int16_t*)out); out += 2; break;
                            case 4: dumplog("%08x (parent=%d) ", *((uint32_t*)out), *((int32_t*)out)); j = *((int32_t*)out); out += 4; break;
                        }
                        if(j != -1 && j >= nb) { dumplog("\n"); dumperr(stderr, "ERROR: bone index out of bound\n"); return 1; }
                        j = 0;
                        switch(si_s) {
                            case 1: j = out[0]; out++; break;
                            case 2: j = *((uint16_t*)out); out += 2; break;
                            case 4: j = *((uint32_t*)out); out += 4; break;
                        }
                        dumplog(si_s == 1 ? "%02x '%s' " : (si_s == 2 ? "%04x '%s' " : "%08x '%s' "),
                            j, j ? str + j : "");
                        j = nv;
                        switch(vi_s) {
                            case 1: dumplog("%02x (pos=%u) ", out[0], (int8_t)out[0]); j = out[0]; out++; break;
                            case 2: dumplog("%04x (pos=%u) ", *((uint16_t*)out), *((int16_t*)out)); j = *((uint16_t*)out); out += 2; break;
                            case 4: dumplog("%08x (pos=%u) ", *((uint32_t*)out), *((int32_t*)out)); j = *((uint32_t*)out); out += 4; break;
                        }
                        if(j >= nv) { dumplog("\n"); dumperr(stderr, "ERROR: vertex index out of bound\n"); return 1; }
                        j = nv;
                        switch(vi_s) {
                            case 1: dumplog("%02x (ori=%u) ", out[0], (int8_t)out[0]); j = out[0]; out++; break;
                            case 2: dumplog("%04x (ori=%u) ", *((uint16_t*)out), *((int16_t*)out)); j = *((uint16_t*)out); out += 2; break;
                            case 4: dumplog("%08x (ori=%u) ", *((uint32_t*)out), *((int32_t*)out)); j = *((uint32_t*)out); out += 4; break;
                        }
                        if(j >= nv) { dumplog("\n"); dumperr(stderr, "ERROR: vertex index out of bound\n"); return 1; }
                        dumplog("\n");
                    }
                    dumplog("    Skin (bone id and weight pairs for vertices)\n");
                    for(i = 0; i < l && out < chend; i++) {
                        dumplog("  %5u.", i);
                        memcpy(&weights, out, nb_s);
                        if(nb_s == 1) {
                            dumplog(" (implicit w=1.0) ");
                            weights[0] = 255;
                        } else
                            for(j = 0; j < (int)nb_s; j++) { dumplog(" %02x (w=%g) ", out[0], ((float)(out[0])) / 255); out++; }
                        for(j = 0; j < (int)nb_s && weights[j]; j++) {
                            m = nb;
                            switch(bi_s) {
                                case 1: dumplog("%02x (bone=%u) ", out[0], out[0]); m = out[0]; out++; break;
                                case 2: dumplog("%04x (bone=%u) ", *((uint16_t*)out), *((uint16_t*)out)); m = *((uint16_t*)out); out += 2; break;
                                case 4: dumplog("%08x (bone=%u) ", *((uint32_t*)out), *((uint32_t*)out)); m = *((uint32_t*)out); out += 4; break;
                            }
                            if(m >= nb) { dumplog("\n"); dumperr(stderr, "ERROR: bone index out of bound\n"); return 1; }
                        }
                        dumplog("\n");
                    }
                    if(i != l || out != chend) { dumperr(stderr, "ERROR: Bad chunk size\n"); return 1; }
                }
                dumplog("\n");
            } else

            if(M3D_CHUNKMAGIC(data, 'M','T','R','L')) {
                chend = data + ((m3dhdr_t*)data)->length;
                i = 0;
                switch(si_s) {
                    case 1: i = out[0]; out++; break;
                    case 2: i = *((uint16_t*)out); out += 2; break;
                    case 4: i = *((uint32_t*)out); out += 4; break;
                }
                dumplog("Material Definiton Chunk\n  Magic:  MTRL\n"
                    "  Length: %u\n"
                    "  Name:   ",
                    ((m3dhdr_t*)data)->length);
                if(dump > 1)
                    dumplog(si_s == 1 ? "%02x " : (si_s == 2 ? "%04x " : "%08x "), i);
                dumplog("'%s'\n", i ? str + i : "");
                if(((m3dhdr_t*)data)->length < 8 + si_s + 2) { dumperr(stderr, "ERROR: Bad chunk size\n"); return 1; }
                if(dump > 1) {
                    while(out < chend) {
                        for(i = 0, j = 256; i < (int)(sizeof(m3d_propertytypes)/sizeof(m3d_propertytypes[0])); i++)
                            if(m3d_propertytypes[i].id == out[0] ||
                                m3d_propertytypes[i].id + 128 == out[0]) { j = i; break; }
                        if(j == 256) { dumplog("    %02x unknown property?\n", out[0]); break; }
                        k = out[0] >= 128 ? m3dpf_map : m3d_propertytypes[j].format;
                        dumplog("    %02x m3dpf_%s ", out[0], pf[k]);
#ifdef M3D_ASCII
                        dumplog("m3dp_%s%-4s%s ", out[0] >= 128 ? "map_" : "", m3d_propertytypes[j].key,
                            out[0] >= 128 ? "" : "    ");
#endif
                        out++;
                        switch(k) {
                            case m3dpf_color:
                                if(!cmap && (ci_s==1 || ci_s==2)) { dumplog("\n"); dumperr(stderr, "ERROR: missing color map\n"); return 1; }
                                m = 0;
                                switch(ci_s) {
                                    case 1: dumplog("%02x (#%08x)", out[0], cmap[out[0]]); m = out[0]; out++; break;
                                    case 2: dumplog("%04x (#%08x)", *((uint16_t*)out), cmap[*((uint16_t*)out)]); m = *((uint16_t*)out); out += 2; break;
                                    case 4: dumplog("%08x (#%08x)", *((uint32_t*)out), *((uint32_t*)out)); out += 4; break;
                                }
                                if(m && m >= nc) { dumplog("\n"); dumperr(stderr, "ERROR: cmap index out of bound\n"); return 1; }
                            break;
                            case m3dpf_uint8: dumplog("%02x (%u)", out[0], out[0]); out++; break;
                            case m3dpf_uint16: dumplog("%04x (%u)", *((uint16_t*)out), *((uint16_t*)out)); out += 2; break;
                            case m3dpf_uint32: dumplog("%08x (%u)", *((uint32_t*)out), *((uint32_t*)out)); out += 4; break;
                            case m3dpf_float: dumplog("%08x (%g)", *((uint32_t*)out), *((float*)out)); out += 4; break;
                            case m3dpf_map:
                                l = 0;
                                switch(si_s) {
                                    case 1: l = out[0]; out++; break;
                                    case 2: l = *((uint16_t*)out); out += 2; break;
                                    case 4: l = *((uint32_t*)out); out += 4; break;
                                }
                                dumplog(si_s == 1 ? "%02x '%s'" : (si_s == 2 ? "%04x '%s'" : "%08x '%s'"),
                                    l, l ? str + l : "");
                            break;
                        }
                        dumplog("\n");
                    }
                    if(out != chend) { dumperr(stderr, "ERROR: Bad chunk size\n"); return 1; }
                }
                dumplog("\n");
            } else

            if(M3D_CHUNKMAGIC(data, 'P','R','O','C')) {
                i = 0;
                switch(si_s) {
                    case 1: i = out[0]; out++; break;
                    case 2: i = *((uint16_t*)out); out += 2; break;
                    case 4: i = *((uint32_t*)out); out += 4; break;
                }
                dumplog("Procedural Surface Chunk\n  Magic:  PROC\n"
                    "  Length: %u\n"
                    "  Script: ",
                    ((m3dhdr_t*)data)->length);
                if(dump > 1)
                    dumplog(si_s == 1 ? "%02x " : (si_s == 2 ? "%04x " : "%08x "), i);
                dumplog("'%s'\n", i ? str + i : "");
                if(((m3dhdr_t*)data)->length != 8 + si_s) { dumperr(stderr, "ERROR: Bad chunk size\n"); return 1; }
                dumplog("\n");
            } else

            if(M3D_CHUNKMAGIC(data, 'M','E','S','H')) {
                chend = data + ((m3dhdr_t*)data)->length;
                dumplog("Polygon Mesh Chunk\n  Magic:  MESH\n"
                    "  Length: %u\n",
                    ((m3dhdr_t*)data)->length);
                if(((m3dhdr_t*)data)->length < 9 + 3 * vi_s) { dumperr(stderr, "ERROR: Bad chunk size\n"); return 1; }
                if(!nv) { dumperr(stderr, "ERROR: mesh without vertex list\n"); return 1; }
                if(dump > 1) {
                    d = 0;
                    for(i = 0; out < chend; i++) {
                        if(!out[0]) {
                            if(dump>2)dumplog("         %02x", out[0]);
                            out++;
                            l = 0;
                            switch(si_s) {
                                case 1: l = out[0]; out++; break;
                                case 2: l = *((uint16_t*)out); out += 2; break;
                                case 4: l = *((uint32_t*)out); out += 4; break;
                            }
                            dumplog("   use material ");
                            dumplog(si_s == 1 ? "%02x " : (si_s == 2 ? "%04x " : "%08x "), l);
                            dumplog("'%s'\n", l ? str + l : "");
                        } else
                        if(out[0] == 1) {
                            if(dump>2)dumplog("         %02x", out[0]);
                            out++;
                            l = 0;
                            switch(si_s) {
                                case 1: l = out[0]; out++; break;
                                case 2: l = *((uint16_t*)out); out += 2; break;
                                case 4: l = *((uint32_t*)out); out += 4; break;
                            }
                            dumplog("   use parameter ");
                            dumplog(si_s == 1 ? "%02x " : (si_s == 2 ? "%04x " : "%08x "), l);
                            dumplog("'%s'\n", l ? str + l : "");
                            np++;
                        } else {
                            if(dump>2)dumplog("  %5u. %02x", d, out[0]);
                            d++;
                            k = *out++;
                            l = k >> 4;
                            if(dump>2)dumplog(" (edge=%u%s%s%s) ", l, k & 1 ? ",uv" : "", k & 2 ? ",n" : "", k & 4 ? ",m" : "");
                            if(!nt && (k & 1)) { dumperr(stderr, "ERROR: UV index without texture map\n"); return 1; }
                            if(!np && (k & 4)) { dumperr(stderr, "ERROR: maximum vertex without parameter\n"); return 1; }
                            for(j = 0; j < l; j++) {
                                if(dump > 2) {
                                    if(j) dumplog(", ");
                                    m = nv;
                                    switch(vi_s) {
                                        case 1: dumplog("%02x (p=%u)", out[0], (uint8_t)out[0]); m = out[0]; out++; break;
                                        case 2: dumplog("%04x (p=%u)", *((uint16_t*)out), *((uint16_t*)out)); m = *((uint16_t*)out); out += 2; break;
                                        case 4: dumplog("%08x (p=%u)", *((uint32_t*)out), *((uint32_t*)out)); m = *((uint32_t*)out); out += 4; break;
                                    }
                                    if(m >= nv) { dumplog("\n"); dumperr(stderr, "ERROR: vertex index out of bound\n"); return 1; }
                                } else out += vi_s;
                                if(k & 1) {
                                    if(dump > 2) {
                                        m = nt;
                                        switch(ti_s) {
                                            case 1: dumplog(" %02x (uv=%u)", out[0], (uint8_t)out[0]); m = out[0]; out++; break;
                                            case 2: dumplog(" %04x (uv=%u)", *((uint16_t*)out), *((uint16_t*)out)); m = *((uint16_t*)out); out += 2; break;
                                            case 4: dumplog(" %08x (uv=%u)", *((uint32_t*)out), *((uint32_t*)out)); m = *((uint32_t*)out); out += 4; break;
                                        }
                                        if(m >= nt) { dumplog("\n"); dumperr(stderr, "ERROR: UV index out of bound\n"); return 1; }
                                    } else if(ti_s != 8) out += ti_s;
                                }
                                if(k & 2) {
                                    if(dump > 2) {
                                        m = nv;
                                        switch(vi_s) {
                                            case 1: dumplog(" %02x (n=%u)", out[0], (uint8_t)out[0]); m = out[0]; out++; break;
                                            case 2: dumplog(" %04x (n=%u)", *((uint16_t*)out), (int)*((uint16_t*)out)); m = *((uint16_t*)out); out += 2; break;
                                            case 4: dumplog(" %08x (n=%u)", *((uint32_t*)out), *((uint32_t*)out)); m = *((uint32_t*)out); out += 4; break;
                                        }
                                        if(m >= nv) { dumplog("\n"); dumperr(stderr, "ERROR: vertex index out of bound\n"); return 1; }
                                    } else out += vi_s;
                                }
                                if(k & 4) {
                                    if(dump > 2) {
                                        m = nv;
                                        switch(vi_s) {
                                            case 1: dumplog(" %02x (m=%u)", out[0], (uint8_t)out[0]); m = out[0]; out++; break;
                                            case 2: dumplog(" %04x (m=%u)", *((uint16_t*)out), *((uint16_t*)out)); m = *((uint16_t*)out); out += 2; break;
                                            case 4: dumplog(" %08x (m=%u)", *((uint32_t*)out), *((uint32_t*)out)); m = *((uint32_t*)out); out += 4; break;
                                        }
                                        if(m >= nv) { dumplog("\n"); dumperr(stderr, "ERROR: vertex index out of bound\n"); return 1; }
                                    } else out += vi_s;
                                }
                            }
                            if(dump > 2) dumplog("\n");
                        }
                    }
                    if(dump == 2) dumplog("  NumFace %u (use -ddd to dump all)\n", d);
                    if(out != chend) { dumperr(stderr, "ERROR: Bad chunk size\n"); return 1; }
                }
                dumplog("\n");
            } else

            if(M3D_CHUNKMAGIC(data, 'V','O','X','T')) {
                chend = data + ((m3dhdr_t*)data)->length;
                dumplog("Voxel Type Chunk\n  Magic:  VOXT\n"
                    "  Length: %u\n",
                    ((m3dhdr_t*)data)->length);
                if(!cmap && (ci_s==1 || ci_s==2)) { dumplog("\n"); dumperr(stderr, "ERROR: missing color map\n"); return 1; }
                for(i = 0; out < chend; i++) {
                    if(dump>1) {
                        dumplog("    ");
                        m = 0;
                        switch(ci_s) {
                            case 1: dumplog("%02x (#%08x) ", out[0], cmap[out[0]]); m = out[0]; out++; break;
                            case 2: dumplog("%04x (#%08x) ", *((uint16_t*)out), cmap[*((uint16_t*)out)]); m = *((uint16_t*)out); out += 2; break;
                            case 4: dumplog("%08x (#%08x) ", *((uint32_t*)out), *((uint32_t*)out)); out += 4; break;
                        }
                        if(m && m >= nc) { dumplog("\n"); dumperr(stderr, "ERROR: cmap index out of bound\n"); return 1; }
                    } else
                        out += ci_s == 8 ? 0 : ci_s;
                    if(dump>1) {
                        m = 0;
                        switch(si_s) {
                            case 1: m = out[0]; out++; break;
                            case 2: m = *((uint16_t*)out); out += 2; break;
                            case 4: m = *((uint32_t*)out); out += 4; break;
                        }
                        if(dump > 1)
                            dumplog(si_s == 1 ? "%02x " : (si_s == 2 ? "%04x " : "%08x "), m);
                        dumplog("'%s' ", m ? str + m : "");
                    } else
                        out += si_s;
                    if(dump>1) {
                        dumplog("%02x (", out[0]);
                        if(out[0] & 0x80) dumplog("height=%u/64", 1+ (out[0] & 0x3F));
                        else dumplog("rotate y=%u z=%u x=%u deg", (out[0] & 3) * 90, ((out[0]>>2) & 3) * 90, ((out[0]>>4) & 3) * 90);
                        dumplog(") %02x (", out[1]);
                        if(!out[1]) dumplog("full cube");
                        else if(out[1] < 8) dumplog("partial cube");
                        else if(out[1] == 8) dumplog("coloumn");
                        else if(out[1] == 9) dumplog("cross");
                        else if(out[1] == 10) dumplog("cylinder");
                        else if(out[1] == 11) dumplog("cylinder cross");
                        else if(out[1] == 12) dumplog("lego top");
                        else if(out[1] == 13) dumplog("lego bottom");
                        else if(out[1] < 32) dumplog("reserved");
                        else dumplog("user defined shape");
                        dumplog(") %02x (numitem=%u) ", out[2], out[2]);
                    }
                    l = out[2];
                    out += 3;
                    if(dump>1) {
                        switch(sk_s) {
                            case 1: dumplog("%02x (skin=%d)", out[0], (int8_t)out[0]); out++; break;
                            case 2: dumplog("%04x (skin=%d)", *((uint16_t*)out), *((int16_t*)out)); out += 2; break;
                            case 4: dumplog("%08x (skin=%d)", *((uint32_t*)out), *((int32_t*)out)); out += 4; break;
                        }
                    } else
                        out += sk_s == 8 ? 0 : sk_s;
                    if(dump>2) {
                        for(j = 0; j < l; j++) {
                            dumplog("        %04x (count=%u)", *((uint16_t*)out), *((uint16_t*)out)); out += 2;
                            m = 0;
                            switch(si_s) {
                                case 1: m = out[0]; out++; break;
                                case 2: m = *((uint16_t*)out); out += 2; break;
                                case 4: m = *((uint32_t*)out); out += 4; break;
                            }
                            dumplog(si_s == 1 ? "%02x " : (si_s == 2 ? "%04x " : "%08x "), m);
                            dumplog("'%s'\n", m ? str + m : "");
                        }
                    } else
                        out += l * (2 + si_s);
                    if(dump>1) dumplog("\n");
                }
                if(dump < 2) dumplog("  #VoxelType %u (use -ddd to dump all)\n", i);
                if(out != chend) { dumperr(stderr, "ERROR: Bad chunk size\n"); return 1; }
                nvt = i;
                dumplog("\n");
            } else

            if(M3D_CHUNKMAGIC(data, 'V','O','X','D')) {
                chend = data + ((m3dhdr_t*)data)->length;
                i = 0;
                switch(si_s) {
                    case 1: i = out[0]; out++; break;
                    case 2: i = *((uint16_t*)out); out += 2; break;
                    case 4: i = *((uint32_t*)out); out += 4; break;
                }
                dumplog("Voxel Data Chunk\n  Magic:  VOXD\n"
                    "  Length: %u\n"
                    "  Name:   ",
                    ((m3dhdr_t*)data)->length);
                if(dump > 1)
                    dumplog(si_s == 1 ? "%02x " : (si_s == 2 ? "%04x " : "%08x "), i);
                dumplog("'%s'\n", i ? str + i : "");
                if(vd_s > 4 || vp_s >= 4) { dumperr(stderr, "ERROR: there should be no voxel data\n"); }
                if(!nvt) { dumplog("\n"); dumperr(stderr, "ERROR: voxel data without voxel type list\n"); return 1; }
                if(dump>1) {
                    dumplog("  Pos:    ");
                    i = j = k = 0;
                    switch(vd_s) {
                        case 1:
                            dumplog("%02x %02x %02x", (int8_t)out[0], (int8_t)out[1], (int8_t)out[2]);
                            i = (int8_t)out[0]; out++;
                            j = (int8_t)out[0]; out++;
                            k = (int8_t)out[0]; out++;
                        break;
                        case 2:
                            dumplog("%04x %04x %04x", *((int16_t*)(out+0)), *((int16_t*)(out+2)), *((int16_t*)(out+4)));
                            i = *((int16_t*)out); out += 2;
                            j = *((int16_t*)out); out += 2;
                            k = *((int16_t*)out); out += 2;
                        break;
                        case 4:
                            dumplog("%08x %08x %08x", *((int32_t*)(out+0)), *((int32_t*)(out+4)), *((int32_t*)(out+8)));
                            i = *((int32_t*)out); out += 4;
                            j = *((int32_t*)out); out += 4;
                            k = *((int32_t*)out); out += 4;
                        break;
                    }
                    dumplog(" (x=%d) (y=%d) (z=%d)\n  Size:   ", i, j, k);
                } else
                    out += 3 * vd_s;
                i = j = k = 0;
                switch(vd_s) {
                    case 1:
                        if(dump>1) dumplog("%02x %02x %02x", (int8_t)out[0], (int8_t)out[1], (int8_t)out[2]);
                        i = (int8_t)out[0]; out++;
                        j = (int8_t)out[0]; out++;
                        k = (int8_t)out[0]; out++;
                    break;
                    case 2:
                        if(dump>1) dumplog("%04x %04x %04x", *((int16_t*)(out+0)), *((int16_t*)(out+2)), *((int16_t*)(out+4)));
                        i = *((int16_t*)out); out += 2;
                        j = *((int16_t*)out); out += 2;
                        k = *((int16_t*)out); out += 2;
                    break;
                    case 4:
                        if(dump>1) dumplog("%08x %08x %08x", *((int32_t*)(out+0)), *((int32_t*)(out+4)), *((int32_t*)(out+8)));
                        i = *((int32_t*)out); out += 4;
                        j = *((int32_t*)out); out += 4;
                        k = *((int32_t*)out); out += 4;
                    break;
                }
                if(dump>1) {
                    dumplog(" (width=%u) (height=%u) (depth=%u)\n  Group:  %02x (uncertain=%u) %02x (groupid=%u)\n", i, j, k,
                        out[0], out[0], out[1], out[1]);
                    if(dump>2) dumplog("    ");
                }
                out += 2;
                l = i * j * k;
                k = vp_s == 1 ? 0xFE : 0xFFFE;
                for(i = 0; i < l && out < chend;) {
                    j = *out++;
                    i += (j & 0x7F) + 1;
                    if(j & 0x80) {
                        if(dump>2) {
                            dumplog("%u*", (j & 0x7F)+1);
                            m = nvt;
                            switch(vp_s) {
                                case 1: dumplog("%02x ", out[0]); m = out[0]; out++; break;
                                case 2: dumplog("%04x ", *((uint16_t*)out)); m = *((uint16_t*)out); out += 2; break;
                            }
                            if(m >= nvt && m < k) { dumplog("\n"); dumperr(stderr, "ERROR: voxel type index out of bound\n"); return 1; }
                        } else
                            out += vp_s;
                    } else {
                        if(dump>2) {
                            for(j++; j; j--) {
                                m = nvt;
                                switch(vp_s) {
                                    case 1: dumplog("%02x ", out[0]); m = out[0]; out++; break;
                                    case 2: dumplog("%04x ", *((uint16_t*)out)); m = *((uint16_t*)out); out += 2; break;
                                }
                                if(m >= nvt && m < k) { dumplog("\n"); dumperr(stderr, "ERROR: voxel type index out of bound\n"); return 1; }
                            }
                        } else
                            out += (j+1) * vp_s;
                    }
                }
                if(dump>2) dumplog("\n");
                dumplog("\n");
                if(out != chend || i != l) { dumperr(stderr, "\nERROR: Bad chunk size\n"); return 1; }
            } else

            if(M3D_CHUNKMAGIC(data, 'S','H','P','E')) {
                chend = data + ((m3dhdr_t*)data)->length;
                i = 0;
                switch(si_s) {
                    case 1: i = out[0]; out++; break;
                    case 2: i = *((uint16_t*)out); out += 2; break;
                    case 4: i = *((uint32_t*)out); out += 4; break;
                }
                j = -1;
                switch(bi_s) {
                    case 1: j = (int8_t)out[0]; out++; break;
                    case 2: j = *((int16_t*)out); out += 2; break;
                    case 4: j = *((int32_t*)out); out += 4; break;
                }
                if(j != -1 && j >= nb) { dumplog("\n"); dumperr(stderr, "ERROR: bone index out of bound\n"); return 1; }
                dumplog("Shape Chunk\n  Magic:  SHPE\n"
                    "  Length: %u\n"
                    "  Group:  %d\n"
                    "  Name:   ",
                    ((m3dhdr_t*)data)->length, j);
                if(dump > 1)
                    dumplog(si_s == 1 ? "%02x " : (si_s == 2 ? "%04x " : "%08x "), i);
                dumplog("'%s'\n", i ? str + i : "");
                if(((m3dhdr_t*)data)->length < 8 + si_s + 1) { dumperr(stderr, "ERROR: Bad chunk size\n"); return 1; }
                if(dump > 1) {
                    while(out < chend) {
                        for(i = 0, j = 256; i < (int)(sizeof(m3d_propertytypes)/sizeof(m3d_propertytypes[0])); i++)
                            if(m3d_propertytypes[i].id == out[0] ||
                                m3d_propertytypes[i].id + 128 == out[0]) { j = i; break; }
                        k = *out++;
                        if(k & 0x80) { k &= 0x7F; k |= (*out++ << 7); }
                        if(k >= (int)(sizeof(m3d_commandtypes)/sizeof(m3d_commandtypes[0])))
                            { dumplog("    %04x unknown command?\n", k); break; }
                        cd = &m3d_commandtypes[k];
                        if(k < 128) dumplog("    %02x ", out[-1]);
                        else dumplog("    %02x %02x ", out[-2], out[-1]);
#ifdef M3D_ASCII
                        dumplog("m3dc_%s ", cd->key);
#endif
                        for(k = n = 0, l = cd->p; k < l; k++)
                            switch(cd->a[((k - n) % (cd->p - n)) + n]) {
                                case m3dcp_mi_t:
                                    i = 0;
                                    switch(si_s) {
                                        case 1: i = out[0]; out++; break;
                                        case 2: i = *((uint16_t*)out); out += 2; break;
                                        case 4: i = *((uint32_t*)out); out += 4; break;
                                    }
                                    dumplog(si_s == 1 ? "%02x " : (si_s == 2 ? "%04x " : "%08x "), i);
                                    dumplog("(material='%s') ", i ? str + i : "");
                                break;
                                case m3dcp_hi_t:
                                    switch(hi_s) {
                                        case 1: dumplog("%02x (shape=%u) ", out[0], out[0]); out++; break;
                                        case 2: dumplog("%04x (shape=%u) ", *((uint16_t*)out), *((uint16_t*)out)); out += 2; break;
                                        case 4: dumplog("%08x (shape=%u) ", *((uint32_t*)out), *((uint32_t*)out)); out += 4; break;
                                    }
                                break;
                                case m3dcp_fi_t:
                                    switch(fi_s) {
                                        case 1: dumplog("%02x (face=%u) ", out[0], out[0]); out++; break;
                                        case 2: dumplog("%04x (face=%u) ", *((uint16_t*)out), *((uint16_t*)out)); out += 2; break;
                                        case 4: dumplog("%08x (face=%u) ", *((uint32_t*)out), *((uint32_t*)out)); out += 4; break;
                                    }
                                break;
                                case m3dcp_vi_t:
                                    m = nv;
                                    switch(vi_s) {
                                        case 1: dumplog("%02x (pos=%u) ", out[0], out[0]); m = out[0]; out++; break;
                                        case 2: dumplog("%04x (pos=%u) ", *((uint16_t*)out), *((uint16_t*)out)); m = *((uint16_t*)out); out += 2; break;
                                        case 4: dumplog("%08x (pos=%u) ", *((uint32_t*)out), *((uint32_t*)out)); m = *((uint32_t*)out); out += 4; break;
                                    }
                                    if(m >= nv) { dumplog("\n"); dumperr(stderr, "ERROR: vertex index out of bound\n"); return 1; }
                                break;
                                case m3dcp_qi_t:
                                    m = nv;
                                    switch(vi_s) {
                                        case 1: dumplog("%02x (ori=%u) ", out[0], out[0]); m = out[0]; out++; break;
                                        case 2: dumplog("%04x (ori=%u) ", *((uint16_t*)out), *((uint16_t*)out)); m = *((uint16_t*)out); out += 2; break;
                                        case 4: dumplog("%08x (ori=%u) ", *((uint32_t*)out), *((uint32_t*)out)); m = *((uint32_t*)out); out += 4; break;
                                    }
                                    if(m >= nv) { dumplog("\n"); dumperr(stderr, "ERROR: vertex index out of bound\n"); return 1; }
                                break;
                                case m3dcp_ti_t:
                                    m = nt;
                                    switch(ti_s) {
                                        case 1: dumplog("%02x (uv=%u) ", out[0], out[0]); m = out[0]; out++; break;
                                        case 2: dumplog("%04x (uv=%u) ", *((uint16_t*)out), *((uint16_t*)out)); m = *((uint16_t*)out); out += 2; break;
                                        case 4: dumplog("%08x (uv=%u) ", *((uint32_t*)out), *((uint32_t*)out)); m = *((uint32_t*)out); out += 4; break;
                                    }
                                    if(m >= nt) { dumplog("\n"); dumperr(stderr, "ERROR: tmap index out of bound\n"); return 1; }
                                break;
                                case m3dcp_vc_t:
                                    switch(vc_s) {
                                        case 1: dumplog("%02x (%g) ", out[0], (float)((int8_t)out[0]) / 127); break;
                                        case 2:
                                            dumplog("%04x (%g) ", *((int16_t*)(out+0)), (float)(*((int16_t*)(out+0))) / 32767);
                                        break;
                                        case 4: dumplog("%08x (%g) ", *((int32_t*)(out+0)), *((float*)(out+0))); break;
                                        case 8: dumplog("(%g) ", *((double*)(out+0))); break;
                                    }
                                    out += vc_s;
                                break;
                                case m3dcp_i1_t: dumplog("%02x (%d) ", *((uint8_t*)out), *((int8_t*)out)); out++; break;
                                case m3dcp_i2_t: dumplog("%04x (%d) ", *((uint16_t*)out), *((int16_t*)out)); out += 2; break;
                                case m3dcp_i4_t: dumplog("%08x (%d) ", *((uint32_t*)out), *((int32_t*)out)); out += 4; break;
                                case m3dcp_va_t: dumplog("%08x (va_args=%u) ", *((uint32_t*)out), *((int32_t*)out));
                                    m = *((int32_t*)out); out += 4; n = k + 1; l += (m - 1) * (cd->p - k - 1);
                                break;
                                default: dumplog("\n"); dumperr(stderr, "ERROR: unknown command argument type\n"); return 1;
                            }
                        dumplog("\n");
                    }
                }
                dumplog("\n");
            } else

            if(M3D_CHUNKMAGIC(data, 'L','B','L','S')) {
                chend = data + ((m3dhdr_t*)data)->length;
                i = 0;
                switch(si_s) {
                    case 1: i = out[0]; out++; break;
                    case 2: i = *((uint16_t*)out); out += 2; break;
                    case 4: i = *((uint32_t*)out); out += 4; break;
                }
                dumplog("Labels Chunk\n  Magic:  LBLS\n"
                    "  Length: %u\n"
                    "  Group:  ",
                    ((m3dhdr_t*)data)->length);
                if(dump > 1)
                    dumplog(si_s == 1 ? "%02x " : (si_s == 2 ? "%04x " : "%08x "), i);
                dumplog("'%s'\n", i ? str + i : "");
                i = 0;
                switch(si_s) {
                    case 1: i = out[0]; out++; break;
                    case 2: i = *((uint16_t*)out); out += 2; break;
                    case 4: i = *((uint32_t*)out); out += 4; break;
                }
                dumplog("  Lang:   ");
                if(dump > 1)
                    dumplog(si_s == 1 ? "%02x " : (si_s == 2 ? "%04x " : "%08x "), i);
                dumplog("'%s'\n", i ? str + i : "");
                if(!cmap && (ci_s==1 || ci_s==2)) { dumplog("\n"); dumperr(stderr, "ERROR: missing color map\n"); return 1; }
                m = 0;
                switch(ci_s) {
                    case 1: dumplog("  Color:  %02x (#%08x)\n", out[0], cmap[out[0]]); m = out[0]; out++; break;
                    case 2: dumplog("  Color:  %04x (#%08x)\n", *((uint16_t*)out), cmap[*((uint16_t*)out)]); m = *((uint16_t*)out); out += 2; break;
                    case 4: dumplog("  Color:  %08x (#%08x)\n", *((uint32_t*)out), *((uint32_t*)out)); out += 4; break;
                }
                if(m && m >= nc) { dumplog("\n"); dumperr(stderr, "ERROR: cmap index out of bound\n"); return 1; }
                i = vi_s + si_s;
                j = (((m3dhdr_t*)data)->length-8)/i;
                if(dump == 2) dumplog("  Numlabel: %u, reclen %u (use -ddd to dump all)\n", j, i);
                if(((m3dhdr_t*)data)->length != 8 + 2 * si_s + (ci_s!=8 ? ci_s : 0) + j * i)
                    { dumperr(stderr, "ERROR: Bad chunk size\n"); return 1; }
                if(dump > 2) {
                    while(out < chend) {
                        dumplog("    ");
                        m = nv;
                        switch(vi_s) {
                            case 1: dumplog("%02x (vertex=%u) ", out[0], out[0]); m = out[0]; out++; break;
                            case 2: dumplog("%04x (vertex=%u) ", *((uint16_t*)out), *((uint16_t*)out)); m = *((uint16_t*)out); out += 2; break;
                            case 4: dumplog("%08x (vertex=%u) ", *((uint32_t*)out), *((uint32_t*)out)); m = *((uint32_t*)out); out += 4; break;
                        }
                        if(m >= nv) { dumplog("\n"); dumperr(stderr, "ERROR: vertex index out of bound\n"); return 1; }
                        i = 0;
                        switch(si_s) {
                            case 1: i = out[0]; out++; break;
                            case 2: i = *((uint16_t*)out); out += 2; break;
                            case 4: i = *((uint32_t*)out); out += 4; break;
                        }
                        dumplog(si_s == 1 ? "%02x " : (si_s == 2 ? "%04x " : "%08x "), i);
                        dumplog("'%s'\n", i ? str + i : "");
                    }
                }
                dumplog("\n");
            } else

            if(M3D_CHUNKMAGIC(data, 'A','C','T','N')) {
                chend = data + ((m3dhdr_t*)data)->length;
                i = 0;
                switch(si_s) {
                    case 1: i = out[0]; out++; break;
                    case 2: i = *((uint16_t*)out); out += 2; break;
                    case 4: i = *((uint32_t*)out); out += 4; break;
                }
                j = *((uint16_t*)out); out += 2;
                d = *((uint32_t*)out); out += 4;
                dumplog("Action Chunk\n  Magic:    ACTN\n"
                    "  Length:   %u\n"
                    "  Name:     ",
                    ((m3dhdr_t*)data)->length);
                if(dump > 1)
                    dumplog(si_s == 1 ? "%02x " : (si_s == 2 ? "%04x " : "%08x "), i);
                dumplog("'%s'\n"
                    "  #Frames:  %u\n"
                    "  Duration: %g sec\n",  i ? str + i : "", j, ((float)d)/1000);
                if(j < 1 || d < 10) { dumperr(stderr, "ERROR: Bad duration or bad number of frames\n"); return 1; }
                if(bi_s >= 4 || fc_s >= 4) { dumperr(stderr, "ERROR: there should be no action chunk\n"); }
                if(!nv) { dumperr(stderr, "ERROR: action chunk without vertex list\n"); return 1; }
                if(((m3dhdr_t*)data)->length < 8 + si_s + 6 + j * (4 + fc_s + 2 * vi_s))
                    { dumperr(stderr, "ERROR: Bad chunk size\n"); return 1; }
                if(dump > 1) {
                    for(i = 0; i < j; i++) {
                        l = *((uint32_t*)out); out += 4;
                        k = 0;
                        switch(fc_s) {
                            case 1: k = out[0]; out++; break;
                            case 2: k = *((uint16_t*)out); out += 2; break;
                            case 4: k = *((uint32_t*)out); out += 4; break;
                        }
                        dumplog("    Frame #%u at %08x (%g sec) ", i+1, l, ((float)l)/1000);
                        dumplog(fc_s == 1 ? "%02x " : (fc_s == 2 ? "%04x " : "%08x "), k);
                        dumplog("(%u transforms)\n", k);
                        if(l > d) { dumperr(stderr, "ERROR: invalid frame timestamp\n"); return 1; }
                        if(k == 0 || out + k * (fc_s + 2 * vi_s) > chend)
                            { dumperr(stderr, "ERROR: invalid number of transforms\n"); return 1; }
                        for(l = 0; l < k; l++) {
                            dumplog("      ");
                            m = nb;
                            switch(bi_s) {
                                case 1: dumplog("%02x (bone=%u) ", out[0], out[0]); m = out[0]; out++; break;
                                case 2: dumplog("%04x (bone=%u) ", *((uint16_t*)out), *((uint16_t*)out)); m = *((uint16_t*)out); out += 2; break;
                                case 4: dumplog("%08x (bone=%u) ", *((uint32_t*)out), *((uint32_t*)out)); m = *((uint32_t*)out); out += 4; break;
                            }
                            if(m >= nb) { dumplog("\n"); dumperr(stderr, "ERROR: bone index out of bound\n"); return 1; }
                            switch(vi_s) {
                                case 1:
                                    dumplog("%02x (pos=%u) ", out[0], out[0]); m = out[0]; out++;
                                    if(m >= nv) { dumplog("\n"); dumperr(stderr, "ERROR: vertex index out of bound\n"); return 1; }
                                    dumplog("%02x (ori=%u)\n", out[0], out[0]); m = out[0]; out++;
                                    if(m >= nv) { dumplog("\n"); dumperr(stderr, "ERROR: vertex index out of bound\n"); return 1; }
                                break;
                                case 2:
                                    dumplog("%04x (pos=%u) ", *((uint16_t*)out), *((uint16_t*)out)); m = *((uint16_t*)out); out += 2;
                                    if(m >= nv) { dumplog("\n"); dumperr(stderr, "ERROR: vertex index out of bound\n"); return 1; }
                                    dumplog("%04x (ori=%u)\n", *((uint16_t*)out), *((uint16_t*)out)); m = *((uint16_t*)out); out += 2;
                                    if(m >= nv) { dumplog("\n"); dumperr(stderr, "ERROR: vertex index out of bound\n"); return 1; }
                                break;
                                case 4:
                                    dumplog("%08x (pos=%u) ", *((uint32_t*)out), *((uint32_t*)out)); m = *((uint32_t*)out); out += 4;
                                    if(m >= nv) { dumplog("\n"); dumperr(stderr, "ERROR: vertex index out of bound\n"); return 1; }
                                    dumplog("%08x (ori=%u)\n", *((uint32_t*)out), *((uint32_t*)out)); m = *((uint32_t*)out); out += 4;
                                    if(m >= nv) { dumplog("\n"); dumperr(stderr, "ERROR: vertex index out of bound\n"); return 1; }
                                break;
                            }
                        }
                    }
                    if(out != chend) { dumperr(stderr, "ERROR: Bad chunk size\n"); return 1; }
                }
                dumplog("\n");
            } else

            if(M3D_CHUNKMAGIC(data, 'A','S','E','T')) {
                i = 0;
                switch(si_s) {
                    case 1: i = out[0]; out++; break;
                    case 2: i = *((uint16_t*)out); out += 2; break;
                    case 4: i = *((uint32_t*)out); out += 4; break;
                }
                dumplog("Inlined Asset Chunk\n  Magic:  ASET\n"
                    "  Length: %u\n"
                    "  Name:   ",
                    ((m3dhdr_t*)data)->length);
                if(dump > 1)
                    dumplog(si_s == 1 ? "%02x " : (si_s == 2 ? "%04x " : "%08x "), i);
                dumplog("'%s'\n", i ? str + i : "");
                if(((m3dhdr_t*)data)->length < 8 + si_s + 1) { dumperr(stderr, "ERROR: Bad chunk size\n"); return 1; }
                dumplog("\n");
            } else
                dumplog("Extra Chunk\n  Magic:  %c%c%c%c\n"
                    "  Length: %u\n\n",
                    data[0], data[1], data[2], data[3], ((m3dhdr_t*)data)->length);

            data += len;
        }
    } else {
        dumperr(stderr, "ERROR: not a valid Model 3D binary\n");
        return 1;
    }
    dumperr(stderr, "Model parsed OK.\n");
    if(zip) free(zip);
    return 0;
}

/**
 * Dump in-memory representation
 */
void dump_cstruct(m3d_t *model)
{
    unsigned int i, j, k, l, n;
    m3dcd_t *cd;

    if(!model) {
        dumplog("m3d_load() returned NULL, probably memory allocation error\n");
        return;
    }
    dumplog("m3d_t = {\n"
        "  raw:0x%lx, flags:0x%x, errcode:%d (fatal:%s),\n"
        , (uintptr_t)model->raw, model->flags, model->errcode, M3D_ERR_ISFATAL(model->errcode) ? "true" : "false");
    dumplog("  vc_s:%d, vi_s:%d, si_s:%d, ci_s:%d, ti_s:%d, bi_s:%d, nb_s:%d, sk_s:%d, fc_s:%d, hi_s:%d, fi_s:%d\n",
        model->vc_s, model->vi_s, model->si_s, model->ci_s, model->ti_s, model->bi_s,
        model->nb_s, model->sk_s, model->fc_s, model->hi_s, model->fi_s);
    dumplog("  name:\"%s\"\n  license:\"%s\"\n  author:\"%s\"\n  desc:\"%s\"\n  scale:%g\n",
        model->name, model->license, model->author, model->desc, model->scale);
    dumplog("  numcmap:%d, cmap:0x%lx,\n  numtmap:%d, tmap:0x%lx,\n  numtexture:%d, texture:0x%lx,\n  numbone:%d, bone:0x%lx,\n",
        model->numcmap, (uintptr_t)model->cmap, model->numtmap, (uintptr_t)model->tmap,
        model->numtexture, (uintptr_t)model->texture, model->numbone, (uintptr_t)model->bone);
    dumplog("  numvertex:%d, vertex:0x%lx,\n  numskin:%d, skin:0x%lx,\n  nummaterial:%d, material:0x%lx,\n"
        "  numface:%d, face:0x%lx,\n", model->numvertex, (uintptr_t)model->vertex, model->numskin, (uintptr_t)model->skin,
        model->nummaterial, (uintptr_t)model->material, model->numface, (uintptr_t)model->face);
    dumplog("  numvoxtype:%d, voxtype:0x%lx,\n  numvoxel:%d, voxel:0x%lx,\n",
        model->numvoxtype, (uintptr_t)model->voxtype, model->numvoxel, (uintptr_t)model->voxel);
    dumplog("  numshape:%d, shape:0x%lx,\n  numlabel:%d, label:0x%lx,\n  numaction:%d, action:0x%lx,"
        "\n  numinlined:%d, inlined:0x%lx,\n  numextra:%d, extra:0x%lx,\n  preview.length:%d, preview.data:0x%lx\n",
        model->numshape, (uintptr_t)model->shape, model->numlabel, (uintptr_t)model->label, model->numaction,
        (uintptr_t)model->action, model->numinlined, (uintptr_t)model->inlined, model->numextra, (uintptr_t)model->extra,
        model->preview.length, (uintptr_t)model->preview.data);
    dumplog("}\n\n");

    if(model->preview.length && model->preview.data) {
        dumplog("m3d_t.preview.data[%d] = {", model->preview.length);
        for(i = 0; i < 8; i++) dumplog("%s0x%02x(%c)", i ? ", ":" ", model->preview.data[i], model->preview.data[i] < 32 ||
            model->preview.data[i] > 127 ? '.' : model->preview.data[i]);
        dumplog("... }\n\n");
    }

    if(model->numcmap && model->cmap && model->ci_s != 8) {
        dumplog("m3d_t.cmap[%d] = {", model->numcmap);
        for(i = 0; i < model->numcmap; i++) dumplog("%s0x%08x", i ? ", ":" ", model->cmap[i]);
        dumplog(" }\n\n");
    }

    if(model->numtmap && model->tmap) {
        dumplog("m3d_t.tmap[%d] = {", model->numtmap);
        for(i = 0; i < model->numtmap; i++)
            dumplog("%s\n  /* %d. */ { u:%g, v:%g }", i ? ", ":" ", i, model->tmap[i].u, model->tmap[i].v);
        dumplog("\n}\n\n");
    }

    if(model->numvertex && model->vertex) {
        dumplog("m3d_t.vertex[%d] = {", model->numvertex);
        for(i = 0; i < model->numvertex; i++)
            dumplog("%s\n  /* %d. */ { x:%g, y:%g, z:%g, w:%g, color:0x%08x, skinid:%d }", i ? ", ":" ", i,
                model->vertex[i].x, model->vertex[i].y, model->vertex[i].z, model->vertex[i].w,
                model->vertex[i].color, model->vertex[i].skinid);
        dumplog("\n}\n\n");
    }

    if(model->numface && model->face) {
        dumplog("m3d_t.face[%d] = {", model->numface);
        for(i = 0; i < model->numface; i++)
#ifdef M3D_VERTEXMAX
            dumplog("%s\n  { materialid:%d, paramid:%d, vertex[3]={%d,%d,%d}, vertmax[3]={%d,%d,%d}, normal[3]={%d,%d,%d}, "
                "texcoord[3]={%d,%d,%d} }", i ? ", ":" ",
                model->face[i].materialid, model->face[i].paramid,
                model->face[i].vertex[0], model->face[i].vertex[1], model->face[i].vertex[2],
                model->face[i].vertmax[0], model->face[i].vertmax[1], model->face[i].vertmax[2],
                model->face[i].normal[0], model->face[i].normal[1], model->face[i].normal[2],
                model->face[i].texcoord[0], model->face[i].texcoord[1], model->face[i].texcoord[2]);
#else
            dumplog("%s\n  { materialid:%d, vertex[3]={%d,%d,%d}, normal[3]={%d,%d,%d}, texcoord[3]={%d,%d,%d} }", i ? ", ":" ",
                model->face[i].materialid, model->face[i].vertex[0], model->face[i].vertex[1], model->face[i].vertex[2],
                model->face[i].normal[0], model->face[i].normal[1], model->face[i].normal[2],
                model->face[i].texcoord[0], model->face[i].texcoord[1], model->face[i].texcoord[2]);
#endif
        dumplog("\n}\n\n");
    }

    if(model->numvoxtype && model->voxtype) {
        dumplog("m3d_t.voxtype[%d] = {", model->numvoxtype);
        for(i = 0; i < model->numvoxtype; i++) {
            dumplog("%s\n  /* %d. */ { name:\"%s\", rotation:%02x, voxshape:%03x, materialid:%d, color:0x%08x, skinid:%d, "
                "item[%d] = {", i ? ", ":" ", i, model->voxtype[i].name, model->voxtype[i].rotation,
                model->voxtype[i].voxshape, model->voxtype[i].materialid, model->voxtype[i].color, model->voxtype[i].skinid,
                model->voxtype[i].numitem);
            if(model->voxtype[i].numitem && model->voxtype[i].item) {
                for(j = 0; j < model->voxtype[i].numitem; j++)
                    dumplog("%s{count:%d, name:\"%s\"}", j ? ", ":" ",model->voxtype[i].item[j].count,
                        model->voxtype[i].item[j].name);
            }
            dumplog("} }");
        }
        dumplog("\n}\n\n");
    }

    if(model->numvoxel && model->voxel) {
        dumplog("m3d_t.voxel[%d] = {", model->numvoxel);
        for(i = 0; i < model->numvoxel; i++)
            dumplog("%s\n  { name:\"%s\", x:%d, y:%d, z:%d, w:%d, h:%d, d:%d, uncertain:%d, groupid:%d, data:%ld bytes }",
                i ? ", ":" ", model->voxel[i].name, model->voxel[i].x, model->voxel[i].y, model->voxel[i].z,
                model->voxel[i].w, model->voxel[i].h, model->voxel[i].d, model->voxel[i].uncertain, model->voxel[i].groupid,
                model->voxel[i].w * model->voxel[i].h * model->voxel[i].d * sizeof(M3D_VOXEL));
        dumplog("\n}\n\n");
    }

    if(model->numshape && model->shape) {
        dumplog("m3d_t.shape[%d] = {", model->numshape);
        for(i = 0; i < model->numshape; i++) {
            dumplog("%s\n  /* %d. */ { name:\"%s\", group:%d, numcmd:%d, cmd[%d] = {", i ? ", ":" ", i,
                model->shape[i].name, model->shape[i].group, model->shape[i].numcmd, model->shape[i].numcmd);
            for(j = 0; j < model->shape[i].numcmd; j++) {
                dumplog("%s\n      { type:%d", j ? ", ":" ", model->shape[i].cmd[j].type);
                if(model->shape[i].cmd[j].type >= (unsigned int)(sizeof(m3d_commandtypes)/sizeof(m3d_commandtypes[0])))
                    { dumplog(", unknown command type? }"); continue; }
                if(!model->shape[i].cmd[j].arg) { dumplog("arg = NULL? }"); continue; }
                cd = &m3d_commandtypes[model->shape[i].cmd[j].type];
#ifdef M3D_ASCII
                dumplog(" m3dc_%s", cd->key);
#endif
                dumplog(", arg[%d] = { ", cd->p);
                for(k = n = 0, l = cd->p; k < l; k++) {
                    if(j) dumplog(", ");
                    if(cd->a[k < cd->p ? k : (k % (cd->p - 1)) + n] == m3dcp_vc_t)
                        dumplog("%g", *((float*)&model->shape[i].cmd[j].arg[k]));
                    else
                        dumplog("%d", model->shape[i].cmd[j].arg[k]);
                    if(cd->a[k < cd->p ? k : (k % (cd->p - 1)) + n] == m3dcp_va_t) {
                        n = k + 1; l += model->shape[i].cmd[j].arg[k] * (cd->p - k - 1) - 1;
                    }
                }
                dumplog(" }");
            }
            dumplog("\n    }\n  }");
        }
        dumplog("\n}\n\n");
    }

    if(model->numlabel && model->label) {
        dumplog("m3d_t.label[%d] = {", model->numlabel);
        for(i = 0; i < model->numlabel; i++)
            dumplog("%s\n  { name:\"%s\", lang:\"%s\", color:0x%08x, vertexid:%d, text:\"%s\" }", i ? ", ":" ",
              model->label[i].name, model->label[i].lang, model->label[i].color, model->label[i].vertexid, model->label[i].text);
        dumplog("\n}\n\n");
    }

    if(model->numtexture && model->texture) {
        dumplog("m3d_t.texture[%d] = {", model->numtexture);
        for(i = 0; i < model->numtexture; i++)
            dumplog("%s\n  /* %d. */ { name:\"%s\", w:%d, h:%d, d:%lx }", i ? ", ":" ", i,
                model->texture[i].name, model->texture[i].w, model->texture[i].h, (uintptr_t)model->texture[i].d);
        dumplog("\n}\n\n");
    }

    if(model->numbone && model->bone) {
        dumplog("m3d_t.bone[%d] = {", model->numbone);
        for(i = 0; i < model->numbone; i++)
            dumplog("%s\n  /* %d. */ { parent:%d, name:\"%s\", pos:%d, ori:%d }", i ? ", ":" ", i,
                model->bone[i].parent, model->bone[i].name, model->bone[i].pos, model->bone[i].ori);
        dumplog("\n}\n\n");
    }

    if(model->numskin && model->skin) {
        dumplog("m3d_t.skin[%d] = {", model->numskin);
        for(i = 0; i < model->numskin; i++) {
            dumplog("%s\n  /* %d. */ { boneid[%d] = { ", i ? ", ":" ", i, M3D_NUMBONE);
            for(j = 0; j < M3D_NUMBONE; j++) dumplog("%s%d", j ? ", ":" ", model->skin[i].boneid[j]);
            dumplog(" }, weight[%d] = {", M3D_NUMBONE);
            for(j = 0; j < M3D_NUMBONE; j++) dumplog("%s%g", j ? ", ":" ", model->skin[i].weight[j]);
            dumplog(" } }");
        }
        dumplog("\n}\n\n");
    }

    if(model->nummaterial && model->material) {
        dumplog("m3d_t.material[%d] = {", model->nummaterial);
        for(i = 0; i < model->nummaterial; i++) {
            dumplog("%s\n  /* %d. */ { name:\"%s\", numprop:%d, prop[%d] = {", i ? ", ":" ", i,
                model->material[i].name, model->material[i].numprop, model->material[i].numprop);
            for(j = 0; j < model->material[i].numprop; j++) {
                dumplog("%s\n      { type:%d ", j ? ", ":" ", model->material[i].prop[j].type);
                for(k = 0, l = 256; k < (int)(sizeof(m3d_propertytypes)/sizeof(m3d_propertytypes[0])); k++)
                    if(m3d_propertytypes[k].id == model->material[i].prop[j].type ||
                        m3d_propertytypes[k].id + 128 == model->material[i].prop[j].type) { l = k; break; }
                if(l == 256) { dumplog("unknown property? value.num:0x%x }", model->material[i].prop[j].value.num); break; }
                k = model->material[i].prop[j].type >= 128 ? m3dpf_map : m3d_propertytypes[l].format;
#ifdef M3D_ASCII
                dumplog("m3dp_%s%s, value.", model->material[i].prop[j].type >= 128 ? "map_" : "", m3d_propertytypes[l].key);
#else
                dumplog(", value.");
#endif
                switch(k) {
                    case m3dpf_color: dumplog("color:0x%08x }", model->material[i].prop[j].value.color); break;
                    case m3dpf_float: dumplog("fnum:%g }", model->material[i].prop[j].value.fnum); break;
                    case m3dpf_map: dumplog("textureid:%d }", model->material[i].prop[j].value.textureid); break;
                    default: dumplog("num:%d }", model->material[i].prop[j].value.num); break;
                }
            }
            dumplog("\n    }\n  }");
        }
        dumplog("\n}\n\n");
    }

    if(model->numaction && model->action) {
        dumplog("m3d_t.action[%d] = {", model->numaction);
        for(i = 0; i < model->numaction; i++) {
            dumplog("%s\n  /* %d. */ { name:\"%s\", durationmsec:%d, numframe:%d, frame[%d] = {", i ? ", ":" ", i,
                model->action[i].name, model->action[i].durationmsec, model->action[i].numframe, model->action[i].numframe);
            for(j = 0; j < model->action[i].numframe; j++) {
                dumplog("%s\n      { msec:%d, numtransform:%d, transform[%d] = {", j ? ", ":" ", model->action[i].frame[j].msec,
                    model->action[i].frame[j].numtransform, model->action[i].frame[j].numtransform);
                for(k = 0; k < model->action[i].frame[j].numtransform; k++)
                    dumplog("%s\n          { boneid:%d, pos:%d, ori:%d }", k ? ", ":" ",
                        model->action[i].frame[j].transform[k].boneid,
                        model->action[i].frame[j].transform[k].pos, model->action[i].frame[j].transform[k].ori);
                dumplog("\n        }\n      }");
            }
            dumplog("\n    }\n  }");
        }
        dumplog("\n}\n\n");
    }
}
