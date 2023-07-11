/*
 * m3d_lua.h
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
 * @brief Lua bindings for procedural 3D models
 * https://gitlab.com/bztsrc/model3d/blob/master/docs/procedural.md
 *
 */

#ifndef _M3D_LUA_H_
#define _M3D_LUA_H_

/**
 * This header provides two main functions. Both receiving a Lua script stored in a Model 3D.
 * - m3d_tx_lua() executes a script and converts its output into a texture to be used in a material
 * - m3d_pr_lua() executes a script but first adds a lua library which is used by the script to generate procedural surface
 *
 * Use like this:
 *  #include <m3d_lua.h>
 *  #include M3D_IMPLEMENTATION
 *  #include <m3d.h>
 */

#ifdef  __cplusplus
extern "C" {
#endif

/* "register" M3D interpreter callbacks */
#define M3D_TX_INTERP m3d_tx_lua
#define M3D_PR_INTERP m3d_pr_lua

#include <m3d.h>
#undef _M3D_H_
#include <string.h>
#define LUA_C89_NUMBERS
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

/**
 * M3D script callback for procedural texture generation
 */
int m3d_tx_lua(const char *name, const void *script, uint32_t len, m3dtx_t *output)
{
    uint32_t i, n, p;
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    if(luaL_loadbuffer(L, script, len, name)) return M3D_ERR_UNKIMG;
    lua_call(L, 0, 1);
    if(lua_istable(L, -1)) {
        /* returns an int array table, ret[0] = width, ret[1] = height, ret[2] = numcomponent, ret[3...] = pixels */
        lua_pushnil(L);
        lua_next(L, -2);
        output->w = (uint16_t)lua_tointeger(L, -1);
        lua_pop(L, 1);
        lua_next(L, -2);
        output->h = (uint16_t)lua_tointeger(L, -1);
        lua_pop(L, 1);
        lua_next(L, -2);
        output->f = (uint8_t)lua_tointeger(L, -1);
        lua_pop(L, 1);
        if(output->w && output->h && output->f >= 1 && output->f <= 4)
            output->d = M3D_MALLOC(output->w * output->h * output->f);
        if(!output->d) {
            lua_close(L);
            return M3D_ERR_ALLOC;
        }
        /* fill up pixel data */
        n = output->w * output->h;
        for(i = 0; i < n && lua_next(L, -2);) {
            p = (uint32_t)lua_tointeger(L, -1);
            switch(output->f) {
                case 1: output->d[i] = p; i++; break;
                case 2: *((uint16_t*)&output->d[i]) = (uint16_t)p; i += 2; break;
                case 3: *((uint32_t*)&output->d[i]) = (uint32_t)p; i += 3; break;
                case 4: *((uint32_t*)&output->d[i]) = (uint32_t)p; i += 4; break;
            }
            lua_pop(L, 1);
        }
    }
    lua_close(L);
    return M3D_SUCCESS;
}

/* C values stored in Lua registry for the Lua callbacks */
const char *_m3d_lua_key = "m3dmodel", *_m3d_lua_mat = "m3dmaterial";
/* Lua callbacks */
int _m3d_lua_getvertex(lua_State *L);
int _m3d_lua_getuv(lua_State *L);
int _m3d_lua_usematerial(lua_State *L);
int _m3d_lua_addtriangle(lua_State *L);
int _m3d_lua_addtriangleuv(lua_State *L);
int _m3d_lua_addtrianglen(lua_State *L);
int _m3d_lua_addtriangleuvn(lua_State *L);

/**
 * M3D script callback for procedural surface generation
 */
int m3d_pr_lua(const char *name, const void *script, uint32_t len, m3d_t *model)
{
    static const struct luaL_Reg m3dlib[] = {
        { "getVertex", _m3d_lua_getvertex },            /* returns the vertex index for the given coordinates */
        { "getUV", _m3d_lua_getuv },                    /* returns the UV index for the given coordinates */
        { "useMaterial", _m3d_lua_usematerial },        /* sets the material for the given material name */
        { "addTriangle", _m3d_lua_addtriangle },        /* adds a simple triangle */
        { "addTriangleUV", _m3d_lua_addtriangleuv },    /* adds triangle with UV coordinates */
        { "addTriangleN", _m3d_lua_addtrianglen },      /* adds triangle with normals */
        { "addTriangleUVN", _m3d_lua_addtriangleuvn },  /* adds triangle with UV coordinates and normals */
        { NULL, NULL }
    };
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    luaL_newmetatable(L, "m3d");
    luaL_setfuncs(L, m3dlib, 0);
    if(luaL_loadbuffer(L, script, len, name)) {
        lua_close(L);
        return M3D_ERR_UNIMPL;
    }
    lua_pushlightuserdata(L, (void*)&_m3d_lua_key);
    lua_pushlightuserdata(L, (void*)model);
    lua_settable(L, LUA_REGISTRYINDEX);
    lua_pushlightuserdata(L, (void*)&_m3d_lua_mat);
    lua_pushinteger(L, -1);
    lua_settable(L, LUA_REGISTRYINDEX);
    lua_call(L, 0, 0);
    lua_close(L);
    return M3D_SUCCESS;
}

int _m3d_lua_getvertex(lua_State *L)
{
    uint32_t i, idx = (uint32_t)-1U;
    m3d_t *model;
    /* called with: vertexIndex = m3d.getVertex(color, x, y, z, w) */
    uint32_t  c = (uint32_t)luaL_checkinteger(L, 1);
    M3D_FLOAT x = (M3D_FLOAT)luaL_checknumber(L, 2);
    M3D_FLOAT y = (M3D_FLOAT)luaL_checknumber(L, 3);
    M3D_FLOAT z = (M3D_FLOAT)luaL_checknumber(L, 4);
    M3D_FLOAT w = (M3D_FLOAT)luaL_checknumber(L, 5);
    if(x >= (M3D_FLOAT)-1.0 && x <= (M3D_FLOAT)1.0 && y >= (M3D_FLOAT)-1.0 && y <= (M3D_FLOAT)1.0 &&
       z >= (M3D_FLOAT)-1.0 && z <= (M3D_FLOAT)1.0 && w >= (M3D_FLOAT)-1.0 && w <= (M3D_FLOAT)1.0) {
        lua_pushlightuserdata(L, (void*)&_m3d_lua_key);
        lua_gettable(L, LUA_REGISTRYINDEX);
        model = (m3d_t*)lua_touserdata(L, -1);
        /* check if vertex exists, get it's index */
        for(i = 0; i < model->numvertex; i++)
            if(model->vertex[i].color == c && model->vertex[i].x == x && model->vertex[i].y == y &&
                model->vertex[i].z == z && model->vertex[i].w == w) { idx = i; break; }
        /* if not, add to model->vertex */
        if(idx == (uint32_t)-1U) {
            idx = model->numvertex++;
            model->vertex = (m3dv_t*)M3D_REALLOC(model->vertex, model->numvertex * sizeof(m3dv_t));
            if(!model->vertex) idx = (uint32_t)-1U;
            else {
                model->vertex[idx].color = c;
                model->vertex[idx].x = x;
                model->vertex[idx].y = y;
                model->vertex[idx].z = z;
                model->vertex[idx].w = w;
            }
        }
    }
    /* return vertex index */
    lua_pushinteger(L, idx);
    return 1;
}
int _m3d_lua_getuv(lua_State *L)
{
    uint32_t i, idx = (uint32_t)-1U;
    m3d_t *model;
    /* called with: uvIndex = m3d.getUV(u, v) */
    M3D_FLOAT u = (M3D_FLOAT)luaL_checknumber(L, 1);
    M3D_FLOAT v = (M3D_FLOAT)luaL_checknumber(L, 2);
    if(u >= (M3D_FLOAT)-1.0 && u <= (M3D_FLOAT)1.0 && v >= (M3D_FLOAT)-1.0 && v <= (M3D_FLOAT)1.0) {
        lua_pushlightuserdata(L, (void*)&_m3d_lua_key);
        lua_gettable(L, LUA_REGISTRYINDEX);
        model = (m3d_t*)lua_touserdata(L, -1);
        /* check if tmap exists, get it's index */
        for(i = 0; i < model->numtmap; i++)
            if(model->tmap[i].u == u && model->tmap[i].v == v) { idx = i; break; }
        /* if not, add to model->tmap */
        if(idx == (uint32_t)-1U) {
            idx = model->numtmap++;
            model->tmap = (m3dti_t*)M3D_REALLOC(model->tmap, model->numtmap * sizeof(m3dti_t));
            if(!model->tmap) idx = (uint32_t)-1U;
            else {
                model->tmap[idx].u = u;
                model->tmap[idx].v = v;
            }
        }
    }
    /* return tmap index */
    lua_pushinteger(L, idx);
    return 1;
}
int _m3d_lua_usematerial(lua_State *L)
{
    uint32_t i;
    m3d_t *model;
    uint32_t materialid = (uint32_t)-1U;
    /* called with: m3d.useMaterial(material name) */
    const char *material = luaL_checkstring(L, 1);
    if(!material || !*material) return 0;
    lua_pushlightuserdata(L, (void*)&_m3d_lua_key);
    lua_gettable(L, LUA_REGISTRYINDEX);
    model = (m3d_t*)lua_touserdata(L, -1);
    for(i = 0; i < model->nummaterial; i++)
        if(!strcmp(material, model->material[i].name)) {
            materialid = i;
            break;
        }
    /* set material index in registry */
    lua_pushlightuserdata(L, (void*)&_m3d_lua_mat);
    lua_pushinteger(L, materialid);
    lua_settable(L, LUA_REGISTRYINDEX);
    lua_pushinteger(L, materialid);
    return 1;
}
int _m3d_lua_addtriangle(lua_State *L)
{
    m3d_t *model;
    uint32_t i, materialid = (uint32_t)-1U;
    /* called with: m3d.addTriangle(v1, v2, v3) */
    uint32_t v1 = (uint32_t)luaL_checkinteger(L, 1);
    uint32_t v2 = (uint32_t)luaL_checkinteger(L, 2);
    uint32_t v3 = (uint32_t)luaL_checkinteger(L, 3);
    lua_pushlightuserdata(L, (void*)&_m3d_lua_key);
    lua_gettable(L, LUA_REGISTRYINDEX);
    model = (m3d_t*)lua_touserdata(L, -1);
    if(v1 < model->numvertex && v2 < model->numvertex && v3 < model->numvertex) {
        lua_pushlightuserdata(L, (void*)&_m3d_lua_mat);
        lua_gettable(L, LUA_REGISTRYINDEX);
        materialid = (uint32_t)lua_tointeger(L, -1);
        /* add to model->face */
        i = model->numface++;
        model->face = (m3df_t*)M3D_REALLOC(model->face, model->numface * sizeof(m3df_t));
        if(model->face) {
            memset(&model->face[i], 255, sizeof(m3df_t)); /* set all index to -1 by default */
            model->face[i].materialid = materialid;
            model->face[i].vertex[0] = v1;
            model->face[i].vertex[1] = v2;
            model->face[i].vertex[2] = v3;
        }
    }
    return 0;
}
int _m3d_lua_addtriangleuv(lua_State *L)
{
    m3d_t *model;
    uint32_t i, materialid = (uint32_t)-1U;
    /* called with: m3d.addTriangleUV(v1, uv1, v2, uv2, v3, uv3) */
    uint32_t v1 = (uint32_t)luaL_checkinteger(L, 1);
    uint32_t uv1= (uint32_t)luaL_checkinteger(L, 2);
    uint32_t v2 = (uint32_t)luaL_checkinteger(L, 3);
    uint32_t uv2= (uint32_t)luaL_checkinteger(L, 4);
    uint32_t v3 = (uint32_t)luaL_checkinteger(L, 5);
    uint32_t uv3= (uint32_t)luaL_checkinteger(L, 6);
    lua_pushlightuserdata(L, (void*)&_m3d_lua_key);
    lua_gettable(L, LUA_REGISTRYINDEX);
    model = (m3d_t*)lua_touserdata(L, -1);
    if(v1 < model->numvertex && v2 < model->numvertex && v3 < model->numvertex &&
        uv1 < model->numtmap && uv2 < model->numtmap && uv3 < model->numtmap) {
        lua_pushlightuserdata(L, (void*)&_m3d_lua_mat);
        lua_gettable(L, LUA_REGISTRYINDEX);
        materialid = (uint32_t)lua_tointeger(L, -1);
        /* add to model->face */
        i = model->numface++;
        model->face = (m3df_t*)M3D_REALLOC(model->face, model->numface * sizeof(m3df_t));
        if(model->face) {
            memset(&model->face[i], 255, sizeof(m3df_t)); /* set all index to -1 by default */
            model->face[i].materialid = materialid;
            model->face[i].vertex[0] = v1;
            model->face[i].vertex[1] = v2;
            model->face[i].vertex[2] = v3;
            model->face[i].texcoord[0] = uv1;
            model->face[i].texcoord[1] = uv2;
            model->face[i].texcoord[2] = uv3;
        }
    }
    return 0;
}
int _m3d_lua_addtrianglen(lua_State *L)
{
    m3d_t *model;
    uint32_t i, materialid = (uint32_t)-1U;
    /* called with: m3d.addTriangleN(v1, n1, v2, n2, v3, n3) */
    uint32_t v1 = (uint32_t)luaL_checkinteger(L, 1);
    uint32_t n1 = (uint32_t)luaL_checkinteger(L, 2);
    uint32_t v2 = (uint32_t)luaL_checkinteger(L, 3);
    uint32_t n2 = (uint32_t)luaL_checkinteger(L, 4);
    uint32_t v3 = (uint32_t)luaL_checkinteger(L, 5);
    uint32_t n3 = (uint32_t)luaL_checkinteger(L, 6);
    lua_pushlightuserdata(L, (void*)&_m3d_lua_key);
    lua_gettable(L, LUA_REGISTRYINDEX);
    model = (m3d_t*)lua_touserdata(L, -1);
    if(v1 < model->numvertex && v2 < model->numvertex && v3 < model->numvertex &&
        n1 < model->numvertex && n2 < model->numvertex && n3 < model->numvertex) {
        lua_pushlightuserdata(L, (void*)&_m3d_lua_mat);
        lua_gettable(L, LUA_REGISTRYINDEX);
        materialid = (uint32_t)lua_tointeger(L, -1);
        /* add to model->face */
        i = model->numface++;
        model->face = (m3df_t*)M3D_REALLOC(model->face, model->numface * sizeof(m3df_t));
        if(model->face) {
            memset(&model->face[i], 255, sizeof(m3df_t)); /* set all index to -1 by default */
            model->face[i].materialid = materialid;
            model->face[i].vertex[0] = v1;
            model->face[i].vertex[1] = v2;
            model->face[i].vertex[2] = v3;
            model->face[i].normal[0] = n1;
            model->face[i].normal[1] = n2;
            model->face[i].normal[2] = n3;
        }
    }
    return 0;
}
int _m3d_lua_addtriangleuvn(lua_State *L)
{
    m3d_t *model;
    uint32_t i, materialid = (uint32_t)-1U;
    /* called with: m3d.addTriangleUVN(v1, uv1, n1, v2, uv2, n2, v3, uv3, n3) */
    uint32_t v1 = (uint32_t)luaL_checkinteger(L, 1);
    uint32_t uv1= (uint32_t)luaL_checkinteger(L, 2);
    uint32_t n1 = (uint32_t)luaL_checkinteger(L, 3);
    uint32_t v2 = (uint32_t)luaL_checkinteger(L, 4);
    uint32_t uv2= (uint32_t)luaL_checkinteger(L, 5);
    uint32_t n2 = (uint32_t)luaL_checkinteger(L, 6);
    uint32_t v3 = (uint32_t)luaL_checkinteger(L, 7);
    uint32_t uv3= (uint32_t)luaL_checkinteger(L, 8);
    uint32_t n3 = (uint32_t)luaL_checkinteger(L, 9);
    lua_pushlightuserdata(L, (void*)&_m3d_lua_key);
    lua_gettable(L, LUA_REGISTRYINDEX);
    model = (m3d_t*)lua_touserdata(L, -1);
    if(v1 < model->numvertex && v2 < model->numvertex && v3 < model->numvertex &&
        n1 < model->numvertex && n2 < model->numvertex && n3 < model->numvertex &&
        uv1 < model->numtmap && uv2 < model->numtmap && uv3 < model->numtmap) {
        lua_pushlightuserdata(L, (void*)&_m3d_lua_mat);
        lua_gettable(L, LUA_REGISTRYINDEX);
        materialid = (uint32_t)lua_tointeger(L, -1);
        /* add to model->face */
        i = model->numface++;
        model->face = (m3df_t*)M3D_REALLOC(model->face, model->numface * sizeof(m3df_t));
        if(model->face) {
            memset(&model->face[i], 255, sizeof(m3df_t)); /* set all index to -1 by default */
            model->face[i].materialid = materialid;
            model->face[i].vertex[0] = v1;
            model->face[i].vertex[1] = v2;
            model->face[i].vertex[2] = v3;
            model->face[i].texcoord[0] = uv1;
            model->face[i].texcoord[1] = uv2;
            model->face[i].texcoord[2] = uv3;
            model->face[i].normal[0] = n1;
            model->face[i].normal[1] = n2;
            model->face[i].normal[2] = n3;
        }
    }
    return 0;
}

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif
