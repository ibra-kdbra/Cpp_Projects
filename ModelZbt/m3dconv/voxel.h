/*
 * m3dconv/voxel.h
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
 * @brief simple voxel image to M3D converter
 * https://gitlab.com/bztsrc/model3d
 *
 */

/* tables */
static char *schem_names[256] = {
  "air","stone","grass","dirt","cobblestone","planks","sapling","bedrock","flowing_water","water","flowing_lava","lava","sand",
  "gravel","gold_ore","iron_ore","coal_ore","log","leaves","sponge","glass","lapis_ore","lapis_block","dispenser","sandstone",
  "noteblock","bed","golden_rail","detector_rail","sticky_piston","web","tallgrass","deadbush","piston","piston_head","wool",
  "piston_extension","yellow_flower","red_flower","brown_mushroom","red_mushroom","gold_block","iron_block","double_stone_slab",
  "stone_slab","brick_block","tnt","bookshelf","mossy_cobblestone","obsidian","torch","fire","mob_spawner","oak_stairs","chest",
  "redstone_wire","diamond_ore","diamond_block","crafting_table","wheat","farmland","furnace","lit_furnace","standing_sign",
  "wooden_door","ladder","rail","stone_stairs","wall_sign","lever","stone_pressure_plate","iron_door","wooden_pressure_plate",
  "redstone_ore","lit_redstone_ore","unlit_redstone_torch","redstone_torch","stone_button","snow_layer","ice","snow","cactus",
  "clay","reeds","jukebox","fence","pumpkin","netherrack","soul_sand","glowstone","portal","lit_pumpkin","cake","unpowered_repeater",
  "powered_repeater","stained_glass","trapdoor","monster_egg","stonebrick","brown_mushroom_block","red_mushroom_block","iron_bars",
  "glass_pane","melon_block","pumpkin_stem","melon_stem","vine","fence_gate","brick_stairs","stone_brick_stairs","mycelium",
  "waterlily","nether_brick","nether_brick_fence","nether_brick_stairs","nether_wart","enchanting_table","brewing_stand","cauldron",
  "end_portal","end_portal_frame","end_stone","dragon_egg","redstone_lamp","lit_redstone_lamp","double_wooden_slab","wooden_slab",
  "cocoa","sandstone_stairs","emerald_ore","ender_chest","tripwire_hook","tripwire","emerald_block","spruce_stairs","birch_stairs",
  "jungle_stairs","command_block","beacon","cobblestone_wall","flower_pot","carrots","potatoes","oak_button","skull","anvil",
  "trapped_chest","light_weighted_pressure_plate","heavy_weighted_pressure_plate","unpowered_comparator","powered_comparator",
  "daylight_detector","redstone_block","quartz_ore","hopper","quartz_block","quartz_stairs","activator_rail","dropper",
  "stained_hardened_clay","stained_glass_pane","leaves2","log2","acacia_stairs","dark_oak_stairs","slime","barrier","iron_trapdoor",
  "prismarine","sea_lantern","hay_block","carpet","hardened_clay","coal_block","packed_ice","double_plant","standing_banner",
  "wall_banner","daylight_detector_inverted","red_sandstone","red_sandstone_stairs","double_stone_slab2","stone_slab2",
  "spruce_fence_gate","birch_fence_gate","jungle_fence_gate","dark_oak_fence_gate","acacia_fence_gate","spruce_fence","birch_fence",
  "jungle_fence","dark_oak_fence","acacia_fence","spruce_door","birch_door","jungle_door","acacia_door","dark_oak_door","end_rod",
  "chorus_plant","chorus_flower","purpur_block","purpur_pillar","purpur_stairs","purpur_double_slab","purpur_slab","end_bricks",
  "beetroots","grass_path","end_gateway","repeating_command_block","chain_command_block","frosted_ice","magma","nether_wart_block",
  "red_nether_brick","bone_block","structure_void","observer","white_shulker_box","orange_shulker_box","magenta_shulker_box",
  "light_blue_shulker_box","yellow_shulker_box","lime_shulker_box","pink_shulker_box","gray_shulker_box","silver_shulker_box",
  "cyan_shulker_box","purple_shulker_box","blue_shulker_box","brown_shulker_box","green_shulker_box","red_shulker_box",
  "black_shulker_box","white_glazed_terracotta","orange_glazed_terracotta","magenta_glazed_terracotta","light_blue_glazed_terracotta",
  "yellow_glazed_terracotta","lime_glazed_terracotta","pink_glazed_terracotta","gray_glazed_terracotta","silver_glazed_terracotta",
  "cyan_glazed_terracotta","purple_glazed_terracotta","blue_glazed_terracotta","brown_glazed_terracotta","green_glazed_terracotta",
  "red_glazed_terracotta","black_glazed_terracotta","concrete","concrete_powder",NULL,NULL,NULL
};
static uint32_t schem_palette[256] = {
    0x00000000,0xff38403c,0xff113634,0xff192b41,0xff383a3c,0xff244459,0xff296d4c,0xff404143,0xffcd7a18,0xffcd7c18,0xff053a8b,0xff054a8b,0xff6f9297,0xff636467,0xff636e71,0xff393e4b,
    0xff38383a,0xff223342,0xff112114,0xff3a989a,0xffa68c8c,0xff68574e,0xff6e3518,0xff5c5c5d,0xff64888b,0xff202e46,0xff242b63,0xff466a87,0xff586778,0xff4c605c,0xffa3a29f,0xff33855b,
    0xff284f6b,0xff4b5a65,0xff9a9a9a,0xff119fc2,0xff183076,0xff5c759b,0xff3f43d6,0xff31a4c1,0xff686768,0xff393c3e,0xff3b3e3f,0xff313351,0xff22228a,0xff213e4e,0xff2e3d3b,0xff100d0c,
    0xff3b6079,0xff2882cf,0xff1f1911,0xff20303e,0xff2c4a5f,0xff000051,0xff575851,0xffb2b94e,0xff223648,0xff4e9daf,0xff364e6c,0xff36393a,0xff36383a,0xff32566e,0xff2b4d63,0xff576e7b,
    0xff383b3c,0xff385e72,0xff5d5f61,0xff757575,0xff9c9a9b,0xff4a7894,0xff314b4b,0xff314a4b,0xff31476a,0xff2647a3,0xff666666,0xffaea5a4,0xffae9075,0xffa79e9e,0xff114e25,0xff7f7f7f,
    0xff2a8e48,0xff223048,0xff3d667f,0xff135d9a,0xff1e1e4c,0xff283140,0xff426787,0xff181006,0xff1b66a0,0xff8c9bba,0xff787a82,0xff71768d,0xffc6c4c4,0xff2d4f63,0xff383e3c,0xff404243,
    0xff405b76,0xff23279e,0xff808080,0xffbab79c,0xff177359,0xff135c9a,0xff177259,0xff0a5120,0xff3f6982,0xff313352,0xff414243,0xff414858,0xff194310,0xff141122,0xff141222,0xff141322,
    0xff111273,0xff464d59,0xff485560,0xff393738,0xff180f06,0xff5b776a,0xff7cafac,0xff0d070a,0xff619695,0xff263949,0xff2c4154,0xff214978,0xff65898c,0xff616b5c,0xff27271a,0x00000000,
    0xff46a122,0xff26445b,0xff919fa4,0xff204155,0xff556785,0xff393a3d,0xff28345d,0xff0c8a1b,0xff22ad19,0xff416b85,0xff898989,0xff393939,0xff1f465f,0xff2e539b,0xff70a2b2,0xff797d86,
    0xff767b96,0xff3c4e59,0xff009a9a,0xff31335c,0xff3d3e3d,0xff31345c,0xff2f315b,0xff576d7b,0xff5c5d5d,0xff1b6892,0xffd3d3d3,0xff0d6327,0xff132331,0xff3b495d,0xff132330,0xff489758,
    0xff0000d5,0xffa2a2a2,0xff75814e,0xff969e88,0xff2c7f94,0xffe0dfdd,0xff364b79,0xff202020,0xffc79071,0xff797470,0xff464b4c,0xff174d91,0xff174e91,0xff185095,0xff19529a,0xff26435b,
    0xff618c9a,0xff405c80,0xff102234,0xff284886,0xff26425a,0xff608a98,0xff3f5a7e,0xff0f2134,0xff274684,0xff97a1a7,0xff816b81,0xff278c65,0xff866386,0xff876687,0xff836284,0xff8a678a,
    0xff8f6a8f,0xff80b0ac,0xff357848,0xff375a74,0xffc78f71,0xff193271,0xff02035a,0xff070637,0xff9babae,0xff473a45,0xff000000,0xff8d8c88,0xff0e4894,0xff67256d,0xff886924,0xff1b7ca1,
    0xff136a3f,0xff675093,0xff2b2926,0xff454a4a,0xff5d530f,0xff6d1647,0xff63201f,0xff192e4a,0xff184738,0xff151561,0xff171413,0xffa3a895,0xff4a757c,0xff9a50a6,0xffa8844c,0xff4799bb,
    0xff2d9e83,0xff927cbb,0xff4b4842,0xff878573,0xff655e2a,0xff7b2657,0xff703326,0xff455560,0xff36725d,0xff2a2f91,0xff1a1836,0xff302d2a,0xff423f3c,0x00000000,0x00000000,0x00000000
};
static uint32_t vox_palette[256] = {
    0x00000000, 0xffffffff, 0xffccffff, 0xff99ffff, 0xff66ffff, 0xff33ffff, 0xff00ffff, 0xffffccff, 0xffccccff, 0xff99ccff, 0xff66ccff, 0xff33ccff, 0xff00ccff, 0xffff99ff, 0xffcc99ff, 0xff9999ff,
    0xff6699ff, 0xff3399ff, 0xff0099ff, 0xffff66ff, 0xffcc66ff, 0xff9966ff, 0xff6666ff, 0xff3366ff, 0xff0066ff, 0xffff33ff, 0xffcc33ff, 0xff9933ff, 0xff6633ff, 0xff3333ff, 0xff0033ff, 0xffff00ff,
    0xffcc00ff, 0xff9900ff, 0xff6600ff, 0xff3300ff, 0xff0000ff, 0xffffffcc, 0xffccffcc, 0xff99ffcc, 0xff66ffcc, 0xff33ffcc, 0xff00ffcc, 0xffffcccc, 0xffcccccc, 0xff99cccc, 0xff66cccc, 0xff33cccc,
    0xff00cccc, 0xffff99cc, 0xffcc99cc, 0xff9999cc, 0xff6699cc, 0xff3399cc, 0xff0099cc, 0xffff66cc, 0xffcc66cc, 0xff9966cc, 0xff6666cc, 0xff3366cc, 0xff0066cc, 0xffff33cc, 0xffcc33cc, 0xff9933cc,
    0xff6633cc, 0xff3333cc, 0xff0033cc, 0xffff00cc, 0xffcc00cc, 0xff9900cc, 0xff6600cc, 0xff3300cc, 0xff0000cc, 0xffffff99, 0xffccff99, 0xff99ff99, 0xff66ff99, 0xff33ff99, 0xff00ff99, 0xffffcc99,
    0xffcccc99, 0xff99cc99, 0xff66cc99, 0xff33cc99, 0xff00cc99, 0xffff9999, 0xffcc9999, 0xff999999, 0xff669999, 0xff339999, 0xff009999, 0xffff6699, 0xffcc6699, 0xff996699, 0xff666699, 0xff336699,
    0xff006699, 0xffff3399, 0xffcc3399, 0xff993399, 0xff663399, 0xff333399, 0xff003399, 0xffff0099, 0xffcc0099, 0xff990099, 0xff660099, 0xff330099, 0xff000099, 0xffffff66, 0xffccff66, 0xff99ff66,
    0xff66ff66, 0xff33ff66, 0xff00ff66, 0xffffcc66, 0xffcccc66, 0xff99cc66, 0xff66cc66, 0xff33cc66, 0xff00cc66, 0xffff9966, 0xffcc9966, 0xff999966, 0xff669966, 0xff339966, 0xff009966, 0xffff6666,
    0xffcc6666, 0xff996666, 0xff666666, 0xff336666, 0xff006666, 0xffff3366, 0xffcc3366, 0xff993366, 0xff663366, 0xff333366, 0xff003366, 0xffff0066, 0xffcc0066, 0xff990066, 0xff660066, 0xff330066,
    0xff000066, 0xffffff33, 0xffccff33, 0xff99ff33, 0xff66ff33, 0xff33ff33, 0xff00ff33, 0xffffcc33, 0xffcccc33, 0xff99cc33, 0xff66cc33, 0xff33cc33, 0xff00cc33, 0xffff9933, 0xffcc9933, 0xff999933,
    0xff669933, 0xff339933, 0xff009933, 0xffff6633, 0xffcc6633, 0xff996633, 0xff666633, 0xff336633, 0xff006633, 0xffff3333, 0xffcc3333, 0xff993333, 0xff663333, 0xff333333, 0xff003333, 0xffff0033,
    0xffcc0033, 0xff990033, 0xff660033, 0xff330033, 0xff000033, 0xffffff00, 0xffccff00, 0xff99ff00, 0xff66ff00, 0xff33ff00, 0xff00ff00, 0xffffcc00, 0xffcccc00, 0xff99cc00, 0xff66cc00, 0xff33cc00,
    0xff00cc00, 0xffff9900, 0xffcc9900, 0xff999900, 0xff669900, 0xff339900, 0xff009900, 0xffff6600, 0xffcc6600, 0xff996600, 0xff666600, 0xff336600, 0xff006600, 0xffff3300, 0xffcc3300, 0xff993300,
    0xff663300, 0xff333300, 0xff003300, 0xffff0000, 0xffcc0000, 0xff990000, 0xff660000, 0xff330000, 0xff0000ee, 0xff0000dd, 0xff0000bb, 0xff0000aa, 0xff000088, 0xff000077, 0xff000055, 0xff000044,
    0xff000022, 0xff000011, 0xff00ee00, 0xff00dd00, 0xff00bb00, 0xff00aa00, 0xff008800, 0xff007700, 0xff005500, 0xff004400, 0xff002200, 0xff001100, 0xffee0000, 0xffdd0000, 0xffbb0000, 0xffaa0000,
    0xff880000, 0xff770000, 0xff550000, 0xff440000, 0xff220000, 0xff110000, 0xffeeeeee, 0xffdddddd, 0xffbbbbbb, 0xffaaaaaa, 0xff888888, 0xff777777, 0xff555555, 0xff444444, 0xff222222, 0xff111111,
};

/**
 * Allocate a voxel m3d structure
 */
m3d_t *alloc_voxm3d()
{
    m3d_t *m3d;

    m3d = (m3d_t*)malloc(sizeof(m3d_t));
    if(!m3d) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
    memset(m3d, 0, sizeof(m3d_t));
    m3d->flags = M3D_FLG_FREESTR;
    m3d->scale = 1.0f;

    if(voxpal && voxpal->voxtype && voxpal->numvoxtype) {
        m3d->numvoxtype = voxpal->numvoxtype;
        m3d->voxtype = voxpal->voxtype;
    }

    return m3d;
}

/**
 * Returns voxel type id for the closest color match
 */
int color2voxtypeid(uint32_t color, m3d_t *m3d)
{
    unsigned char *c = (unsigned char*)&color, *d;
    int i, j, k, ld, diff = 255;

    if(!c[3] || (!c[0] && !c[1] && !c[2])) return -1;

    for(i = 1, j = 0; i < (int)m3d->numvoxtype; i++) {
        /* get local maximum */
        ld = 0; d = (unsigned char*)&m3d->voxtype[i].color;
        k = d[0] > c[0] ? d[0] - c[0] : c[0] - d[0]; if(k > ld) ld = k;
        k = d[1] > c[1] ? d[1] - c[1] : c[1] - d[1]; if(k > ld) ld = k;
        k = d[2] > c[2] ? d[2] - c[2] : c[2] - d[2]; if(k > ld) ld = k;
        k = d[3] > c[3] ? d[3] - c[3] : c[3] - d[3]; if(k > ld) ld = k;
        /* is it smaller than global minimum? */
        if(ld < diff) { diff = ld; j = i; }
    }
    return j;
}

/**
 * Load a schematic and convert into a Model 3D in-memory format
 */
/* parse an integer, the only dynamic type we are interested in. */
#define SCHEM_GETINT(v,t) do{switch(t){case 1:v=d[0];d++;break;case 2:v=(d[0]<<8)|d[1];d+=2;break; \
    case 3:v=(d[0]<<24)|(d[1]<<16)|(d[2]<<8)|d[3];d+=4;break;}}while(0)
m3d_t *schem_load(unsigned char *data, int size)
{
    int i, j, k, l, g = 0, sx = 0, sy = 0, sz = 0, tr[256];
    unsigned char *d, *blk =  NULL, *end = data + size;
    m3d_t *m3d = NULL;

    /* we don't care about that overcomplicated NBT mess */
    for(d = data; d < end; d++) {
        if(!memcmp(d, "\000\006Height", 8)) { d += 8; SCHEM_GETINT(sy, d[-9]); d--; } else
        if(!memcmp(d, "\000\006Length", 8)) { d += 8; SCHEM_GETINT(sz, d[-9]); d--; } else
        if(!memcmp(d, "\000\005Width", 7)) { d += 7; SCHEM_GETINT(sx, d[-8]); d--; } else
        if(!memcmp(d, "\000\009WEOffsetY", 11)) { d += 11; SCHEM_GETINT(g, d[-12]); d--; } else
        if(!memcmp(d, "\007\000\006Blocks", 9)) { d += 9; SCHEM_GETINT(l, 3); blk = d; d += l; }
    }
    if(!blk || !sy || !sz || !sx || l != sy * sz * sx) return NULL;

    m3d = alloc_voxm3d();
    memset(tr, 0, sizeof(tr));

    for(i = 0; i < l; i++) tr[blk[i]] = 1;
    if(!m3d->voxtype) {
        for(i = 1; i < 256; i++)
            m3d->numvoxtype += tr[i];
        m3d->voxtype = (m3dvt_t*)malloc(m3d->numvoxtype * sizeof(m3dvt_t));
        if(!m3d->voxtype) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
        memset(m3d->voxtype, 0, m3d->numvoxtype * sizeof(m3dvt_t));
        for(i = j = 0; i < 256 && j < (int)m3d->numvoxtype; i++)
            if(i && tr[i]) {
                d = (unsigned char*)schem_names[i];
                if(d) {
                    k = strlen((char*)d) + 1;
                    m3d->voxtype[j].name = (char*)malloc(k+10);
                    if(!m3d->voxtype[j].name) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                    memcpy(m3d->voxtype[j].name, "minecraft:", 10);
                    memcpy(m3d->voxtype[j].name + 10, d, k);
                }
                m3d->voxtype[j].color = schem_palette[i];
                m3d->voxtype[j].materialid = M3D_UNDEF;
                m3d->voxtype[j].skinid = M3D_UNDEF;
                tr[i] = j++;
            } else
                tr[i] = -1;
    } else {
        for(i = 0; i < 256; i++)
            tr[i] = i-1;
    }
    m3d->numvoxel = 1;
    m3d->voxel = (m3dvx_t*)malloc(m3d->numvoxel * sizeof(m3dvx_t));
    if(!m3d->voxel) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
    memset(m3d->voxel, 0, m3d->numvoxel * sizeof(m3dvx_t));
    m3d->voxel[0].y = g < 0 ? g : 0;
    m3d->voxel[0].w = sx;
    m3d->voxel[0].h = sy;
    m3d->voxel[0].d = sz;
    m3d->voxel[0].data = (M3D_VOXEL*)malloc(l * sizeof(M3D_VOXEL));
    if(!m3d->voxel[0].data) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
    for(i = 0; i < l; i++)
        m3d->voxel[0].data[i] = tr[blk[i]];

    return m3d;
}

/**
 * Load a Magicavoxel VOX file
 */
m3d_t *vox_load(unsigned char *data, int size)
{
    unsigned char *s, *d, *e = data + size;
    uint32_t *pal = vox_palette;
    int i, j, min_x = 255, min_y = 255, min_z = 255, mi_x, mi_y, mi_z, ma_x, ma_y, ma_z, x, y, z;
    int psiz = 256, siz, tr[256];
    m3d_t *m3d;

    m3d = alloc_voxm3d();
    memset(tr, 0, sizeof(tr));
    for(s = data + 20; s < e; s++) {
        if(!memcmp(s, "RGBA", 4)) {
            pal = (uint32_t*)(s + 8); psiz = *((uint32_t*)(s + 4)) / 4;
            /* use the children size field as the first (empty) color, and we don't have to mess with palette indices */
            pal[0] = 0;
        } else
        if(!memcmp(s, "XYZI", 4)) {
            m3d->numvoxel++;
            siz = (*((uint32_t*)(s + 4)) / 4) - 1;
            s += 16;
            for(i = 0; i < siz; i++, s += 4) {
                if(s[3]) {
                    if(s[0] < min_x) min_x = s[0];
                    if(s[1] < min_z) min_z = s[1];
                    if(s[2] < min_y) min_y = s[2];
                }
                tr[s[3]] = 1;
            }
            s--;
            continue;
        }
        s += *((uint32_t*)(s + 4)) + 11;
    }
    if(!m3d->numvoxel) {
        free(m3d);
        return NULL;
    }
    if(!m3d->voxtype) {
        for(i = 1; i < psiz; i++)
            m3d->numvoxtype += tr[i];
        m3d->voxtype = (m3dvt_t*)malloc(m3d->numvoxtype * sizeof(m3dvt_t));
        if(!m3d->voxtype) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
        memset(m3d->voxtype, 0, m3d->numvoxtype * sizeof(m3dvt_t));
        for(i = j = 0; i < psiz && j < (int)m3d->numvoxtype; i++)
            if(i && tr[i]) {
                m3d->voxtype[j].color = pal[i];
                m3d->voxtype[j].materialid = M3D_UNDEF;
                m3d->voxtype[j].skinid = M3D_UNDEF;
                tr[i] = j++;
            } else
                tr[i] = -1;
    } else {
        for(i = 0; i < psiz; i++)
            tr[i] = voxpal && !forcevoxpal ? color2voxtypeid(pal[i], m3d) : i-1;
    }
    m3d->voxel = (m3dvx_t*)malloc(m3d->numvoxel * sizeof(m3dvx_t));
    if(!m3d->voxel) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
    memset(m3d->voxel, 0, m3d->numvoxel * sizeof(m3dvx_t));

    /* read in the layers */
    for(j = 0, s = data + 20; s < e; s++) {
        if(!memcmp(s, "XYZI", 4)) {
            siz = (*((uint32_t*)(s + 4)) / 4) - 1;
            s += 16; d = s;
            mi_x = mi_y = mi_z = 255; ma_x = ma_y = ma_z = 0;
            for(i = 0; i < siz; i++, s += 4)
                if(s[3]) {
                    if(s[0] < mi_x) mi_x = s[0];
                    if(s[0] > ma_x) ma_x = s[0];
                    if(s[1] < mi_z) mi_z = s[1];
                    if(s[1] > ma_z) ma_z = s[1];
                    if(s[2] < mi_y) mi_y = s[2];
                    if(s[2] > ma_y) ma_y = s[2];
                }
            m3d->voxel[j].x = mi_x - min_x;
            m3d->voxel[j].y = mi_y - min_y;
            m3d->voxel[j].z = mi_z - min_z;
            m3d->voxel[j].w = ma_x - mi_x + 1;
            m3d->voxel[j].h = ma_y - mi_y + 1;
            m3d->voxel[j].d = ma_z - mi_z + 1;
            m3d->voxel[j].data = (M3D_VOXEL*)malloc(m3d->voxel[j].w * m3d->voxel[j].h * m3d->voxel[j].d * sizeof(M3D_VOXEL));
            if(!m3d->voxel[j].data) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
            memset(m3d->voxel[j].data, 0xff, m3d->voxel[j].w * m3d->voxel[j].h * m3d->voxel[j].d * sizeof(M3D_VOXEL));
            for(s = d, i = 0; i < siz; i++, s += 4)
                if(s[3]) {
                    x = s[0] - mi_x;
                    z = s[1] - mi_z;
                    y = s[2] - mi_y;
                    m3d->voxel[j].data[y*m3d->voxel[j].w*m3d->voxel[j].d + (m3d->voxel[j].d-z-1)*m3d->voxel[j].w + x] =
                        (M3D_VOXEL)tr[s[3]];
                }
            j++;
            s--;
            continue;
        }
        s += *((uint32_t*)(s + 4)) + 11;
    }

    return m3d;
}

/**
 * Load a BINVOX file
 */
m3d_t *binvox_load(unsigned char *data, int size)
{
    int i, j, n, sx = 0, sy = 0, sz = 0, x, y, z;
    unsigned char *s, *e = data + size;
    m3d_t *m3d;

    for(s = data; s < e && memcmp(s, "data", 4); s++)
        if(!memcmp(s, "dim ", 4)) {
            s += 4;
            /* accoring to the doc: depth, width and height. Couldn't be worse... */
            while(*s == ' ') { s++; } sz = atoi((char*)s); while(*s >= '0' && *s <= '9') { s++; }
            while(*s == ' ') { s++; } sx = atoi((char*)s); while(*s >= '0' && *s <= '9') { s++; }
            while(*s == ' ') { s++; } sy = atoi((char*)s);
        }
    if(!sx || !sy || !sz || memcmp(s, "data", 4)) return NULL;
    while(*s && *s != '\n') s++;
    while(*s == '\n') s++;

    m3d = alloc_voxm3d();
    m3d->numvoxel = 1;
    m3d->voxel = (m3dvx_t*)malloc(m3d->numvoxel * sizeof(m3dvx_t));
    if(!m3d->voxel) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
    memset(m3d->voxel, 0, m3d->numvoxel * sizeof(m3dvx_t));
    m3d->voxel[0].w = sx;
    m3d->voxel[0].h = sy;
    m3d->voxel[0].d = sz;
    n = sx * sy * sz;
    m3d->voxel[0].data = (M3D_VOXEL*)malloc(n * sizeof(M3D_VOXEL));
    if(!m3d->voxel[0].data) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
    if(!m3d->voxtype) {
        m3d->numvoxtype = 1;
        m3d->voxtype = (m3dvt_t*)malloc(m3d->numvoxtype * sizeof(m3dvt_t));
        if(!m3d->voxtype) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
        memset(m3d->voxtype, 0, m3d->numvoxtype * sizeof(m3dvt_t));
        m3d->voxtype[0].color = 0xFF235580;
        m3d->voxtype[0].materialid = M3D_UNDEF;
        m3d->voxtype[0].skinid = M3D_UNDEF;
    }
    for(i = x = y = z = 0; i < n && s < e && x < sx; s += 2)
        for(j = 0; j < s[1] && x < sx; j++, i++) {
            /* according to the doc: y-coordinate runs fastest, then the z-coordinate, then the x-coordinate */
            m3d->voxel[0].data[y*sx*sz + z*sx + x] = s[0] ? 0 : M3D_VOXUNDEF;
            y++; if(y >= sy) { y = 0; z++; if(z >= sz) { z = 0; x++; } }
        }

    return m3d;
}

/**
 * Load a Qubicle QB file
 */
int _qb_color2voxtypeid(m3d_t *m3d, int cf, unsigned char *s)
{
    uint32_t c;
    int i, j;

    if(!s[3]) return -1;
    c = (uint32_t)(0xFF000000 | (cf ? ((uint32_t)s[0] << 16) | ((uint32_t)s[1] << 8) | (uint32_t)s[2] : *((uint32_t*)s)));
    if(m3d->voxtype) {
        for(i = 0; i < (int)m3d->numvoxtype; i++)
            if(m3d->voxtype[i].color == c) return i;
    }
    if(voxpal)
        return forcevoxpal ? -1 : color2voxtypeid(c, m3d);

    j = m3d->numvoxtype++;
    m3d->voxtype = (m3dvt_t*)realloc(m3d->voxtype, m3d->numvoxtype * sizeof(m3dvt_t));
    if(!m3d->voxtype) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
    memset(&m3d->voxtype[j], 0, sizeof(m3dvt_t));
    m3d->voxtype[j].color = c;
    m3d->voxtype[j].materialid = M3D_UNDEF;
    m3d->voxtype[j].skinid = M3D_UNDEF;
    return j;
}
m3d_t *qb_load(unsigned char *data, int size)
{
    int cf = data[4], za = data[8], co = data[12], mc = *((int*)(data+20)), i, j, k, l, x, y, z, sx, sy, sz;
    unsigned char *s, *e = data + size;
    uint32_t c;
    m3d_t *m3d;

    m3d = alloc_voxm3d();
    m3d->numvoxel = mc;
    m3d->voxel = (m3dvx_t*)malloc(m3d->numvoxel * sizeof(m3dvx_t));
    if(!m3d->voxel) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
    memset(m3d->voxel, 0, m3d->numvoxel * sizeof(m3dvx_t));

    for(s = data + 24, i = 0; i < mc && s < e; i++) {
        j = (*s++) + 1;
        m3d->voxel[i].name = (char*)malloc(j);
        if(!m3d->voxel[i].name) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
        memcpy(m3d->voxel[i].name, s, j);
        s += j;
        m3d->voxel[i].w = sx = *((uint32_t*)s); s += 4;
        m3d->voxel[i].d = sz = *((uint32_t*)s); s += 4;
        m3d->voxel[i].h = sy = *((uint32_t*)s); s += 4;
        m3d->voxel[i].x = *((uint32_t*)s); s += 4;
        m3d->voxel[i].z = *((uint32_t*)s); s += 4;
        m3d->voxel[i].y = *((uint32_t*)s); s += 4;
        m3d->voxel[i].data = (M3D_VOXEL*)malloc(sx * sy * sz * sizeof(M3D_VOXEL));
        if(!m3d->voxel[i].data) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
        memset(m3d->voxel[i].data, 0xff, sx * sy * sz * sizeof(M3D_VOXEL));
        if(co) {
            for(y = 0; y < sy; y++) {
                for(i = 0; 1;) {
                    c = *((uint32_t*)s); s += 4;
                    if(c == 6 || c == 0x60000) break;
                    if(c == 2 || c == 0x20000) { l = *((int32_t*)s); s += 4; c = *((uint32_t*)s); s += 4; } else l = 1;
                    k = _qb_color2voxtypeid(m3d, cf, (unsigned char*)&c);
                    for(j = 0; j < l; j++, i++) {
                        x = i % sx; z = i / sx;
                        m3d->voxel[i].data[y*sx*sz + (za?z:sz-z-1)*sx + x] = k;
                    }
                }
            }
        } else {
            for(y = 0; y < sy; y++)
                for(z = 0; z < sz; z++)
                    for(x = 0; x < sx; x++, s += 4)
                        m3d->voxel[i].data[y*sx*sz + (za?z:sz-z-1)*sx + x] = _qb_color2voxtypeid(m3d, cf, s);
        }
    }
    return m3d;
}

/**
 * Convert surfaces and other parameterized shapes into voxel volume
 */
void voxelize_shapes(int n, m3d_t *m3d)
{
    if(n <= 0 || !m3d->numshape || !m3d->shape || !m3d->numvertex || !m3d->vertex) return;
    /* TODO */
    fprintf(stderr,"m3dconv: shape voxelization is under development.\n");
}

/**
 * Voxelize a triangle mesh (opposite of tri_voxels() in tri.h)
 */
void voxelize_face(int n, m3d_t *m3d)
{
    if(n <= 0 || !m3d->numface || !m3d->face || !m3d->numvertex || !m3d->vertex) return;
    /* TODO */
    fprintf(stderr,"m3dconv: triangle mesh voxelization is under development.\n");
}
