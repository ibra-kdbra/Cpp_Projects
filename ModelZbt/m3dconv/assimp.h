/*
 * m3dconv/assimp.h
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
 * @brief simple 3D model to M3D converter Assimp-related code
 * https://gitlab.com/bztsrc/model3d
 *
 */

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#ifdef EXPINLINE
#include <png.h>
#include <libimagequant.h>
#endif

unsigned char *readfile(char *fn, unsigned int *size);

/*** Assimp specific code. BEWARE, DRAGONS LIVE HERE ***/
const struct aiScene *scene = NULL;

typedef struct {
    char *pKey;
    unsigned int type;
    unsigned int index;
} aiMatProp;

/* --- Scalar Properties ---        !!!!! must match m3d_propertytypes !!!!! */
static aiMatProp aiProps[] = {
    { AI_MATKEY_COLOR_DIFFUSE },                                /* m3dp_Kd */
    { AI_MATKEY_COLOR_AMBIENT },                                /* m3dp_Ka */
    { AI_MATKEY_COLOR_SPECULAR },                               /* m3dp_Ks */
    { AI_MATKEY_SHININESS },                                    /* m3dp_Ns */
    { AI_MATKEY_COLOR_EMISSIVE },                               /* m3dp_Ke */
    { AI_MATKEY_COLOR_REFLECTIVE },                             /* m3dp_Tf */
    { AI_MATKEY_BUMPSCALING },                                  /* m3dp_Km */
    { AI_MATKEY_OPACITY },                                      /* m3dp_d */
    { AI_MATKEY_SHADING_MODEL },                                /* m3dp_il */

    { NULL, 0, 0 },                                             /* m3dp_Pr */
    { AI_MATKEY_REFLECTIVITY },                                 /* m3dp_Pm */
    { NULL, 0, 0 },                                             /* m3dp_Ps */
    { AI_MATKEY_REFRACTI },                                     /* m3dp_Ni */
    { NULL, 0, 0 },
    { NULL, 0, 0 },
    { NULL, 0, 0 },
    { NULL, 0, 0 }
};

/* --- Texture Map Properties ---   !!!!! must match m3d_propertytypes !!!!! */
static aiMatProp aiTxProps[] = {
    { AI_MATKEY_TEXTURE_DIFFUSE(0) },                        /* m3dp_map_Kd */
    { AI_MATKEY_TEXTURE_AMBIENT(0) },                        /* m3dp_map_Ka */
    { AI_MATKEY_TEXTURE_SPECULAR(0) },                       /* m3dp_map_Ks */
    { AI_MATKEY_TEXTURE_SHININESS(0) },                      /* m3dp_map_Ns */
    { AI_MATKEY_TEXTURE_EMISSIVE(0) },                       /* m3dp_map_Ke */
    { NULL, 0, 0 },                                          /* m3dp_map_Tf */
    { AI_MATKEY_TEXTURE_HEIGHT(0) },                         /* m3dp_bump */
    { AI_MATKEY_TEXTURE_OPACITY(0) },                        /* m3dp_map_d */
    { AI_MATKEY_TEXTURE_REFLECTION(0) },                     /* m3dp_refl */

    { AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE_ROUGHNESS,0) },/* m3dp_map_Pr */
    { AI_MATKEY_TEXTURE(aiTextureType_METALNESS,0) },        /* m3dp_map_Pm */
    { NULL, 0, 0 },                                          /* m3dp_map_Ps */
    { AI_MATKEY_TEXTURE(aiTextureType_AMBIENT_OCCLUSION,0) },/* m3dp_map_Ni */
    { NULL, 0, 0 },
    { NULL, 0, 0 },
    { NULL, 0, 0 },
    { NULL, 0, 0 }
};

typedef struct {
    struct aiNode *node;
    struct aiMatrix4x4 mt;
    uint32_t parent;
    M3D_INDEX boneid;
    M3D_INDEX pos;
    M3D_INDEX ori;
    M3D_INDEX lpos;
    M3D_INDEX lori;
    int isbone;
} node_t;
node_t *nodes = NULL;
unsigned int numnodes = 0;
unsigned int numskeleton = 0;

typedef struct {
    unsigned int length;
    unsigned char *data;
} pngbuf_t;

/* helpers for debugging */
void _assimp_nodedump(struct aiNode *node, int l)
{
    unsigned int i;

    if(!node) return;
    for(i=0;(int)i < l; i++) printf("  ");
    printf("node '%s'\n", node->mName.data);
    _assimp_prt(&node->mTransformation);
    for (i = 0; i < node->mNumChildren; i++)
        _assimp_nodedump(node->mChildren[i], l+1);
}

/**
 * Add a material property
 */
void _assimp_addprop(m3dm_t *m, uint8_t type, uint32_t value)
{
    unsigned int i;
    if(type < 128 && !value) return;
    i = m->numprop++;
    m->prop = (m3dp_t*)realloc(m->prop, m->numprop * sizeof(m3dp_t));
    if(!m->prop) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
    m->prop[i].type = type;
    m->prop[i].value.num = value;
}

/**
 * Convert colors
 */
static uint32_t _assimp_color(struct aiColor4D *c)
{
    return  ((uint8_t)(c->a*255) << 24L) |
            ((uint8_t)(c->b*255) << 16L) |
            ((uint8_t)(c->g*255) <<  8L) |
            ((uint8_t)(c->r*255) <<  0L);
}

/**
 * Transform bone relative matrix to world matrix
 */
void _assimp_toworld(struct aiMatrix4x4 *result, struct aiNode *node)
{
    if(node->mParent) {
        _assimp_toworld(result, node->mParent);
        aiMultiplyMatrix4(result, &node->mTransformation);
    } else {
        *result = node->mTransformation;
    }
}

/**
 * Convert node-tree
 */
void _assimp_nodes(struct aiNode *node)
{
    unsigned int i;

    if(!node) return;
    nodes = (node_t*)realloc(nodes, (numnodes+1) * sizeof(node_t));
    if(!nodes) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
    memset(&nodes[numnodes], 0, sizeof(node_t));
    nodes[numnodes].node = node;
    memcpy(&nodes[numnodes].mt, &node->mTransformation, sizeof(struct aiMatrix4x4));
    _assimp_fixmat(&nodes[numnodes].mt);
    nodes[numnodes].parent = nodes[numnodes].boneid = -1U;
    numnodes++;
    for (i = 0; i < node->mNumChildren; i++)
        _assimp_nodes(node->mChildren[i]);
}

/**
 * Fix the node tree...
 */
void _assimp_fixnodes()
{
    struct aiMesh *mesh;
    struct aiNode *node;
    struct aiNode *bone;
/*
    struct aiAnimation *an;
    struct aiNodeAnim *na;
    struct aiVector3D scaling, pos;
    struct aiQuaternion rot;
    int x, y, z, w;
*/
    unsigned int i, j, k, l, m;

    for(m = 0; m < numnodes; m++) {
        node = nodes[m].node;
        for(i = 0; i < node->mNumMeshes; i++) {
            mesh = scene->mMeshes[node->mMeshes[i]];
            /* mark nodes that are listed here as bones */
            for(j = 0; j < mesh->mNumBones; j++)
                for(k = 0; k < numnodes; k++)
                    if(!strcmp(nodes[k].node->mName.data, mesh->mBones[j]->mName.data)) {
                        for(bone = nodes[k].node; bone != NULL; bone = bone->mParent) {
                            for(l = 0; l < numnodes; l++) if(nodes[l].node == bone) break;
                            if(nodes[l].node != scene->mRootNode) {
                                if(!nodes[l].isbone) numskeleton++;
                                nodes[l].isbone = 1;
                            }
                        }
                        break;
                    }
        }
    }
/*
    for(m = 0; m < scene->mNumAnimations; m++) {
        an = scene->mAnimations[m];
        if(an->mTicksPerSecond <= 0.0) an->mTicksPerSecond = 30.0;
        for(j = 0; j < an->mNumChannels; j++) {
            na = an->mChannels[j];
            for(l = 0; l < numnodes; l++)
                if(!strcmp(nodes[l].node->mName.data, na->mNodeName.data)) {
                    if(!nodes[l].isbone) {
                        node = nodes[l].node;
                        aiDecomposeMatrix(&node->mTransformation, &scaling, &rot, &pos);
                        if(na->mNumPositionKeys) {
                            memcpy(&pos, &na->mPositionKeys[0].mValue, sizeof(struct aiVector3D));
                            x = (int)(pos.x * 1000000);
                            y = (int)(pos.y * 1000000);
                            z = (int)(pos.z * 1000000);
                            for(k = 0; k < na->mNumPositionKeys; k++) {
                                if( (int)(na->mPositionKeys[k].mValue.x * 1000000) != x ||
                                    (int)(na->mPositionKeys[k].mValue.y * 1000000) != y ||
                                    (int)(na->mPositionKeys[k].mValue.z * 1000000) != z) {
                                    printf("ERROR: mesh '%s' position transform cannot be converted as a skeletal animation\n",
                                        na->mNodeName.data);
                                    break;
                                }
                            }
                        }
                        if(na->mNumRotationKeys) {
                            memcpy(&rot, &na->mRotationKeys[0].mValue, sizeof(struct aiQuaternion));
                            x = (int)(rot.x * 1000000);
                            y = (int)(rot.y * 1000000);
                            z = (int)(rot.z * 1000000);
                            w = (int)(rot.w * 1000000);
                            for(k = 0; k < na->mNumRotationKeys; k++)
                                if( (int)(na->mRotationKeys[k].mValue.x * 1000000) != x ||
                                    (int)(na->mRotationKeys[k].mValue.y * 1000000) != y ||
                                    (int)(na->mRotationKeys[k].mValue.z * 1000000) != z ||
                                    (int)(na->mRotationKeys[k].mValue.w * 1000000) != w) {
                                    printf("ERROR: mesh '%s' rotation transform cannot be converted as a skeletal animation\n",
                                        na->mNodeName.data);
                                    break;
                                }
                        }
                        if(na->mNumScalingKeys) {
                            memcpy(&scaling, &na->mScalingKeys[0].mValue, sizeof(struct aiVector3D));
                            x = (int)(scaling.x * 1000000);
                            y = (int)(scaling.y * 1000000);
                            z = (int)(scaling.z * 1000000);
                            for(k = 0; k < na->mNumScalingKeys; k++)
                                if( (int)(na->mScalingKeys[k].mValue.x * 1000000) != x ||
                                    (int)(na->mScalingKeys[k].mValue.y * 1000000) != y ||
                                    (int)(na->mScalingKeys[k].mValue.z * 1000000) != z) {
                                    printf("ERROR: mesh '%s' scaling transform cannot be converted as a skeletal animation\n",
                                        na->mNodeName.data);
                                    break;
                                }
                        }
                        printf("fixing %s %g,%g,%g %g,%g,%g,%g\n",node->mName.data,pos.x,pos.y,pos.z,rot.x,rot.y,rot.z,rot.w);
                        _assimp_prt(&node->mTransformation);
                        _assimp_composematrix(&node->mTransformation, &pos, &rot, &scaling);
                    }
                    break;
                }
        }
    }
*/
}

#ifdef EXPINLINE
/* png helpers */
static void _assimp_pngerr(png_structp png_ptr, png_const_charp str)
{
    pngbuf_t *pngbuf = (pngbuf_t*)png_get_io_ptr(png_ptr);
    fprintf(stderr, "m3dconv: png compression failed: %s\n", str);
    if(pngbuf && pngbuf->data) { free(pngbuf->data); pngbuf->data = NULL; }
    pngbuf->length = 0;
}
static void _assimp_pngwr(png_structp png_ptr, png_bytep data, png_size_t length)
{
    unsigned int i;
    pngbuf_t *pngbuf = (pngbuf_t*)png_get_io_ptr(png_ptr);
    i = pngbuf->length; pngbuf->length += length;
    pngbuf->data = (unsigned char*)realloc(pngbuf->data, pngbuf->length);
    if(!pngbuf->data) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
    memcpy(pngbuf->data + i, data, length);
}

/**
 * Get inlined textures
 */
void _assimp_textures(m3d_t *m3d)
{
    unsigned int i, j, k, l, o, ct;
    uint8_t *buff;
    char *bn;
    struct aiTexture *tx;
    png_structp png_ptr;
    png_infop info_ptr;
    png_color pal[256];
    png_byte trns[256];
    pngbuf_t pngbuf;
    liq_attr *handle;
    liq_image *input_image;
    liq_result *quantization_result;
    const liq_palette *liqpalette;

    for(i = 0; i < scene->mNumTextures; i++) {
        tx = scene->mTextures[i];
        if(!tx->mWidth || !tx->mHeight || !tx->pcData) continue;
        bn = strrchr((const char*)&tx->mFilename.data, '.');
        if(bn) *bn = 0;
        bn = strrchr((const char*)&tx->mFilename.data, '/');
        if(!bn) bn = strrchr((const char*)&tx->mFilename.data, '\\');
        if(!bn) bn = tx->mFilename.data; else bn++;
        bn = _m3d_safestr(bn, 0);
        for(l = 0, o = -1U; l < m3d->numtexture; l++)
            if(!strcmp(bn, m3d->texture[l].name)) { o = l; free(bn); break; }
        if(o != -1U) continue;
        o = m3d->numtexture++;
        m3d->texture = (m3dtx_t*)realloc(m3d->texture, m3d->numtexture * sizeof(m3dtx_t));
        if(!m3d->texture) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
        m3d->texture[o].name = bn;
        m3d->texture[o].w = 0;
        m3d->texture[o].h = 0;
        m3d->texture[o].d = NULL;
        if(storeinline) {
            l = 1; ct = PNG_COLOR_TYPE_GRAY;
            if(!strcmp(tx->achFormatHint, "rgba8008")) { l = 2; ct = PNG_COLOR_TYPE_GRAY_ALPHA; } else
            if(!strcmp(tx->achFormatHint, "rgba0808")) { l = 2; ct = PNG_COLOR_TYPE_GRAY_ALPHA; } else
            if(!strcmp(tx->achFormatHint, "rgba0088")) { l = 2; ct = PNG_COLOR_TYPE_GRAY_ALPHA; } else
            if(!strcmp(tx->achFormatHint, "rgba8880")) { l = 3; ct = PNG_COLOR_TYPE_RGB_ALPHA; } else
            if(!strcmp(tx->achFormatHint, "rgba8888")) { l = 4; ct = PNG_COLOR_TYPE_RGB_ALPHA; }
            buff = (uint8_t*)malloc(tx->mWidth * tx->mHeight * l);
            if(!buff) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
            for(j = k = 0; j < tx->mWidth * tx->mHeight; j++)
                switch(l) {
                    case 1:
                        buff[k++] = tx->achFormatHint[4] == '8' ? tx->pcData[j].r :
                        (tx->achFormatHint[5] == '8' ? tx->pcData[j].g :tx->pcData[j].b);
                    break;
                    case 2:
                        buff[k++] = tx->achFormatHint[4] == '8' ? tx->pcData[j].r :
                        (tx->achFormatHint[5] == '8' ? tx->pcData[j].g :tx->pcData[j].b);
                        buff[k++] = tx->pcData[j].a;
                    break;
                    case 3:
                        buff[k++] = tx->pcData[j].r; buff[k++] = tx->pcData[j].g;
                        buff[k++] = tx->pcData[j].b; buff[k++] = 255;
                    break;
                    case 4:
                        buff[k++] = tx->pcData[j].r; buff[k++] = tx->pcData[j].g;
                        buff[k++] = tx->pcData[j].b; buff[k++] = tx->pcData[j].a;
                    break;
                }
            png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, _assimp_pngerr, NULL);
            if(!png_ptr) {
                _assimp_pngerr(NULL, "png_create_write_struct failed");
                free(buff);
                continue;
            }
            info_ptr = png_create_info_struct(png_ptr);
            if(!info_ptr) {
                _assimp_pngerr(NULL, "png_create_info_struct failed");
                png_destroy_write_struct(&png_ptr, NULL);
                free(buff);
                continue;
            }
            if(setjmp(png_jmpbuf(png_ptr))) {
                _assimp_pngerr(NULL, "unknown error happened");
                png_destroy_write_struct(&png_ptr, &info_ptr);
                free(buff);
                if(pngbuf.data) free(pngbuf.data);
                continue;
            }
            memset(&pngbuf, 0, sizeof(pngbuf_t));
            png_set_write_fn(png_ptr, &pngbuf, _assimp_pngwr, NULL);
            png_set_compression_level(png_ptr, 9);
            png_set_compression_mem_level(png_ptr, 5);
            png_set_filter(png_ptr, PNG_FILTER_TYPE_BASE, PNG_FILTER_VALUE_NONE);
            if(l > 2) {
                /* why can't people design a simple and clean API? This liq is braindead too. */
                handle = liq_attr_create();
                input_image = liq_image_create_rgba(handle, buff, tx->mWidth, tx->mHeight, 0);
                if (liq_image_quantize(input_image, handle, &quantization_result) == LIQ_OK) {
                    liq_set_dithering_level(quantization_result, 1.0);
                    liqpalette = liq_get_palette(quantization_result);
                    liq_write_remapped_image(quantization_result, input_image, buff, tx->mWidth * tx->mHeight);
                    ct = PNG_COLOR_TYPE_PALETTE;
                    for(j = k = 0; j < liqpalette->count; j++) {
                        pal[j].red = liqpalette->entries[j].r;
                        pal[j].green = liqpalette->entries[j].g;
                        pal[j].blue = liqpalette->entries[j].b;
                        trns[j] = liqpalette->entries[j].a;
                        if(trns[j] < 255) k++;
                    }
                    png_set_PLTE(png_ptr, info_ptr, pal, liqpalette->count);
                    if(k)
                        png_set_tRNS(png_ptr, info_ptr, trns, k, NULL);
                }
            }
            png_set_IHDR(png_ptr, info_ptr, tx->mWidth, tx->mHeight, 8, ct, PNG_INTERLACE_NONE,
                PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_BASE);
            png_write_info(png_ptr, info_ptr);
            png_set_packing(png_ptr);
            png_write_image(png_ptr, (png_bytepp)&buff);
            png_write_end(png_ptr, info_ptr);
            png_destroy_write_struct(&png_ptr, &info_ptr);
            free(buff);
            if(pngbuf.length && pngbuf.data) {
                inlined = (m3di_t*)realloc(inlined, (numinlined+1) * sizeof(m3di_t));
                if(!inlined) { fprintf(stderr, "m3dconv: memory allocation error\n"); exit(3); }
                inlined[numinlined].name = bn;
                inlined[numinlined].length = pngbuf.length;
                inlined[numinlined].data = pngbuf.data;
                numinlined++;
                if(verbose>1) printf("  Inlining '%s'\n", bn);
            }
        }
    }
}
#endif

/**
 * This does the hard work of parsing most of the assimp structures
 */
void _assimp_mesh(m3d_t *m3d, char *fn)
{
    struct aiMatrix4x4 mt;
    struct aiMatrix3x3 mr;
    struct aiMaterial *mat = NULL;
    struct aiMesh *mesh;
    struct aiFace *face;
    struct aiNode *node;
    struct aiVector3D v;
    struct aiString name;
    struct aiColor4D c;
    char *tn, *path, *bn, *sn;
    unsigned int i, j, k, l, n, m, o, mi, idx, ti;
    float f;
    m3dv_t *vertex;

    for(m = 0; m < numnodes; m++) {
        node = nodes[m].node;
        _assimp_toworld(&mt, node);
        _assimp_fixmat(&mt);
        _assimp_extract3x3(&mr, &mt);
        for(i = 0; i < node->mNumMeshes; i++) {
            mesh = scene->mMeshes[node->mMeshes[i]];
            /* mesh->mMaterialIndex */
            mi = (M3D_INDEX)-1U;
            if(scene->mMaterials) {
                mat = scene->mMaterials[mesh->mMaterialIndex];
                if(mat && aiGetMaterialString(mat, AI_MATKEY_NAME, &name) == AI_SUCCESS && name.length &&
                    strcmp(name.data, AI_DEFAULT_MATERIAL_NAME)) {
                    for(j = 0; j < m3d->nummaterial; j++)
                        if(!strcmp(name.data, m3d->material[j].name)) { mi = j; break; }
                    if(mi == -1U) {
                        mi = m3d->nummaterial++;
                        m3d->material = (m3dm_t*)realloc(m3d->material, m3d->nummaterial * sizeof(m3dm_t));
                        if(!m3d->material) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                        m3d->material[mi].name = _m3d_safestr(name.data, 0);
                        m3d->material[mi].numprop = 0;
                        m3d->material[mi].prop = NULL;
                        for(k = 0; k < sizeof(m3d_propertytypes)/sizeof(m3d_propertytypes[0]); k++) {
                            if(m3d_propertytypes[k].format == m3dpf_map) continue;
                            if(aiProps[k].pKey) {
                                switch(m3d_propertytypes[k].format) {
                                    case m3dpf_color:
                                        if(aiGetMaterialColor(mat, aiProps[k].pKey,aiProps[k].type,aiProps[k].index, &c)
                                            == AI_SUCCESS) {
                                            l = _assimp_color(&c);
                                            if(l != 0)
                                                _assimp_addprop(&m3d->material[mi], m3d_propertytypes[k].id, l);
                                        }
                                    break;
                                    case m3dpf_float:
                                        if(aiGetMaterialFloatArray(mat, aiProps[k].pKey,aiProps[k].type,aiProps[k].index,&f,NULL)
                                            == AI_SUCCESS) {
                                            _assimp_addprop(&m3d->material[mi], m3d_propertytypes[k].id, *((uint32_t*)&f));
                                        }
                                    break;
                                    default:
                                        if(aiGetMaterialIntegerArray(mat, aiProps[k].pKey,aiProps[k].type,aiProps[k].index,
                                            (int*)&l, NULL) == AI_SUCCESS) {
                                            if(m3d_propertytypes[k].id == m3dp_il)
                                                switch(l) {
                                                    case aiShadingMode_NoShading: l = 0; break;
                                                    case aiShadingMode_Phong: l = 2; break;
                                                    default: l = 1; break;
                                                }
                                            _assimp_addprop(&m3d->material[mi], m3d_propertytypes[k].id, l);
                                        }
                                    break;
                                }
                            }
                            if(aiTxProps[k].pKey && aiGetMaterialTexture(mat, aiTxProps[k].type, aiTxProps[k].index,
                                &name, NULL, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
                                    /* some designers are foolish enough to store ABSOLUTE paths in texture names... */
                                    sn = strrchr(name.data, '/');
                                    if(!sn) sn = strrchr(name.data, '\\');
                                    if(!sn) sn = name.data; else sn++;
                                    path = (char*)malloc(strlen(fn) + strlen(sn) + 5);
                                    if(!path) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                                    strcpy(path, fn);
                                    bn = strrchr(path, '/');
                                    if(!bn) bn = strrchr(path, '\\');
                                    if(!bn) bn = path; else bn++;
                                    strcpy(bn, sn);
                                    tn = _m3d_safestr(sn, 0);
                                    bn = strrchr(tn, '.');
                                    if(bn) *bn = 0;
                                    /* check if we already have this texture */
                                    for(l = 0, o = -1U; l < m3d->numtexture; l++)
                                        if(!strcmp(tn, m3d->texture[l].name)) { o = l; free(tn); break; }
                                    if(o == -1U) {
                                        /* only png textures can be inlined. Let's try to find the texture file... */
                                        if(storeinline) {
                                            for(l = 0, o = -1U; l < numinlined; l++)
                                                if(!strcmp(tn, inlined[l].name)) { o = l; break; }
                                            if(o == -1U) {
                                                bn = strrchr(path, '.');
                                                if(bn) strcpy(bn, ".png");
                                                if(!readfile(path, &o) && bn) {
                                                    strcpy(bn, ".PNG");
                                                    if(!readfile(path, &o)) {
                                                        for(o = strlen(path)-1; o > 0 && path[o]!='/' && path[o]!='\\'; o--)
                                                            if(path[o] >= 'A' && path[o] <= 'Z') path[o] += 'a'-'A';
                                                        readfile(path, &o);
                                                    }
                                                }
                                                if(o && verbose>1) printf("  Inlining '%s'\n", tn);
                                            }
                                        }
                                        o = m3d->numtexture++;
                                        m3d->texture = (m3dtx_t*)realloc(m3d->texture, m3d->numtexture * sizeof(m3dtx_t));
                                        if(!m3d->texture) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                                        m3d->texture[o].name = tn;
                                        m3d->texture[o].w = 0;
                                        m3d->texture[o].h = 0;
                                        m3d->texture[o].d = NULL;
                                    }
                                    free(path);
                                    _assimp_addprop(&m3d->material[mi], m3d_propertytypes[k].id + 128, o);
                            }
                        }
                    }
                }
            }
            /* dirty hack, we need a place to store global m3d vertex ids for these local vertices */
            if(mesh->mBitangents) free(mesh->mBitangents);
            mesh->mBitangents = (struct aiVector3D*)malloc(mesh->mNumVertices * sizeof(uint32_t));
            if(!mesh->mBitangents) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
            memset(mesh->mBitangents, 0, mesh->mNumVertices * sizeof(uint32_t));
            /* iterate on face polygons */
            for(l = j = 0; j < mesh->mNumFaces; j++) {
                if(mesh->mFaces[j].mNumIndices != 3) continue;
                l++;
            }
            m3d->face = (m3df_t*)realloc(m3d->face, (m3d->numface + l) * sizeof(m3df_t));
            if(!m3d->face) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
            memset(&m3d->face[m3d->numface], 255, l * sizeof(m3df_t)); /* set all index to -1 by default */
            idx = m3d->numvertex;
            m3d->numvertex += (mesh->mNormals && !withoutnorm ? 6 : 3) * l;
            m3d->vertex = (m3dv_t*)realloc(m3d->vertex, m3d->numvertex * sizeof(m3dv_t));
            if(!m3d->vertex) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
            memset(&m3d->vertex[idx], 0, (m3d->numvertex - idx) * sizeof(m3dv_t));
            ti = m3d->numtmap;
            m3d->numtmap += (mesh->mTextureCoords[0] && !withoutuv ? 3 : 0) * l;
            if(m3d->numtmap > ti) {
                m3d->tmap = (m3dti_t*)realloc(m3d->tmap, m3d->numtmap * sizeof(m3dti_t));
                if(!m3d->tmap) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                memset(&m3d->tmap[ti], 0, (m3d->numtmap - ti) * sizeof(m3dti_t));
            }
            for(j = 0; j < mesh->mNumFaces; j++) {
                face = &mesh->mFaces[j];
                if(face->mNumIndices != 3) continue;
                n = m3d->numface++;
                m3d->face[n].materialid = mi;
                for(k = 0; k < face->mNumIndices; k++) {
                    l = face->mIndices[k];
                    v = mesh->mVertices[l];
                    aiTransformVecByMatrix4(&v, &mt);
                    m3d->face[n].vertex[k] = (M3D_INDEX)idx;
                    ((uint32_t*)(mesh->mBitangents))[l] = idx;
                    vertex = &m3d->vertex[idx];
                    vertex->x = v.x;
                    vertex->y = v.y;
                    vertex->z = v.z;
                    vertex->w = 1.0;
                    vertex->skinid = (M3D_INDEX)-1U;
                    vertex->type = 0;
                    if(mesh->mColors[0] != NULL)
                        vertex->color = _assimp_color(&mesh->mColors[0][l]);
                    idx++;
                    if(mesh->mNormals != NULL && !withoutnorm) {
                        v = mesh->mNormals[l];
                        aiTransformVecByMatrix3(&v, &mr);
                        m3d->face[n].normal[k] = (M3D_INDEX)idx;
                        vertex = &m3d->vertex[idx];
                        vertex->x = v.x;
                        vertex->y = v.y;
                        vertex->z = v.z;
                        vertex->w = 1.0;
                        vertex->skinid = (M3D_INDEX)-1U;
                        vertex->type = 1;
                        idx++;
                    }
                    if(mesh->mTextureCoords[0] != NULL && !withoutuv) {
                        m3d->tmap[ti].u = mesh->mTextureCoords[0][l].x;
                        m3d->tmap[ti].v = mesh->mTextureCoords[0][l].y;
                        m3d->face[n].texcoord[k] = (M3D_INDEX)ti;
                        ti++;
                    }
                }
            }
        }
    }
}

/**
 * Get the skeleton from the node tree
 */
int _assimp_skeleton(m3d_t *m3d)
{
    unsigned int i, j, k, r = 0;
    struct aiMatrix4x4 m, inv;

    for(i = 0; i < numnodes; i++) {
        k = -1U;
        if(nodes[i].node->mParent && nodes[i].node->mParent != scene->mRootNode) {
            if(nodes[i].parent != -1U)
                k = nodes[i].parent;
            else {
                for(j = 0; j < i; j++)
                    if(nodes[j].node == nodes[i].node->mParent) {
                        k = nodes[i].parent = j;
                        break;
                    }
                if(k == -1U) {
                    printf("ERROR: inconsistent bone structure, no such parent '%s' for node '%s'\n",
                        nodes[i].node->mParent->mName.data, nodes[i].node->mName.data);
                    exit(1);
                }
            }
        }
        /* corrigate for the mess we might have in assimp node structures... */
        if(doworld) {
            if(k != -1U)
                _assimp_toworld(&inv, nodes[k].node);
            else
                aiIdentityMatrix4(&inv);
            _assimp_inverse(&inv);
            _assimp_toworld(&m, nodes[i].node);
            aiMultiplyMatrix4(&m, &inv);
        } else
            memcpy(&m, &nodes[i].node->mTransformation, sizeof(struct aiMatrix4x4));
        /* now m is a parent bone relative transformation matrix */
        m3d->vertex = _assimp_addspace(m3d->vertex, &m3d->numvertex, &m, k == -1U ? VT_WORLD : VT_RELATIVE, &k);
        nodes[i].pos = (M3D_INDEX)k;
        nodes[i].ori = (M3D_INDEX)(k + 1);
        /* we don't need the scaling part */
        m3d->numvertex--;
        if(nodes[i].boneid != -1U && (nodes[i].pos != nodes[i].lpos || nodes[i].ori != nodes[i].lori)) r++;
    }
    return r;
}

/**
 * Export bone structure
 */
void _assimp_bones(m3d_t *m3d)
{
    unsigned int i, j;

    if(!numskeleton) return;
    m3d->numbone = numskeleton;
    m3d->bone = (m3db_t*)malloc(numskeleton * sizeof(m3db_t));
    if(!m3d->bone) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
    memset(m3d->bone, 0, numskeleton * sizeof(m3db_t));
    _assimp_skeleton(m3d);

    for(i = j = 0; i < numnodes; i++) {
        if(nodes[i].isbone) {
            m3d->bone[j].name = _m3d_safestr(nodes[i].node->mName.data, 0);
            m3d->bone[j].parent = (nodes[i].parent != -1U ? nodes[nodes[i].parent].boneid : -1U);
            m3d->bone[j].pos = nodes[i].pos;
            m3d->bone[j].ori = nodes[i].ori;
            nodes[i].boneid = j++;
        }
    }
}

/**
 * Export skin into a temporary skintrans array
 */
void _assimp_skin(m3d_t *m3d)
{
    unsigned int i, j, k, l, m, boneid, idx;
    M3D_FLOAT w;
    struct aiNode *node;
    struct aiMesh *mesh;
    struct aiBone *bone;

    /* allocate separate skin group for each vertex. m3d_save will compress it */
    m3d->skin = (m3ds_t*)malloc(m3d->numvertex * sizeof(m3ds_t));
    if(!m3d->skin) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
    m3d->numskin = m3d->numvertex;
    for(i = 0; i < m3d->numvertex; i++)
        for(j = 0; j < M3D_NUMBONE; j++) {
            m3d->skin[i].boneid[j] = (M3D_INDEX)-1U;
            m3d->skin[i].weight[j] = (M3D_FLOAT)0.0;
        }

    /* this is messed up, m3d_t stores boneid+weight pairs (skin records) per vertex, but Assimp
     * has mesh local list of bones and local vertex indices+weight pairs */
    for(m = 0; m < numnodes; m++) {
        node = nodes[m].node;
        for(i = 0; i < node->mNumMeshes; i++) {
            mesh = scene->mMeshes[node->mMeshes[i]];
            if(mesh->mBitangents) {
                for(j = 0; j < mesh->mNumBones; j++) {
                    bone = mesh->mBones[j];
                    if(bone) {
                        boneid = -1U;
                        for(k = 0; k < numnodes; k++)
                            if(!strcmp(nodes[k].node->mName.data, bone->mName.data)) { boneid = nodes[k].boneid; break; }
                        if(boneid != -1U) {
                            for(k = 0; k < bone->mNumWeights; k++) {
                                idx = ((uint32_t*)(mesh->mBitangents))[bone->mWeights[k].mVertexId];
                                m3d->vertex[idx].skinid = (M3D_INDEX)idx;
                                w = (M3D_FLOAT)bone->mWeights[k].mWeight;
                                /* sort skin records by weight descending */
                                for(l = 0; l < M3D_NUMBONE && m3d->skin[idx].weight[l] > w; l++);
                                if(m3d->skin[idx].boneid[l] == boneid && m3d->skin[idx].weight[l] == w) continue;
                                /* insert new boneid+weight pair if there's place */
                                if(l < M3D_NUMBONE) {
                                    if(l + 1 < M3D_NUMBONE && m3d->skin[idx].weight[l] != (M3D_FLOAT)0.0) {
                                        memmove(&m3d->skin[idx].boneid[l+1], &m3d->skin[idx].boneid[l],
                                            (M3D_NUMBONE - l - 1) * sizeof(M3D_INDEX));
                                        memmove(&m3d->skin[idx].weight[l+1], &m3d->skin[idx].weight[l],
                                            (M3D_NUMBONE - l - 1) * sizeof(M3D_FLOAT));
                                    }
                                    m3d->skin[idx].boneid[l] = boneid;
                                    m3d->skin[idx].weight[l] = w;
                                }
                            }
                        } else
                            printf("ERROR: inconsistent skeleton, no bone aiNode for aiBone '%s'\n", bone->mName.data);
                    }
                }
            }
        }
    }
}

/**
 * Import assimp animation data
 */
void _assimp_anim(m3d_t *m3d)
{
    unsigned int i, j, k, l, m, a, n;
    struct aiMesh *mesh;
    struct aiBone *bone;
    struct aiMatrix4x4 ma;
    struct aiAnimation *an;
    struct aiNodeAnim *na;
    struct aiNode *node;
    struct aiVector3D scaling, pos;
    struct aiQuaternion rot;
    double *t;
    m3da_t *anim;

    if(!m3d->numbone) return;
    /* Assimp has:
     * anim
     *  \-channels (bones)
     *     |-positionkeys (each with it's own timestamp)
     *     |-rotationkeys (each with it's own timestamp)
     *     \-scalingkeys  (each with it's own timestamp)
     *
     * We need:
     * anim
     *  \-frame (timestamp)
     *     \-bone, position, rotation (only that changed compared to the previous frame, scaling in rotation)
     */

    if(arg_framedelay <= 0.0) arg_framedelay = 1.0;
    for(a = 0; a < scene->mNumAnimations; a++) {
        an = scene->mAnimations[a];
        if(an->mTicksPerSecond <= 0.0) an->mTicksPerSecond = 1.0;
        /* add a new action */
        j = m3d->numaction++;
        m3d->action = (m3da_t*)realloc(m3d->action, m3d->numaction * sizeof(m3da_t));
        if(!m3d->action) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
        anim = &m3d->action[j];
        anim->name = _m3d_safestr(an->mName.data[0] ? an->mName.data : "Global", 0);
        /* don't trust Assimp, mDuration is often wrong! */
        /* anim->durationmsec = an->mDuration / an->mTicksPerSecond * 1000.0 * arg_framedelay; */
        anim->durationmsec = 0;
        anim->numframe = 0;
        anim->frame = NULL;
        /* at this point I'm absolutely certain that assimp devs were on drugs. Pretty strong ones may I add.
         * let's get how many frames are there actually, and what timestamps they have */
        for(j = n = 0, t = NULL; j < an->mNumChannels; j++) {
            na = an->mChannels[j];
            for(k = 0; k < na->mNumPositionKeys; k++) {
                for(l = 0; l < n && t[l] < na->mPositionKeys[k].mTime; l++);
                if(l == n || t[l] > na->mPositionKeys[k].mTime) {
                    n++; t = (double*)realloc(t, n * sizeof(double));
                    if(l + 1 != n) memmove(&t[l+1], &t[l], (n - l - 1) * sizeof(double));
                }
                t[l] = na->mPositionKeys[k].mTime;
            }
            for(k = 0; k < na->mNumRotationKeys; k++) {
                for(l = 0; l < n && t[l] < na->mRotationKeys[k].mTime; l++);
                if(l == n || t[l] > na->mRotationKeys[k].mTime) {
                    n++; t = (double*)realloc(t, n * sizeof(double));
                    if(l + 1 != n) memmove(&t[l+1], &t[l], (n - l - 1) * sizeof(double));
                }
                t[l] = na->mRotationKeys[k].mTime;
            }
            for(k = 0; k < na->mNumScalingKeys; k++) {
                for(l = 0; l < n && t[l] < na->mScalingKeys[k].mTime; l++);
                if(l == n || t[l] > na->mScalingKeys[k].mTime) {
                    n++; t = (double*)realloc(t, n * sizeof(double));
                    if(l + 1 != n) memmove(&t[l+1], &t[l], (n - l - 1) * sizeof(double));
                }
                t[l] = na->mScalingKeys[k].mTime;
            }
        }
        /* ok, now t[n] contains the timestamps in a sorted list */
        /* reset skeleton to bind-pose on animation start */
        for(j = 0; j < numnodes; j++) {
            if(nodes[j].boneid != -1U) {
                nodes[j].pos = nodes[j].lpos = m3d->bone[nodes[j].boneid].pos;
                nodes[j].ori = nodes[j].lori = m3d->bone[nodes[j].boneid].ori;
            }
        }
        /* iterate on frames */
        for(i = 0; i < n; i++) {
            /* reset skeleton transformation matrices on every frame start */
            for(j = 0; j < numnodes; j++)
                memcpy(&nodes[j].node->mTransformation, &nodes[j].mt, sizeof(struct aiMatrix4x4));
            /* one does not simply get an animation-pose from assimp... */
            for(j = l = 0; j < an->mNumChannels; j++) {
                na = an->mChannels[j];
                node = NULL;
                for(k = 0; k < numnodes; k++)
                    if(!strcmp(nodes[k].node->mName.data, na->mNodeName.data)) {
                        if(nodes[k].boneid != -1U) node = nodes[k].node;
                        else {
                            l = 1;
                            fprintf(stderr, "m3dconv: mesh transform for '%s' in '%s' frame %d\n",
                                na->mNodeName.data, an->mName.data, n + 1);
                        }
                        break;
                    }
                if(!node) continue;
                aiDecomposeMatrix(&node->mTransformation, &scaling, &rot, &pos);
                for(k = 0; k < na->mNumPositionKeys && na->mPositionKeys[k].mTime <= t[i]; k++)
                    if(na->mPositionKeys[k].mTime == t[i]) {
                        memcpy(&pos, &na->mPositionKeys[k].mValue, sizeof(struct aiVector3D));
                        break;
                    }
                for(k = 0; k < na->mNumRotationKeys && na->mRotationKeys[k].mTime <= t[i]; k++)
                    if(na->mRotationKeys[k].mTime == t[i]) {
                        memcpy(&rot, &na->mRotationKeys[k].mValue, sizeof(struct aiQuaternion));
                        break;
                    }
                for(k = 0; k < na->mNumScalingKeys && na->mScalingKeys[k].mTime <= t[i]; k++)
                    if(na->mScalingKeys[k].mTime == t[i]) {
                        memcpy(&scaling, &na->mScalingKeys[k].mValue, sizeof(struct aiVector3D));
                        break;
                    }
                _assimp_composematrix(&node->mTransformation, &pos, &rot, &scaling);
            }
            /* dirty hack. we try to compensate for mesh transforms in skeletal animation... by multiplying the
             * affected bones with the mesh transform. We have to do this in a second round, since matrix
             * multiplication is not commutative. */
            if(l) {
                for(j = 0; j < an->mNumChannels; j++) {
                    na = an->mChannels[j];
                    node = NULL;
                    for(k = 0; k < numnodes; k++)
                        if(!strcmp(nodes[k].node->mName.data, na->mNodeName.data) && nodes[k].boneid == -1U) {
                            node = nodes[k].node;
                            break;
                        }
                    if(node) {
                        memcpy(&ma, &node->mTransformation, sizeof(struct aiMatrix4x4));
                        aiTransposeMatrix4(&ma);
                        for(k = 0; k < node->mNumMeshes; k++) {
                            mesh = scene->mMeshes[node->mMeshes[k]];
                            if(mesh)
                                for(l = 0; l < mesh->mNumBones; l++) {
                                    bone = mesh->mBones[l];
                                    if(bone) {
                                        for(m = 0; m < numnodes; m++)
                                            if(!strcmp(nodes[m].node->mName.data, bone->mName.data)) {
                                                aiMultiplyMatrix4(&nodes[m].node->mTransformation, &ma);
                                                break;
                                            }
                                    }
                                }
                        }
                    }
                }
            }
            /* now get the animation-pose skeleton */
            j = _assimp_skeleton(m3d);
            /* if we have changed bones */
            if(j) {
                /* add a new frame */
                m = anim->numframe++;
                anim->frame = (m3dfr_t*)realloc(anim->frame, anim->numframe * sizeof(m3dfr_t));
                if(!anim->frame) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                anim->frame[m].transform = (m3dtr_t*)malloc(j * sizeof(m3dtr_t));
                if(!anim->frame[m].transform) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                anim->frame[m].msec = t[i] / an->mTicksPerSecond * 1000.0 * arg_framedelay;
                if(anim->frame[m].msec > anim->durationmsec)
                    anim->durationmsec = anim->frame[m].msec;
                anim->frame[m].numtransform = j;
                /* add changed bones transforms to this frame */
                for(k = j = 0; k < numnodes; k++)
                    if(nodes[k].boneid != -1U && (nodes[k].pos != nodes[k].lpos || nodes[k].ori != nodes[k].lori)) {
                        anim->frame[m].transform[j].boneid = nodes[k].boneid;
                        anim->frame[m].transform[j].pos = nodes[k].lpos = nodes[k].pos;
                        anim->frame[m].transform[j].ori = nodes[k].lori = nodes[k].ori;
                        j++;
                    }
            }
        }
        free(t);
    }
}

/**
 * Fix materials if there's a texture next to the model by the same name
 */
void _assimp_fixmaterials(m3d_t *m3d, char *fn)
{
    unsigned char *data;
    char *tfn, *ext;
    unsigned int i, j, o, mi, s;

    /* first, look for modelname.png */
    tfn = malloc(strlen(fn) + 512);
    if(!tfn) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); return; }
    strcpy(tfn, fn);
    ext = strrchr(tfn, '.');
    if(ext) *ext = 0;
    strcat(tfn, ".png");
    ext = tfn + strlen(tfn) - 4;
    /* check if file exists, and read it into inlined array if inline requested */
    data = readfile(tfn, &s);
    if(!data) { memcpy(ext, ".PNG", 4); data = readfile(tfn, &s); }
    if(!data) { free(tfn); return; }
    /* file exists */
    if(!storeinline) free(data);
    *ext = 0;
    ext = strrchr(tfn, '/');
    if(!ext) ext = strrchr(tfn, '\\');
    if(!ext) ext = tfn; else ext++;
    /* is this texture already defined in the model? */
    for(i = 0, o = -1U; i < m3d->numtexture; i++)
        if(!strcmp(ext, m3d->texture[i].name)) { o = i; break; }
    if(o == -1U) {
        /* add texture */
        o = m3d->numtexture++;
        m3d->texture = (m3dtx_t*)realloc(m3d->texture, m3d->numtexture * sizeof(m3dtx_t));
        if(!m3d->texture) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
        m3d->texture[o].name = (char*)malloc(strlen(ext)+1);
        if(!m3d->texture[o].name) { free(tfn); return; }
        strcpy(m3d->texture[o].name, ext);
        m3d->texture[o].w = 0;
        m3d->texture[o].h = 0;
        m3d->texture[o].d = NULL;
        /* add a material with this texture as diffuse color map */
        mi = m3d->nummaterial++;
        m3d->material = (m3dm_t*)realloc(m3d->material, m3d->nummaterial * sizeof(m3dm_t));
        if(!m3d->material) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
        m3d->material[mi].name = _m3d_safestr("Default", 0);
        m3d->material[mi].numprop = 0;
        m3d->material[mi].prop = NULL;
        _assimp_addprop(&m3d->material[mi], m3dp_map_Kd, o);
        /* replace materialid to this new material in face for triangles which have UV coordinates */
        for(i = 0; i < m3d->numface; i++) {
            if(m3d->face[i].materialid == -1U && m3d->face[i].texcoord[0] != -1U)
                m3d->face[i].materialid = mi;
        }
    }
    /* now let's look for materialname.png */
    for(i = 0; i < m3d->nummaterial; i++) {
        /* does this material have a Kd map? */
        for(j = 0, o = -1U; j < m3d->material[i].numprop; j++)
            if(m3d->material[i].prop[j].type == m3dp_map_Kd) { o = j; break; }
        if(o == -1U) {
            /* no, and is there a materialname.png? */
            strcpy(ext, m3d->material[i].name);
            strcat(ext, ".png");
            data = readfile(tfn, &s);
            if(data) {
                /* file exists */
                if(!storeinline) free(data);
                for(j = 0, o = -1U; j < m3d->numtexture; j++)
                    if(!strcmp(m3d->material[i].name, m3d->texture[j].name)) { o = j; break; }
                if(o == -1U) {
                    /* add texture */
                    o = m3d->numtexture++;
                    m3d->texture = (m3dtx_t*)realloc(m3d->texture, m3d->numtexture * sizeof(m3dtx_t));
                    if(!m3d->texture) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                    m3d->texture[o].name = (char*)malloc(strlen(m3d->material[i].name)+1);
                    if(!m3d->texture[o].name) { free(tfn); return; }
                    strcpy(m3d->texture[o].name, m3d->material[i].name);
                    m3d->texture[o].w = 0;
                    m3d->texture[o].h = 0;
                    m3d->texture[o].d = NULL;
                }
                _assimp_addprop(&m3d->material[i], m3dp_map_Kd, o);
            }
        }
    }
    free(tfn);
}

/**
 * Load a model and convert it's structures into a Model 3D in-memory format
 */
m3d_t *assimp_load(char *fn)
{
    m3d_t *m3d;
    unsigned int i;
    char *name = strrchr(fn, '/');
    if(!name) name = strrchr(fn, '\\');

    m3d = (m3d_t*)malloc(sizeof(m3d_t));
    if(!m3d) return NULL;
    memset(m3d, 0, sizeof(m3d_t));
    m3d->flags = M3D_FLG_FREESTR;

    if(verbose>1) printf("  aiImportFile\n");
    scene = aiImportFile(fn, aiProcess_Triangulate | aiProcess_FindInstances | aiProcess_RemoveRedundantMaterials |
#ifdef EXPINLINE
        aiProcess_EmbedTextures |
#endif
        (withoutnorm ? 0 : aiProcess_GenSmoothNormals) | (withoutuv ? 0 : aiProcess_GenUVCoords |
        aiProcess_TransformUVCoords));
    if(!scene) {
        free(m3d);
        errstr = (char*)aiGetErrorString();
        return NULL;
    }
    m3d->name = _m3d_safestr( arg_name && *arg_name ? arg_name : ( strcmp(scene->mRootNode->mName.data, "ROOT") &&
        strcmp(scene->mRootNode->mName.data, "Root") && strcmp(scene->mRootNode->mName.data, "root") ?
        scene->mRootNode->mName.data : (name ? name + 1 : fn)), 2);

    /* convert assimp node structure into something we can actually work with */
    if(verbose>1) printf("  Converting nodes\n");
    _assimp_nodes(scene->mRootNode);
    _assimp_fixnodes();
#ifdef EXPINLINE
    /* convert assimp inlined textures */
    if(verbose>1) printf("  Converting inlined textures\n");
    _assimp_textures(m3d);
#endif
    /* convert assimp meshes into m3d_t */
    if(verbose>1) printf("  Converting meshes\n");
    _assimp_mesh(m3d, fn);
    if(!domesh && numskeleton) {
        if(verbose>1) printf("  Converting bones\n");
        _assimp_bones(m3d);
        if(m3d->numvertex && m3d->numbone) {
            if(verbose>1) printf("  Converting skins\n");
            _assimp_skin(m3d);
            if(verbose>1) printf("  Converting animations\n");
            _assimp_anim(m3d);
        }
    }
    if(verbose>1) printf("  Fix materials\n");
    _assimp_fixmaterials(m3d, fn);
    for(i = 0; i < scene->mNumMeshes; i++)
        if(scene->mMeshes[i]->mBitangents) { free(scene->mMeshes[i]->mBitangents); scene->mMeshes[i]->mBitangents = NULL; }
    free(nodes);

    if(verbose>1) printf("  aiReleaseImport\n");
    aiReleaseImport(scene);
    scene = NULL;
    return m3d;
}
