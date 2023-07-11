/*
 * m3dconv/fbx.h
 *
 * Copyright (C) 2022 bzt (bztsrc@gitlab)
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
 * @brief simple 3D model to M3D converter Autodesk FBX importer
 * https://gitlab.com/bztsrc/model3d
 * https://github.com/bqqbarbhg/ufbx
 *
 * Special thanks to bqqbarbhg for the help he provided in this!
 *
 */

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
#include "ufbx.h"
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#define MAX_BONES 64
#define MAX_BLEND_SHAPES 64

typedef struct skin_vertex {
    uint8_t bone_index[4];
    uint8_t bone_weight[4];
} skin_vertex;

/**
 * This comes mostly from ufbx/examples/viewer.c
 */
static void read_mesh(m3d_t *m3d, ufbx_mesh *mesh, ufbx_node *node)
{
    size_t max_triangles = 0;
    size_t pi, ci, fi, vi;
    size_t num_tri_indices = mesh->max_face_triangles * 3;
    size_t num_parts = 0;
    ufbx_matrix normal_to_world;
    ufbx_mesh_material *mesh_mat;
    size_t num_blend_shapes = 0;
    ufbx_blend_channel *blend_channels[MAX_BLEND_SHAPES];
    size_t num_bones = 0;
    ufbx_skin_deformer *skin = NULL;
    ufbx_skin_cluster *cluster;
    uint32_t *tri_indices;
    skin_vertex *mesh_skin_vertices;
    ufbx_face face;
    ufbx_vec3 pos, normal;
    ufbx_vec2 uv;
    ufbx_vec4 color;
    ufbx_real length;
    size_t num_tris;
    M3D_INDEX mi;
    int i;

    if(!m3d || !mesh || !node) return;

    normal_to_world = ufbx_matrix_for_normals(&node->geometry_to_world);

    for (pi = 0; pi < mesh->materials.count; pi++) {
        mesh_mat = &mesh->materials.data[pi];
        if (mesh_mat->num_triangles == 0) continue;
        if(mesh_mat->num_triangles > max_triangles) max_triangles = mesh_mat->num_triangles;
    }

    tri_indices = calloc(num_tri_indices, sizeof(uint32_t));
    mesh_skin_vertices = calloc(mesh->num_vertices, sizeof(skin_vertex));
    if(!tri_indices || !mesh_skin_vertices) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }

/*
    TODO: bones and skin

    if (mesh->skin_deformers.count > 0) {
        skin = mesh->skin_deformers.data[0];
        for (ci = 0; ci < skin->clusters.count; ci++) {
            cluster = skin->clusters.data[ci];
            if (num_bones < MAX_BONES) {
                vmesh->bone_indices[num_bones] = (int32_t)cluster->bone_node->typed_id;
                vmesh->bone_matrices[num_bones] = ufbx_to_um_mat(cluster->geometry_to_bone);
                num_bones++;
            }
        }
        vmesh->num_bones = num_bones;

        for (vi = 0; vi < mesh->num_vertices; vi++) {
            size_t num_weights = 0;
            float total_weight = 0.0f;
            float weights[4] = { 0.0f };
            uint8_t clusters[4] = { 0 };

            ufbx_skin_vertex vertex_weights = skin->vertices.data[vi];
            for (size_t wi = 0; wi < vertex_weights.num_weights; wi++) {
                if (num_weights >= 4) break;
                ufbx_skin_weight weight = skin->weights.data[vertex_weights.weight_begin + wi];

                if (weight.cluster_index < MAX_BONES) {
                    total_weight += (float)weight.weight;
                    clusters[num_weights] = (uint8_t)weight.cluster_index;
                    weights[num_weights] = (float)weight.weight;
                    num_weights++;
                }
            }

            if (total_weight > 0.0f) {
                skin_vertex *skin_vert = &mesh_skin_vertices[vi];
                uint32_t quantized_sum = 0;
                for (size_t i = 0; i < 4; i++) {
                    uint8_t quantized_weight = (uint8_t)((float)weights[i] / total_weight * 255.0f);
                    quantized_sum += quantized_weight;
                    skin_vert->bone_index[i] = clusters[i];
                    skin_vert->bone_weight[i] = quantized_weight;
                }
                skin_vert->bone_weight[0] += 255 - quantized_sum;
            }
        }
    }
*/

    for (pi = 0; pi < mesh->materials.count; pi++) {
        mesh_mat = &mesh->materials.data[pi];
        if (mesh_mat->num_triangles == 0) continue;

        if(mesh_mat->material == NULL) {
            mi = -1U;
        } else {
            for(mi = 0; mi < m3d->nummaterial && strcmp(m3d->material[mi].name, mesh_mat->material->name.data); mi++);
            if(mi >= m3d->nummaterial) {
                mi = m3d->nummaterial++;
                m3d->material = (m3dm_t*)realloc(m3d->material, m3d->nummaterial * sizeof(m3dm_t));
                if(!m3d->material) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                memset(&m3d->material[mi], 0, sizeof(m3dm_t));
                m3d->material[mi].name = (char*)malloc(mesh_mat->material->name.length + 1);
                if(!m3d->material[mi].name) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
                strcpy(m3d->material[mi].name, mesh_mat->material->name.data);
                /* TODO: add material props */
            }
        }

        for (fi = 0; fi < mesh_mat->num_faces; fi++) {
            face = mesh->faces.data[mesh_mat->face_indices.data[fi]];
            num_tris = ufbx_triangulate_face(tri_indices, num_tri_indices, mesh, face);

            m3d->face = (m3df_t*)realloc(m3d->face, (m3d->numface + num_tris) * sizeof(m3df_t));
            if(!m3d->face) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
            memset(&m3d->face[m3d->numface], 255, num_tris * sizeof(m3df_t));

            m3d->vertex = (m3dv_t*)realloc(m3d->vertex, (m3d->numvertex + num_tris * 6) * sizeof(m3dv_t));
            if(!m3d->vertex) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
            memset(&m3d->vertex[m3d->numvertex], 0, num_tris * 6 * sizeof(m3dv_t));
            m3d->tmap = (m3dti_t*)realloc(m3d->tmap, (m3d->numtmap + num_tris * 3) * sizeof(m3dti_t));
            if(!m3d->tmap) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
            memset(&m3d->tmap[m3d->numtmap], 0, num_tris * 3 * sizeof(m3dti_t));

            for (vi = 0; vi < num_tris; vi++) {
                m3d->face[m3d->numface].materialid = mi;
                for(i = 0; i < 3; i++) {
                    pos = ufbx_get_vertex_vec3(&mesh->vertex_position, tri_indices[3 * vi + i]);
                    pos = ufbx_transform_position(&node->geometry_to_world, pos);
                    m3d->vertex[m3d->numvertex].x = pos.x;
                    m3d->vertex[m3d->numvertex].y = pos.y;
                    m3d->vertex[m3d->numvertex].z = pos.z;
                    m3d->vertex[m3d->numvertex].w = 1.0;
                    m3d->vertex[m3d->numvertex].type = VT_WORLD;
/*
                    TODO: get skinid
                    // The skinning vertex stream is pre-calculated above so we just need to
                    // copy the right one by the vertex index.
                    if (skin) {
                        skin_vertices[num_indices] = mesh_skin_vertices[mesh->vertex_indices.data[tri_indices[3 * vi + i]]];
                    }
*/
                    m3d->vertex[m3d->numvertex].skinid = -1U;
                    if(mesh->vertex_color.exists) {
                        color = ufbx_get_vertex_vec4(&mesh->vertex_color, tri_indices[3 * vi + i]);
                        m3d->vertex[m3d->numvertex].color =
                            ((uint32_t)(color.w * 255.0) << 24L) |
                            ((uint32_t)(color.z * 255.0) << 16L) |
                            ((uint32_t)(color.y * 255.0) << 8L) |
                            ((uint32_t)(color.x * 255.0));
                    }
                    m3d->face[m3d->numface].vertex[i] = m3d->numvertex++;
                    if(mesh->vertex_normal.exists) {
                        normal = ufbx_get_vertex_vec3(&mesh->vertex_normal, tri_indices[3 * vi + i]);
                        normal = ufbx_transform_direction(&normal_to_world, normal);
                        length = sqrt(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
                        m3d->vertex[m3d->numvertex].x = normal.x / length;
                        m3d->vertex[m3d->numvertex].y = normal.y / length;
                        m3d->vertex[m3d->numvertex].z = normal.z / length;
                        m3d->vertex[m3d->numvertex].w = 1.0;
                        m3d->vertex[m3d->numvertex].type = VT_NORMAL;
                        m3d->vertex[m3d->numvertex].skinid = -1U;
                        m3d->face[m3d->numface].normal[i] = m3d->numvertex++;
                    }
                    if(mesh->vertex_uv.exists) {
                        uv = ufbx_get_vertex_vec2(&mesh->vertex_uv, tri_indices[3 * vi + i]);
                        m3d->tmap[m3d->numtmap].u = uv.x;
                        m3d->tmap[m3d->numtmap].v = uv.y;
                        m3d->face[m3d->numface].texcoord[i] = m3d->numtmap++;
                    }
                }
                m3d->numface++;
            }
        }
    }

    free(tri_indices);
    free(mesh_skin_vertices);
}

/**
 * Load a model and convert it's structures into a Model 3D in-memory format
 */
m3d_t *fbx_load(unsigned char *data, unsigned int size)
{
    m3d_t *m3d;
    ufbx_load_opts opts = { 0 };
    ufbx_error error = { 0 };
    ufbx_scene *scene = NULL;
    size_t i, j;

    opts.load_external_files = true;
    opts.allow_null_material = true;
    opts.evaluate_skinning = true;
    opts.target_axes.right = UFBX_COORDINATE_AXIS_POSITIVE_X;
    opts.target_axes.up = UFBX_COORDINATE_AXIS_POSITIVE_Y;
    opts.target_axes.front = UFBX_COORDINATE_AXIS_POSITIVE_Z;
    /* load the model using ufbx */
    scene = ufbx_load_memory((const void*)data, size, &opts, &error);
    if(!scene) {
        fprintf(stderr, "m3dconv: ufbx failed to parse model file (err %x, '%s')\n", error.type, error.description.data); exit(1);
    }

    m3d = (m3d_t*)malloc(sizeof(m3d_t));
    if(!m3d) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
    memset(m3d, 0, sizeof(m3d_t));
    m3d->flags = M3D_FLG_FREESTR;

    /*
    for(i = 0; i < scene->metadata.scene_props.props.count; i++)
        printf("%s='%s'\n",scene->metadata.scene_props.props.data[i].name.data,scene->metadata.scene_props.props.data[i].value_str.data);
    */
    if(scene->metadata.creator.data) {
        m3d->author = (char*)malloc(scene->metadata.creator.length + 1);
        if(!m3d->author) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
        strcpy(m3d->author, scene->metadata.creator.data);
    }

    /* add default position and orientation, may be needed by bones in group statements */
    m3d->numvertex = 2;
    m3d->vertex = (m3dv_t*)malloc(m3d->numvertex * sizeof(m3dv_t));
    if(!m3d->vertex) { fprintf(stderr, "m3dconv: unable to allocate memory\n"); exit(1); }
    memset(m3d->vertex, 0, 2 * sizeof(m3dv_t));
    m3d->vertex[0].skinid = -1U;
    m3d->vertex[0].type = VT_WORLD;
    m3d->vertex[1].skinid = -2U;
    m3d->vertex[1].type = VT_QUATERN;

    /* get the mesh */
    for(i = 0; i < scene->meshes.count; i++)
        for(j = 0; j < scene->meshes.data[i]->instances.count; j++)
            read_mesh(m3d, scene->meshes.data[i], scene->meshes.data[i]->instances.data[j]);

    ufbx_free_scene(scene);
    return m3d;
}
