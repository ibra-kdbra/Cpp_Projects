/*
I removed C99 style comments, replaced "inline" with "__inline__", added this license block, otherwise verbatim.

https://github.com/bqqbarbhg/ufbx

ALTERNATIVE A - MIT License
Copyright (c) 2020 Samuli Raivio
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef UFBX_UFBX_H_INLCUDED
#define UFBX_UFBX_H_INLCUDED



#if defined(UFBX_CONFIG_HEADER)
	#include UFBX_CONFIG_HEADER
#endif



#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>



#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable: 4201)
	#pragma warning(disable: 4505)
	#define ufbx_inline static __forceinline
#elif defined(__GNUC__)
	#define ufbx_inline static __inline__ __attribute__((always_inline, unused))
#else
	#define ufbx_inline static
#endif

#ifndef ufbx_assert
	#include <assert.h>
	#define ufbx_assert(cond) assert(cond)
#endif


#define ufbx_nullable


#define ufbx_unsafe

#ifndef ufbx_abi
	#define ufbx_abi
#endif




typedef double ufbx_real;

#define UFBX_ERROR_STACK_MAX_DEPTH 8
#define UFBX_PANIC_MESSAGE_LENGTH 128



#if defined(__cplusplus)
	#define UFBX_LIST_TYPE(p_name, p_type) struct p_name { p_type *data; size_t count; \
		p_type &operator[](size_t index) const { ufbx_assert(index < count); return data[index]; } \
		p_type *begin() const { return data; } \
		p_type *end() const { return data + count; } }
#else
	#define UFBX_LIST_TYPE(p_name, p_type) typedef struct p_name { p_type *data; size_t count; } p_name
#endif

#if defined(__cplusplus)
	#define UFBX_VERTEX_ATTRIB_IMPL(p_type) \
		p_type &operator[](size_t index) const { ufbx_assert(index < indices.count); return values.data[indices.data[index]]; }
#else
	#define UFBX_VERTEX_ATTRIB_IMPL(p_type)
#endif

#if defined(__cplusplus) && (__cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900))
	#define UFBX_CALLBACK_IMPL(p_name, p_fn, p_params, p_args) \
		p_name() = default; \
		p_name(p_fn *f) : fn(f), user(nullptr) { } \
		template <typename F> p_name(F &f) \
			: fn([] p_params { F *f = (F*)user; return f p_args; }), user((void*)&f) { }
#else
	#define UFBX_CALLBACK_IMPL(p_name, p_fn, p_params, p_args)
#endif



#define ufbx_pack_version(major, minor, patch) ((uint32_t)(major)*1000000u + (uint32_t)(minor)*1000u + (uint32_t)(patch))
#define ufbx_version_major(version) ((uint32_t)(version)/1000000u%1000u)
#define ufbx_version_minor(version) ((uint32_t)(version)/1000u%1000u)
#define ufbx_version_patch(version) ((uint32_t)(version)%1000u)

#define UFBX_HEADER_VERSION ufbx_pack_version(0, 1, 1)



#define UFBX_NO_INDEX ((uint32_t)~0u)


typedef struct ufbx_string {
	const char *data;
	size_t length;
} ufbx_string;


typedef struct ufbx_blob {
	const void *data;
	size_t size;
} ufbx_blob;


typedef struct ufbx_vec2 {
	union {
		struct { ufbx_real x, y; };
		ufbx_real v[2];
	};
} ufbx_vec2;


typedef struct ufbx_vec3 {
	union {
		struct { ufbx_real x, y, z; };
		ufbx_real v[3];
	};
} ufbx_vec3;


typedef struct ufbx_vec4 {
	union {
		struct { ufbx_real x, y, z, w; };
		ufbx_real v[4];
	};
} ufbx_vec4;


typedef struct ufbx_quat {
	union {
		struct { ufbx_real x, y, z, w; };
		ufbx_real v[4];
	};
} ufbx_quat;





typedef enum ufbx_rotation_order {
	UFBX_ROTATION_XYZ,
	UFBX_ROTATION_XZY,
	UFBX_ROTATION_YZX,
	UFBX_ROTATION_YXZ,
	UFBX_ROTATION_ZXY,
	UFBX_ROTATION_ZYX,
	UFBX_ROTATION_SPHERIC,

	UFBX_ROTATION_ORDER_COUNT,
	UFBX_ROTATION_ORDER_FORCE_32BIT = 0x7fffffff,
} ufbx_rotation_order;



typedef struct ufbx_transform {
	ufbx_vec3 translation;
	ufbx_quat rotation;
	ufbx_vec3 scale;
} ufbx_transform;



typedef struct ufbx_matrix {
	union {
		struct {
			ufbx_real m00, m10, m20;
			ufbx_real m01, m11, m21;
			ufbx_real m02, m12, m22;
			ufbx_real m03, m13, m23;
		};
		ufbx_vec3 cols[4];
		ufbx_real v[12];
	};
} ufbx_matrix;

typedef struct ufbx_void_list {
	void *data;
	size_t count;
} ufbx_void_list;

UFBX_LIST_TYPE(ufbx_bool_list, bool);
UFBX_LIST_TYPE(ufbx_uint32_list, uint32_t);
UFBX_LIST_TYPE(ufbx_real_list, ufbx_real);
UFBX_LIST_TYPE(ufbx_vec2_list, ufbx_vec2);
UFBX_LIST_TYPE(ufbx_vec3_list, ufbx_vec3);
UFBX_LIST_TYPE(ufbx_vec4_list, ufbx_vec4);
UFBX_LIST_TYPE(ufbx_string_list, ufbx_string);



typedef enum ufbx_dom_value_type {
	UFBX_DOM_VALUE_NUMBER,
	UFBX_DOM_VALUE_STRING,
	UFBX_DOM_VALUE_ARRAY_I8,
	UFBX_DOM_VALUE_ARRAY_I32,
	UFBX_DOM_VALUE_ARRAY_I64,
	UFBX_DOM_VALUE_ARRAY_F32,
	UFBX_DOM_VALUE_ARRAY_F64,
	UFBX_DOM_VALUE_ARRAY_RAW_STRING,
	UFBX_DOM_VALUE_ARRAY_IGNORED,

	UFBX_DOM_VALUE_TYPE_COUNT,
	UFBX_DOM_VALUE_TYPE_FORCE_32BIT = 0x7fffffff,
} ufbx_dom_value_type;

typedef struct ufbx_dom_node ufbx_dom_node;

typedef struct ufbx_dom_value {
	ufbx_dom_value_type type;
	ufbx_string value_str;
	ufbx_blob value_blob;
	int64_t value_int;
	double value_float;
} ufbx_dom_value;

UFBX_LIST_TYPE(ufbx_dom_node_list, ufbx_dom_node*);
UFBX_LIST_TYPE(ufbx_dom_value_list, ufbx_dom_value);

struct ufbx_dom_node {
	ufbx_string name;
	ufbx_dom_node_list children;
	ufbx_dom_value_list values;
};









typedef struct ufbx_prop ufbx_prop;
typedef struct ufbx_props ufbx_props;





typedef enum ufbx_prop_type {
	UFBX_PROP_UNKNOWN,
	UFBX_PROP_BOOLEAN,
	UFBX_PROP_INTEGER,
	UFBX_PROP_NUMBER,
	UFBX_PROP_VECTOR,
	UFBX_PROP_COLOR,
	UFBX_PROP_COLOR_WITH_ALPHA,
	UFBX_PROP_STRING,
	UFBX_PROP_DATE_TIME,
	UFBX_PROP_TRANSLATION,
	UFBX_PROP_ROTATION,
	UFBX_PROP_SCALING,
	UFBX_PROP_DISTANCE,
	UFBX_PROP_COMPOUND,
	UFBX_PROP_BLOB,
	UFBX_PROP_REFERENCE,

	UFBX_NUM_PROP_TYPES,
	UFBX_PROP_TYPE_FORCE_32BIT = 0x7fffffff,
} ufbx_prop_type;


typedef enum ufbx_prop_flags {


	UFBX_PROP_FLAG_ANIMATABLE = 0x1,


	UFBX_PROP_FLAG_USER_DEFINED = 0x2,


	UFBX_PROP_FLAG_HIDDEN = 0x4,


	UFBX_PROP_FLAG_LOCK_X = 0x10,
	UFBX_PROP_FLAG_LOCK_Y = 0x20,
	UFBX_PROP_FLAG_LOCK_Z = 0x40,
	UFBX_PROP_FLAG_LOCK_W = 0x80,


	UFBX_PROP_FLAG_MUTE_X = 0x100,
	UFBX_PROP_FLAG_MUTE_Y = 0x200,
	UFBX_PROP_FLAG_MUTE_Z = 0x400,
	UFBX_PROP_FLAG_MUTE_W = 0x800,




	UFBX_PROP_FLAG_SYNTHETIC = 0x1000,


	UFBX_PROP_FLAG_ANIMATED = 0x2000,


	UFBX_PROP_FLAG_NOT_FOUND = 0x4000,





	UFBX_PROP_FLAG_CONNECTED = 0x8000,


	UFBX_PROP_FLAG_NO_VALUE = 0x10000,



	UFBX_PROP_FLAG_OVERRIDDEN = 0x20000,

	UFBX_PROP_FLAG_FORCE_32BIT = 0x7fffffff,
} ufbx_prop_flags;


struct ufbx_prop {
	ufbx_string name;
	uint32_t _internal_key;
	ufbx_prop_type type;
	ufbx_prop_flags flags;

	ufbx_string value_str;
	ufbx_blob value_blob;
	int64_t value_int;
	union {
		ufbx_real value_real_arr[4];
		ufbx_real value_real;
		ufbx_vec2 value_vec2;
		ufbx_vec3 value_vec3;
		ufbx_vec4 value_vec4;
	};
};

UFBX_LIST_TYPE(ufbx_prop_list, ufbx_prop);




struct ufbx_props {
	ufbx_prop_list props;
	size_t num_animated;

	ufbx_nullable ufbx_props *defaults;
};

typedef struct ufbx_scene ufbx_scene;







typedef struct ufbx_element ufbx_element;


typedef struct ufbx_unknown ufbx_unknown;


typedef struct ufbx_node ufbx_node;


typedef struct ufbx_mesh ufbx_mesh;
typedef struct ufbx_light ufbx_light;
typedef struct ufbx_camera ufbx_camera;
typedef struct ufbx_bone ufbx_bone;
typedef struct ufbx_empty ufbx_empty;


typedef struct ufbx_line_curve ufbx_line_curve;
typedef struct ufbx_nurbs_curve ufbx_nurbs_curve;
typedef struct ufbx_nurbs_surface ufbx_nurbs_surface;
typedef struct ufbx_nurbs_trim_surface ufbx_nurbs_trim_surface;
typedef struct ufbx_nurbs_trim_boundary ufbx_nurbs_trim_boundary;


typedef struct ufbx_procedural_geometry ufbx_procedural_geometry;
typedef struct ufbx_stereo_camera ufbx_stereo_camera;
typedef struct ufbx_camera_switcher ufbx_camera_switcher;
typedef struct ufbx_marker ufbx_marker;
typedef struct ufbx_lod_group ufbx_lod_group;


typedef struct ufbx_skin_deformer ufbx_skin_deformer;
typedef struct ufbx_skin_cluster ufbx_skin_cluster;
typedef struct ufbx_blend_deformer ufbx_blend_deformer;
typedef struct ufbx_blend_channel ufbx_blend_channel;
typedef struct ufbx_blend_shape ufbx_blend_shape;
typedef struct ufbx_cache_deformer ufbx_cache_deformer;
typedef struct ufbx_cache_file ufbx_cache_file;


typedef struct ufbx_material ufbx_material;
typedef struct ufbx_texture ufbx_texture;
typedef struct ufbx_video ufbx_video;
typedef struct ufbx_shader ufbx_shader;
typedef struct ufbx_shader_binding ufbx_shader_binding;


typedef struct ufbx_anim_stack ufbx_anim_stack;
typedef struct ufbx_anim_layer ufbx_anim_layer;
typedef struct ufbx_anim_value ufbx_anim_value;
typedef struct ufbx_anim_curve ufbx_anim_curve;


typedef struct ufbx_display_layer ufbx_display_layer;
typedef struct ufbx_selection_set ufbx_selection_set;
typedef struct ufbx_selection_node ufbx_selection_node;


typedef struct ufbx_character ufbx_character;
typedef struct ufbx_constraint ufbx_constraint;


typedef struct ufbx_pose ufbx_pose;
typedef struct ufbx_metadata_object ufbx_metadata_object;

UFBX_LIST_TYPE(ufbx_element_list, ufbx_element*);
UFBX_LIST_TYPE(ufbx_unknown_list, ufbx_unknown*);
UFBX_LIST_TYPE(ufbx_node_list, ufbx_node*);
UFBX_LIST_TYPE(ufbx_mesh_list, ufbx_mesh*);
UFBX_LIST_TYPE(ufbx_light_list, ufbx_light*);
UFBX_LIST_TYPE(ufbx_camera_list, ufbx_camera*);
UFBX_LIST_TYPE(ufbx_bone_list, ufbx_bone*);
UFBX_LIST_TYPE(ufbx_empty_list, ufbx_empty*);
UFBX_LIST_TYPE(ufbx_line_curve_list, ufbx_line_curve*);
UFBX_LIST_TYPE(ufbx_nurbs_curve_list, ufbx_nurbs_curve*);
UFBX_LIST_TYPE(ufbx_nurbs_surface_list, ufbx_nurbs_surface*);
UFBX_LIST_TYPE(ufbx_nurbs_trim_surface_list, ufbx_nurbs_trim_surface*);
UFBX_LIST_TYPE(ufbx_nurbs_trim_boundary_list, ufbx_nurbs_trim_boundary*);
UFBX_LIST_TYPE(ufbx_procedural_geometry_list, ufbx_procedural_geometry*);
UFBX_LIST_TYPE(ufbx_stereo_camera_list, ufbx_stereo_camera*);
UFBX_LIST_TYPE(ufbx_camera_switcher_list, ufbx_camera_switcher*);
UFBX_LIST_TYPE(ufbx_marker_list, ufbx_marker*);
UFBX_LIST_TYPE(ufbx_lod_group_list, ufbx_lod_group*);
UFBX_LIST_TYPE(ufbx_skin_deformer_list, ufbx_skin_deformer*);
UFBX_LIST_TYPE(ufbx_skin_cluster_list, ufbx_skin_cluster*);
UFBX_LIST_TYPE(ufbx_blend_deformer_list, ufbx_blend_deformer*);
UFBX_LIST_TYPE(ufbx_blend_channel_list, ufbx_blend_channel*);
UFBX_LIST_TYPE(ufbx_blend_shape_list, ufbx_blend_shape*);
UFBX_LIST_TYPE(ufbx_cache_deformer_list, ufbx_cache_deformer*);
UFBX_LIST_TYPE(ufbx_cache_file_list, ufbx_cache_file*);
UFBX_LIST_TYPE(ufbx_material_list, ufbx_material*);
UFBX_LIST_TYPE(ufbx_texture_list, ufbx_texture*);
UFBX_LIST_TYPE(ufbx_video_list, ufbx_video*);
UFBX_LIST_TYPE(ufbx_shader_list, ufbx_shader*);
UFBX_LIST_TYPE(ufbx_shader_binding_list, ufbx_shader_binding*);
UFBX_LIST_TYPE(ufbx_anim_stack_list, ufbx_anim_stack*);
UFBX_LIST_TYPE(ufbx_anim_layer_list, ufbx_anim_layer*);
UFBX_LIST_TYPE(ufbx_anim_value_list, ufbx_anim_value*);
UFBX_LIST_TYPE(ufbx_anim_curve_list, ufbx_anim_curve*);
UFBX_LIST_TYPE(ufbx_display_layer_list, ufbx_display_layer*);
UFBX_LIST_TYPE(ufbx_selection_set_list, ufbx_selection_set*);
UFBX_LIST_TYPE(ufbx_selection_node_list, ufbx_selection_node*);
UFBX_LIST_TYPE(ufbx_character_list, ufbx_character*);
UFBX_LIST_TYPE(ufbx_constraint_list, ufbx_constraint*);
UFBX_LIST_TYPE(ufbx_pose_list, ufbx_pose*);
UFBX_LIST_TYPE(ufbx_metadata_object_list, ufbx_metadata_object*);

typedef enum ufbx_element_type {
	UFBX_ELEMENT_UNKNOWN,
	UFBX_ELEMENT_NODE,
	UFBX_ELEMENT_MESH,
	UFBX_ELEMENT_LIGHT,
	UFBX_ELEMENT_CAMERA,
	UFBX_ELEMENT_BONE,
	UFBX_ELEMENT_EMPTY,
	UFBX_ELEMENT_LINE_CURVE,
	UFBX_ELEMENT_NURBS_CURVE,
	UFBX_ELEMENT_NURBS_SURFACE,
	UFBX_ELEMENT_NURBS_TRIM_SURFACE,
	UFBX_ELEMENT_NURBS_TRIM_BOUNDARY,
	UFBX_ELEMENT_PROCEDURAL_GEOMETRY,
	UFBX_ELEMENT_STEREO_CAMERA,
	UFBX_ELEMENT_CAMERA_SWITCHER,
	UFBX_ELEMENT_MARKER,
	UFBX_ELEMENT_LOD_GROUP,
	UFBX_ELEMENT_SKIN_DEFORMER,
	UFBX_ELEMENT_SKIN_CLUSTER,
	UFBX_ELEMENT_BLEND_DEFORMER,
	UFBX_ELEMENT_BLEND_CHANNEL,
	UFBX_ELEMENT_BLEND_SHAPE,
	UFBX_ELEMENT_CACHE_DEFORMER,
	UFBX_ELEMENT_CACHE_FILE,
	UFBX_ELEMENT_MATERIAL,
	UFBX_ELEMENT_TEXTURE,
	UFBX_ELEMENT_VIDEO,
	UFBX_ELEMENT_SHADER,
	UFBX_ELEMENT_SHADER_BINDING,
	UFBX_ELEMENT_ANIM_STACK,
	UFBX_ELEMENT_ANIM_LAYER,
	UFBX_ELEMENT_ANIM_VALUE,
	UFBX_ELEMENT_ANIM_CURVE,
	UFBX_ELEMENT_DISPLAY_LAYER,
	UFBX_ELEMENT_SELECTION_SET,
	UFBX_ELEMENT_SELECTION_NODE,
	UFBX_ELEMENT_CHARACTER,
	UFBX_ELEMENT_CONSTRAINT,
	UFBX_ELEMENT_POSE,
	UFBX_ELEMENT_METADATA_OBJECT,

	UFBX_ELEMENT_TYPE_COUNT,
	UFBX_ELEMENT_TYPE_FORCE_32BIT = 0x7fffffff,
	UFBX_ELEMENT_TYPE_FIRST_ATTRIB = UFBX_ELEMENT_MESH,
	UFBX_ELEMENT_TYPE_LAST_ATTRIB = UFBX_ELEMENT_LOD_GROUP,
} ufbx_element_type;




typedef struct ufbx_connection {
	ufbx_element *src;
	ufbx_element *dst;
	ufbx_string src_prop;
	ufbx_string dst_prop;
} ufbx_connection;

UFBX_LIST_TYPE(ufbx_connection_list, ufbx_connection);






struct ufbx_element {
	ufbx_string name;
	ufbx_props props;
	uint32_t element_id;
	uint32_t typed_id;
	ufbx_node_list instances;
	ufbx_element_type type;
	ufbx_connection_list connections_src;
	ufbx_connection_list connections_dst;
	ufbx_nullable ufbx_dom_node *dom_node;
	ufbx_scene *scene;
};



struct ufbx_unknown {

	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
	}; };




	ufbx_string type;
	ufbx_string super_type;
	ufbx_string sub_type;
};







typedef enum ufbx_inherit_type {
	UFBX_INHERIT_NO_SHEAR,
	UFBX_INHERIT_NORMAL,
	UFBX_INHERIT_NO_SCALE,

	UFBX_INHERIT_TYPE_COUNT,
	UFBX_INHERIT_TYPE_FORCE_32BIT = 0x7fffffff,
} ufbx_inherit_type;




struct ufbx_node {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
	}; };







	ufbx_nullable ufbx_node *parent;


	ufbx_node_list children;




	ufbx_nullable ufbx_mesh *mesh;
	ufbx_nullable ufbx_light *light;
	ufbx_nullable ufbx_camera *camera;
	ufbx_nullable ufbx_bone *bone;





	ufbx_nullable ufbx_element *attrib;


	ufbx_element_type attrib_type;





	ufbx_element_list all_attribs;



	ufbx_inherit_type inherit_type;
	ufbx_transform local_transform;
	ufbx_transform geometry_transform;




	ufbx_rotation_order rotation_order;


	ufbx_vec3 euler_rotation;



	ufbx_transform world_transform;






	ufbx_matrix node_to_parent;




	ufbx_matrix node_to_world;



	ufbx_matrix geometry_to_node;


	ufbx_matrix geometry_to_world;




	ufbx_material_list materials;


	bool visible;


	bool is_root;



	uint32_t node_depth;
};










typedef struct ufbx_vertex_attrib {
	bool exists;
	ufbx_void_list values;
	ufbx_uint32_list indices;
	size_t value_reals;
	bool unique_per_vertex;
} ufbx_vertex_attrib;


typedef struct ufbx_vertex_real {
	bool exists;
	ufbx_real_list values;
	ufbx_uint32_list indices;
	size_t value_reals;
	bool unique_per_vertex;

	UFBX_VERTEX_ATTRIB_IMPL(ufbx_real)
} ufbx_vertex_real;


typedef struct ufbx_vertex_vec2 {
	bool exists;
	ufbx_vec2_list values;
	ufbx_uint32_list indices;
	size_t value_reals;
	bool unique_per_vertex;

	UFBX_VERTEX_ATTRIB_IMPL(ufbx_vec2)
} ufbx_vertex_vec2;


typedef struct ufbx_vertex_vec3 {
	bool exists;
	ufbx_vec3_list values;
	ufbx_uint32_list indices;
	size_t value_reals;
	bool unique_per_vertex;

	UFBX_VERTEX_ATTRIB_IMPL(ufbx_vec3)
} ufbx_vertex_vec3;


typedef struct ufbx_vertex_vec4 {
	bool exists;
	ufbx_vec4_list values;
	ufbx_uint32_list indices;
	size_t value_reals;
	bool unique_per_vertex;

	UFBX_VERTEX_ATTRIB_IMPL(ufbx_vec4)
} ufbx_vertex_vec4;


typedef struct ufbx_uv_set {
	ufbx_string name;
	uint32_t index;


	ufbx_vertex_vec2 vertex_uv;
	ufbx_vertex_vec3 vertex_tangent;
	ufbx_vertex_vec3 vertex_bitangent;
} ufbx_uv_set;


typedef struct ufbx_color_set {
	ufbx_string name;
	uint32_t index;


	ufbx_vertex_vec4 vertex_color;
} ufbx_color_set;

UFBX_LIST_TYPE(ufbx_uv_set_list, ufbx_uv_set);
UFBX_LIST_TYPE(ufbx_color_set_list, ufbx_color_set);


typedef struct ufbx_edge {
	union {
		struct { uint32_t a, b; };
		uint32_t indices[2];
	};
} ufbx_edge;

UFBX_LIST_TYPE(ufbx_edge_list, ufbx_edge);






typedef struct ufbx_face {
	uint32_t index_begin;
	uint32_t num_indices;
} ufbx_face;

UFBX_LIST_TYPE(ufbx_face_list, ufbx_face);

typedef struct ufbx_mesh_material {
	ufbx_nullable ufbx_material *material;


	size_t num_faces;
	size_t num_triangles;



	ufbx_uint32_list face_indices;

} ufbx_mesh_material;

UFBX_LIST_TYPE(ufbx_mesh_material_list, ufbx_mesh_material);

typedef struct ufbx_subdivision_weight_range {
	uint32_t weight_begin;
	uint32_t num_weights;
} ufbx_subdivision_weight_range;

UFBX_LIST_TYPE(ufbx_subdivision_weight_range_list, ufbx_subdivision_weight_range);

typedef struct ufbx_subdivision_weight {
	ufbx_real weight;
	uint32_t index;
} ufbx_subdivision_weight;

UFBX_LIST_TYPE(ufbx_subdivision_weight_list, ufbx_subdivision_weight);

typedef struct ufbx_subdivision_result {
	size_t result_memory_used;
	size_t temp_memory_used;
	size_t result_allocs;
	size_t temp_allocs;



	ufbx_subdivision_weight_range_list source_vertex_ranges;
	ufbx_subdivision_weight_list source_vertex_weights;



	ufbx_subdivision_weight_range_list skin_cluster_ranges;
	ufbx_subdivision_weight_list skin_cluster_weights;

} ufbx_subdivision_result;

typedef enum ufbx_subdivision_display_mode {
	UFBX_SUBDIVISION_DISPLAY_DISABLED,
	UFBX_SUBDIVISION_DISPLAY_HULL,
	UFBX_SUBDIVISION_DISPLAY_HULL_AND_SMOOTH,
	UFBX_SUBDIVISION_DISPLAY_SMOOTH,

	UFBX_SUBDIVISION_DISPLAY_MODE_COUNT,
	UFBX_SUBDIVISION_DISPLAY_MODE_FORCE_32BIT = 0x7fffffff,
} ufbx_subdivision_display_mode;

typedef enum ufbx_subdivision_boundary {
	UFBX_SUBDIVISION_BOUNDARY_DEFAULT,
	UFBX_SUBDIVISION_BOUNDARY_LEGACY,

	UFBX_SUBDIVISION_BOUNDARY_SHARP_CORNERS,

	UFBX_SUBDIVISION_BOUNDARY_SHARP_NONE,

	UFBX_SUBDIVISION_BOUNDARY_SHARP_BOUNDARY,

	UFBX_SUBDIVISION_BOUNDARY_SHARP_INTERIOR,

	UFBX_SUBDIVISION_BOUNDARY_COUNT,
	UFBX_SUBDIVISION_BOUNDARY_FORCE_32BIT = 0x7fffffff,
} ufbx_subdivision_boundary;

















































struct ufbx_mesh {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
		ufbx_node_list instances;
	}; };



	size_t num_vertices;
	size_t num_indices;
	size_t num_faces;
	size_t num_triangles;



	size_t num_edges;


	ufbx_face_list faces;
	ufbx_bool_list face_smoothing;
	ufbx_uint32_list face_material;
	ufbx_uint32_list face_group;
	ufbx_bool_list face_hole;
	size_t max_face_triangles;
	size_t num_bad_faces;


	ufbx_edge_list edges;
	ufbx_bool_list edge_smoothing;
	ufbx_real_list edge_crease;
	ufbx_bool_list edge_visibility;



	ufbx_uint32_list vertex_indices;
	ufbx_vec3_list vertices;


	ufbx_uint32_list vertex_first_index;







	ufbx_vertex_vec3 vertex_position;
	ufbx_vertex_vec3 vertex_normal;
	ufbx_vertex_vec2 vertex_uv;
	ufbx_vertex_vec3 vertex_tangent;
	ufbx_vertex_vec3 vertex_bitangent;
	ufbx_vertex_vec4 vertex_color;
	ufbx_vertex_real vertex_crease;



	ufbx_uv_set_list uv_sets;
	ufbx_color_set_list color_sets;










	ufbx_mesh_material_list materials;





	bool skinned_is_local;
	ufbx_vertex_vec3 skinned_position;
	ufbx_vertex_vec3 skinned_normal;


	ufbx_skin_deformer_list skin_deformers;
	ufbx_blend_deformer_list blend_deformers;
	ufbx_cache_deformer_list cache_deformers;
	ufbx_element_list all_deformers;


	uint32_t subdivision_preview_levels;
	uint32_t subdivision_render_levels;
	ufbx_subdivision_display_mode subdivision_display_mode;
	ufbx_subdivision_boundary subdivision_boundary;
	ufbx_subdivision_boundary subdivision_uv_boundary;


	bool subdivision_evaluated;
	ufbx_nullable ufbx_subdivision_result *subdivision_result;


	bool from_tessellated_nurbs;
};


typedef enum ufbx_light_type {

	UFBX_LIGHT_POINT,


	UFBX_LIGHT_DIRECTIONAL,


	UFBX_LIGHT_SPOT,


	UFBX_LIGHT_AREA,


	UFBX_LIGHT_VOLUME,

	UFBX_LIGHT_TYPE_COUNT,
	UFBX_LIGHT_TYPE_FORCE_32BIT = 0x7fffffff,
} ufbx_light_type;


typedef enum ufbx_light_decay {
	UFBX_LIGHT_DECAY_NONE,
	UFBX_LIGHT_DECAY_LINEAR,
	UFBX_LIGHT_DECAY_QUADRATIC,
	UFBX_LIGHT_DECAY_CUBIC,

	UFBX_LIGHT_DECAY_COUNT,
	UFBX_LIGHT_DECAY_FORCE_32BIT = 0x7fffffff,
} ufbx_light_decay;

typedef enum ufbx_light_area_shape {
	UFBX_LIGHT_AREA_SHAPE_RECTANGLE,
	UFBX_LIGHT_AREA_SHAPE_SPHERE,

	UFBX_LIGHT_AREA_SHAPE_COUNT,
	UFBX_LIGHT_AREA_SHAPE_FORCE_32BIT = 0x7fffffff,
} ufbx_light_area_shape;


struct ufbx_light {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
		ufbx_node_list instances;
	}; };




	ufbx_vec3 color;
	ufbx_real intensity;


	ufbx_vec3 local_direction;


	ufbx_light_type type;
	ufbx_light_decay decay;
	ufbx_light_area_shape area_shape;
	ufbx_real inner_angle;
	ufbx_real outer_angle;

	bool cast_light;
	bool cast_shadows;
};



typedef enum ufbx_aspect_mode {

	UFBX_ASPECT_MODE_WINDOW_SIZE,

	UFBX_ASPECT_MODE_FIXED_RATIO,

	UFBX_ASPECT_MODE_FIXED_RESOLUTION,

	UFBX_ASPECT_MODE_FIXED_WIDTH,

	UFBX_ASPECT_MODE_FIXED_HEIGHT,

	UFBX_ASPECT_MODE_COUNT,
	UFBX_ASPECT_MODE_FORCE_32BIT = 0x7fffffff,
} ufbx_aspect_mode;



typedef enum ufbx_aperture_mode {

	UFBX_APERTURE_MODE_HORIZONTAL_AND_VERTICAL,

	UFBX_APERTURE_MODE_HORIZONTAL,

	UFBX_APERTURE_MODE_VERTICAL,

	UFBX_APERTURE_MODE_FOCAL_LENGTH,

	UFBX_APERTURE_MODE_COUNT,
	UFBX_APERTURE_MODE_FORCE_32BIT = 0x7fffffff,
} ufbx_aperture_mode;



typedef enum ufbx_gate_fit {

	UFBX_GATE_FIT_NONE,

	UFBX_GATE_FIT_VERTICAL,

	UFBX_GATE_FIT_HORIZONTAL,

	UFBX_GATE_FIT_FILL,

	UFBX_GATE_FIT_OVERSCAN,


	UFBX_GATE_FIT_STRETCH,

	UFBX_GATE_FIT_COUNT,
	UFBX_GATE_FIT_FORCE_32BIT = 0x7fffffff,
} ufbx_gate_fit;



typedef enum ufbx_aperture_format {
	UFBX_APERTURE_FORMAT_CUSTOM,
	UFBX_APERTURE_FORMAT_16MM_THEATRICAL,
	UFBX_APERTURE_FORMAT_SUPER_16MM,
	UFBX_APERTURE_FORMAT_35MM_ACADEMY,
	UFBX_APERTURE_FORMAT_35MM_TV_PROJECTION,
	UFBX_APERTURE_FORMAT_35MM_FULL_APERTURE,
	UFBX_APERTURE_FORMAT_35MM_185_PROJECTION,
	UFBX_APERTURE_FORMAT_35MM_ANAMORPHIC,
	UFBX_APERTURE_FORMAT_70MM_PROJECTION,
	UFBX_APERTURE_FORMAT_VISTAVISION,
	UFBX_APERTURE_FORMAT_DYNAVISION,
	UFBX_APERTURE_FORMAT_IMAX,

	UFBX_APERTURE_FORMAT_COUNT,
	UFBX_APERTURE_FORMAT_FORCE_32BIT = 0x7fffffff,
} ufbx_aperture_format;


struct ufbx_camera {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
		ufbx_node_list instances;
	}; };



	bool resolution_is_pixels;


	ufbx_vec2 resolution;


	ufbx_vec2 field_of_view_deg;



	ufbx_vec2 field_of_view_tan;


	ufbx_aspect_mode aspect_mode;
	ufbx_aperture_mode aperture_mode;
	ufbx_gate_fit gate_fit;
	ufbx_aperture_format aperture_format;
	ufbx_real focal_length_mm;
	ufbx_vec2 film_size_inch;
	ufbx_vec2 aperture_size_inch;
	ufbx_real squeeze_ratio;
};



struct ufbx_bone {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
		ufbx_node_list instances;
	}; };


	ufbx_real radius;


	ufbx_real relative_length;


	bool is_root;
};


struct ufbx_empty {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
		ufbx_node_list instances;
	}; };
};




typedef struct ufbx_line_segment {
	uint32_t index_begin;
	uint32_t num_indices;
} ufbx_line_segment;

UFBX_LIST_TYPE(ufbx_line_segment_list, ufbx_line_segment);

struct ufbx_line_curve {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
		ufbx_node_list instances;
	}; };

	ufbx_vec3 color;

	ufbx_vec3_list control_points;
	ufbx_uint32_list point_indices;

	ufbx_line_segment_list segments;


	bool from_tessellated_nurbs;
};

typedef enum ufbx_nurbs_topology {

	UFBX_NURBS_TOPOLOGY_OPEN,

	UFBX_NURBS_TOPOLOGY_PERIODIC,

	UFBX_NURBS_TOPOLOGY_CLOSED,

	UFBX_NURBS_TOPOLOGY_COUNT,
	UFBX_NURBS_TOPOLOGY_FORCE_32BIT = 0x7fffffff,
} ufbx_nurbs_topology;


typedef struct ufbx_nurbs_basis {



	uint32_t order;


	ufbx_nurbs_topology topology;


	ufbx_real_list knot_vector;


	ufbx_real t_min;
	ufbx_real t_max;


	ufbx_real_list spans;


	bool is_2d;







	size_t num_wrap_control_points;


	bool valid;

} ufbx_nurbs_basis;

struct ufbx_nurbs_curve {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
		ufbx_node_list instances;
	}; };


	ufbx_nurbs_basis basis;




	ufbx_vec4_list control_points;
};

struct ufbx_nurbs_surface {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
		ufbx_node_list instances;
	}; };


	ufbx_nurbs_basis basis_u;
	ufbx_nurbs_basis basis_v;


	size_t num_control_points_u;
	size_t num_control_points_v;





	ufbx_vec4_list control_points;


	uint32_t span_subdivision_u;
	uint32_t span_subdivision_v;


	bool flip_normals;



	ufbx_nullable ufbx_material *material;
};

struct ufbx_nurbs_trim_surface {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
		ufbx_node_list instances;
	}; };
};

struct ufbx_nurbs_trim_boundary {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
		ufbx_node_list instances;
	}; };
};



struct ufbx_procedural_geometry {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
		ufbx_node_list instances;
	}; };
};

struct ufbx_stereo_camera {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
		ufbx_node_list instances;
	}; };

	ufbx_camera *left;
	ufbx_camera *right;
};

struct ufbx_camera_switcher {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
		ufbx_node_list instances;
	}; };
};

typedef enum ufbx_marker_type {
	UFBX_MARKER_UNKNOWN,
	UFBX_MARKER_FK_EFFECTOR,
	UFBX_MARKER_IK_EFFECTOR,

	UFBX_MARKER_TYPE_COUNT,
	UFBX_MARKER_TYPE_FORCE_32BIT = 0x7fffffff,
} ufbx_marker_type;


struct ufbx_marker {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
		ufbx_node_list instances;
	}; };


	ufbx_marker_type type;
};


typedef enum ufbx_lod_display {
	UFBX_LOD_DISPLAY_USE_LOD,
	UFBX_LOD_DISPLAY_SHOW,
	UFBX_LOD_DISPLAY_HIDE,

	UFBX_LOD_DISPLAY_COUNT,
	UFBX_LOD_DISPLAY_FORCE_32BIT = 0x7fffffff,
} ufbx_lod_display;



typedef struct ufbx_lod_level {




	ufbx_real distance;




	ufbx_lod_display display;

} ufbx_lod_level;

UFBX_LIST_TYPE(ufbx_lod_level_list, ufbx_lod_level);



struct ufbx_lod_group {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
		ufbx_node_list instances;
	}; };


	bool relative_distances;


	ufbx_lod_level_list lod_levels;


	bool ignore_parent_transform;



	bool use_distance_limit;
	ufbx_real distance_limit_min;
	ufbx_real distance_limit_max;
};




typedef enum ufbx_skinning_method {

	UFBX_SKINNING_METHOD_LINEAR,

	UFBX_SKINNING_METHOD_RIGID,

	UFBX_SKINNING_METHOD_DUAL_QUATERNION,



	UFBX_SKINNING_METHOD_BLENDED_DQ_LINEAR,

	UFBX_SKINNING_METHOD_COUNT,
	UFBX_SKINNING_METHOD_FORCE_32BIT = 0x7fffffff,
} ufbx_skinning_method;


typedef struct ufbx_skin_vertex {





	uint32_t weight_begin;
	uint32_t num_weights;



	ufbx_real dq_weight;

} ufbx_skin_vertex;

UFBX_LIST_TYPE(ufbx_skin_vertex_list, ufbx_skin_vertex);


typedef struct ufbx_skin_weight {
	uint32_t cluster_index;
	ufbx_real weight;
} ufbx_skin_weight;

UFBX_LIST_TYPE(ufbx_skin_weight_list, ufbx_skin_weight);




struct ufbx_skin_deformer {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
	}; };

	ufbx_skinning_method skinning_method;


	ufbx_skin_cluster_list clusters;


	ufbx_skin_vertex_list vertices;
	ufbx_skin_weight_list weights;


	size_t max_weights_per_vertex;




	size_t num_dq_weights;
	ufbx_uint32_list dq_vertices;
	ufbx_real_list dq_weights;
};


struct ufbx_skin_cluster {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
	}; };




	ufbx_nullable ufbx_node *bone_node;


	ufbx_matrix geometry_to_bone;



	ufbx_matrix mesh_node_to_bone;



	ufbx_matrix bind_to_world;



	ufbx_matrix geometry_to_world;
	ufbx_transform geometry_to_world_transform;




	size_t num_weights;
	ufbx_uint32_list vertices;
	ufbx_real_list weights;
};



struct ufbx_blend_deformer {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
	}; };


	ufbx_blend_channel_list channels;
};


typedef struct ufbx_blend_keyframe {

	ufbx_blend_shape *shape;


	ufbx_real target_weight;


	ufbx_real effective_weight;
} ufbx_blend_keyframe;

UFBX_LIST_TYPE(ufbx_blend_keyframe_list, ufbx_blend_keyframe);



struct ufbx_blend_channel {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
	}; };


	ufbx_real weight;



	ufbx_blend_keyframe_list keyframes;
};


struct ufbx_blend_shape {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
	}; };



	size_t num_offsets;
	ufbx_uint32_list offset_vertices;
	ufbx_vec3_list position_offsets;
	ufbx_vec3_list normal_offsets;
};

typedef enum ufbx_cache_file_format {
	UFBX_CACHE_FILE_FORMAT_UNKNOWN,
	UFBX_CACHE_FILE_FORMAT_PC2,
	UFBX_CACHE_FILE_FORMAT_MC,

	UFBX_CACHE_FILE_FORMAT_COUNT,
	UFBX_CACHE_FILE_FORMAT_FORCE_32BIT = 0x7fffffff,
} ufbx_cache_file_format;

typedef enum ufbx_cache_data_format {
	UFBX_CACHE_DATA_FORMAT_UNKNOWN,
	UFBX_CACHE_DATA_FORMAT_REAL_FLOAT,
	UFBX_CACHE_DATA_FORMAT_VEC3_FLOAT,
	UFBX_CACHE_DATA_FORMAT_REAL_DOUBLE,
	UFBX_CACHE_DATA_FORMAT_VEC3_DOUBLE,

	UFBX_CACHE_DATA_FORMAT_COUNT,
	UFBX_CACHE_DATA_FORMAT_FORCE_32BIT = 0x7fffffff,
} ufbx_cache_data_format;

typedef enum ufbx_cache_data_encoding {
	UFBX_CACHE_DATA_ENCODING_UNKNOWN,
	UFBX_CACHE_DATA_ENCODING_LITTLE_ENDIAN,
	UFBX_CACHE_DATA_ENCODING_BIG_ENDIAN,

	UFBX_CACHE_DATA_ENCODING_COUNT,
	UFBX_CACHE_DATA_ENCODING_FORCE_32BIT = 0x7fffffff,
} ufbx_cache_data_encoding;


typedef enum ufbx_cache_interpretation {

	UFBX_CACHE_INTERPRETATION_UNKNOWN,



	UFBX_CACHE_INTERPRETATION_POINTS,


	UFBX_CACHE_INTERPRETATION_VERTEX_POSITION,


	UFBX_CACHE_INTERPRETATION_VERTEX_NORMAL,

	UFBX_CACHE_INTERPRETATION_COUNT,
	UFBX_CACHE_INTERPRETATION_FORCE_32BIT = 0x7fffffff,
} ufbx_cache_interpretation;

typedef struct ufbx_cache_frame {


	ufbx_string channel;


	double time;




	ufbx_string filename;


	ufbx_cache_file_format file_format;

	ufbx_cache_data_format data_format;
	ufbx_cache_data_encoding data_encoding;
	uint64_t data_offset;
	uint32_t data_count;
	uint32_t data_element_bytes;
	uint64_t data_total_bytes;
} ufbx_cache_frame;

UFBX_LIST_TYPE(ufbx_cache_frame_list, ufbx_cache_frame);

typedef struct ufbx_cache_channel {


	ufbx_string name;


	ufbx_cache_interpretation interpretation;



	ufbx_string interpretation_name;



	ufbx_cache_frame_list frames;

} ufbx_cache_channel;

UFBX_LIST_TYPE(ufbx_cache_channel_list, ufbx_cache_channel);

typedef struct ufbx_geometry_cache {
	ufbx_string root_filename;
	ufbx_cache_channel_list channels;
	ufbx_cache_frame_list frames;
	ufbx_string_list extra_info;
} ufbx_geometry_cache;

struct ufbx_cache_deformer {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
	}; };

	ufbx_string channel;
	ufbx_nullable ufbx_cache_file *file;


	ufbx_nullable ufbx_geometry_cache *external_cache;
	ufbx_nullable ufbx_cache_channel *external_channel;
};

struct ufbx_cache_file {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
	}; };

	ufbx_string filename;
	ufbx_string absolute_filename;
	ufbx_string relative_filename;

	ufbx_blob raw_filename;
	ufbx_blob raw_absolute_filename;
	ufbx_blob raw_relative_filename;

	ufbx_cache_file_format format;


	ufbx_nullable ufbx_geometry_cache *external_cache;
};




typedef struct ufbx_material_map {




	union {
		ufbx_real value_real;
		ufbx_vec2 value_vec2;
		ufbx_vec3 value_vec3;
		ufbx_vec4 value_vec4;
	};
	int64_t value_int;



	ufbx_nullable ufbx_texture *texture;




	bool has_value;



	bool texture_enabled;




	bool texture_inverted;


	bool feature_disabled;

} ufbx_material_map;


typedef struct ufbx_material_feature_info {



	bool enabled;


	bool is_explicit;

} ufbx_material_feature_info;


typedef struct ufbx_material_texture {
	ufbx_string material_prop;
	ufbx_string shader_prop;


	ufbx_texture *texture;

} ufbx_material_texture;

UFBX_LIST_TYPE(ufbx_material_texture_list, ufbx_material_texture);


typedef enum ufbx_shader_type {

	UFBX_SHADER_UNKNOWN,

	UFBX_SHADER_FBX_LAMBERT,

	UFBX_SHADER_FBX_PHONG,


	UFBX_SHADER_OSL_STANDARD_SURFACE,


	UFBX_SHADER_ARNOLD_STANDARD_SURFACE,


	UFBX_SHADER_3DS_MAX_PHYSICAL_MATERIAL,


	UFBX_SHADER_3DS_MAX_PBR_METAL_ROUGH,


	UFBX_SHADER_3DS_MAX_PBR_SPEC_GLOSS,


	UFBX_SHADER_GLTF_MATERIAL,


	UFBX_SHADER_SHADERFX_GRAPH,


	UFBX_SHADER_BLENDER_PHONG,

	UFBX_SHADER_TYPE_COUNT,
	UFBX_SHADER_TYPE_FORCE_32BIT = 0x7fffffff,
} ufbx_shader_type;


typedef enum ufbx_material_fbx_map {
	UFBX_MATERIAL_FBX_DIFFUSE_FACTOR,
	UFBX_MATERIAL_FBX_DIFFUSE_COLOR,
	UFBX_MATERIAL_FBX_SPECULAR_FACTOR,
	UFBX_MATERIAL_FBX_SPECULAR_COLOR,
	UFBX_MATERIAL_FBX_SPECULAR_EXPONENT,
	UFBX_MATERIAL_FBX_REFLECTION_FACTOR,
	UFBX_MATERIAL_FBX_REFLECTION_COLOR,
	UFBX_MATERIAL_FBX_TRANSPARENCY_FACTOR,
	UFBX_MATERIAL_FBX_TRANSPARENCY_COLOR,
	UFBX_MATERIAL_FBX_EMISSION_FACTOR,
	UFBX_MATERIAL_FBX_EMISSION_COLOR,
	UFBX_MATERIAL_FBX_AMBIENT_FACTOR,
	UFBX_MATERIAL_FBX_AMBIENT_COLOR,
	UFBX_MATERIAL_FBX_NORMAL_MAP,
	UFBX_MATERIAL_FBX_BUMP,
	UFBX_MATERIAL_FBX_BUMP_FACTOR,
	UFBX_MATERIAL_FBX_DISPLACEMENT_FACTOR,
	UFBX_MATERIAL_FBX_DISPLACEMENT,
	UFBX_MATERIAL_FBX_VECTOR_DISPLACEMENT_FACTOR,
	UFBX_MATERIAL_FBX_VECTOR_DISPLACEMENT,

	UFBX_MATERIAL_FBX_MAP_COUNT,
	UFBX_MATERIAL_FBX_MAP_FORCE_32BIT = 0x7fffffff,
} ufbx_material_fbx_map;


typedef enum ufbx_material_pbr_map {
	UFBX_MATERIAL_PBR_BASE_FACTOR,
	UFBX_MATERIAL_PBR_BASE_COLOR,
	UFBX_MATERIAL_PBR_ROUGHNESS,
	UFBX_MATERIAL_PBR_METALNESS,
	UFBX_MATERIAL_PBR_DIFFUSE_ROUGHNESS,
	UFBX_MATERIAL_PBR_SPECULAR_FACTOR,
	UFBX_MATERIAL_PBR_SPECULAR_COLOR,
	UFBX_MATERIAL_PBR_SPECULAR_IOR,
	UFBX_MATERIAL_PBR_SPECULAR_ANISOTROPY,
	UFBX_MATERIAL_PBR_SPECULAR_ROTATION,
	UFBX_MATERIAL_PBR_TRANSMISSION_FACTOR,
	UFBX_MATERIAL_PBR_TRANSMISSION_COLOR,
	UFBX_MATERIAL_PBR_TRANSMISSION_DEPTH,
	UFBX_MATERIAL_PBR_TRANSMISSION_SCATTER,
	UFBX_MATERIAL_PBR_TRANSMISSION_SCATTER_ANISOTROPY,
	UFBX_MATERIAL_PBR_TRANSMISSION_DISPERSION,
	UFBX_MATERIAL_PBR_TRANSMISSION_ROUGHNESS,
	UFBX_MATERIAL_PBR_TRANSMISSION_EXTRA_ROUGHNESS,
	UFBX_MATERIAL_PBR_TRANSMISSION_PRIORITY,
	UFBX_MATERIAL_PBR_TRANSMISSION_ENABLE_IN_AOV,
	UFBX_MATERIAL_PBR_SUBSURFACE_FACTOR,
	UFBX_MATERIAL_PBR_SUBSURFACE_COLOR,
	UFBX_MATERIAL_PBR_SUBSURFACE_RADIUS,
	UFBX_MATERIAL_PBR_SUBSURFACE_SCALE,
	UFBX_MATERIAL_PBR_SUBSURFACE_ANISOTROPY,
	UFBX_MATERIAL_PBR_SUBSURFACE_TINT_COLOR,
	UFBX_MATERIAL_PBR_SUBSURFACE_TYPE,
	UFBX_MATERIAL_PBR_SHEEN_FACTOR,
	UFBX_MATERIAL_PBR_SHEEN_COLOR,
	UFBX_MATERIAL_PBR_SHEEN_ROUGHNESS,
	UFBX_MATERIAL_PBR_COAT_FACTOR,
	UFBX_MATERIAL_PBR_COAT_COLOR,
	UFBX_MATERIAL_PBR_COAT_ROUGHNESS,
	UFBX_MATERIAL_PBR_COAT_IOR,
	UFBX_MATERIAL_PBR_COAT_ANISOTROPY,
	UFBX_MATERIAL_PBR_COAT_ROTATION,
	UFBX_MATERIAL_PBR_COAT_NORMAL,
	UFBX_MATERIAL_PBR_COAT_AFFECT_BASE_COLOR,
	UFBX_MATERIAL_PBR_COAT_AFFECT_BASE_ROUGHNESS,
	UFBX_MATERIAL_PBR_THIN_FILM_THICKNESS,
	UFBX_MATERIAL_PBR_THIN_FILM_IOR,
	UFBX_MATERIAL_PBR_EMISSION_FACTOR,
	UFBX_MATERIAL_PBR_EMISSION_COLOR,
	UFBX_MATERIAL_PBR_OPACITY,
	UFBX_MATERIAL_PBR_INDIRECT_DIFFUSE,
	UFBX_MATERIAL_PBR_INDIRECT_SPECULAR,
	UFBX_MATERIAL_PBR_NORMAL_MAP,
	UFBX_MATERIAL_PBR_TANGENT_MAP,
	UFBX_MATERIAL_PBR_DISPLACEMENT_MAP,
	UFBX_MATERIAL_PBR_MATTE_FACTOR,
	UFBX_MATERIAL_PBR_MATTE_COLOR,
	UFBX_MATERIAL_PBR_AMBIENT_OCCLUSION,

	UFBX_MATERIAL_PBR_MAP_COUNT,
	UFBX_MATERIAL_PBR_MAP_FORCE_32BIT = 0x7fffffff,
} ufbx_material_pbr_map;


typedef enum ufbx_material_feature {
	UFBX_MATERIAL_FEATURE_METALNESS,
	UFBX_MATERIAL_FEATURE_DIFFUSE,
	UFBX_MATERIAL_FEATURE_SPECULAR,
	UFBX_MATERIAL_FEATURE_EMISSION,
	UFBX_MATERIAL_FEATURE_TRANSMISSION,
	UFBX_MATERIAL_FEATURE_COAT,
	UFBX_MATERIAL_FEATURE_SHEEN,
	UFBX_MATERIAL_FEATURE_OPACITY,
	UFBX_MATERIAL_FEATURE_AMBIENT_OCCLUSION,
	UFBX_MATERIAL_FEATURE_MATTE,
	UFBX_MATERIAL_FEATURE_UNLIT,
	UFBX_MATERIAL_FEATURE_IOR,
	UFBX_MATERIAL_FEATURE_DIFFUSE_ROUGHNESS,
	UFBX_MATERIAL_FEATURE_TRANSMISSION_ROUGHNESS,
	UFBX_MATERIAL_FEATURE_THIN_WALLED,
	UFBX_MATERIAL_FEATURE_CAUSTICS,
	UFBX_MATERIAL_FEATURE_EXIT_TO_BACKGROUND,
	UFBX_MATERIAL_FEATURE_INTERNAL_REFLECTIONS,
	UFBX_MATERIAL_FEATURE_DOUBLE_SIDED,

	UFBX_MATERIAL_FEATURE_COUNT,
	UFBX_MATERIAL_FEATURE_FORCE_32BIT = 0x7fffffff,
} ufbx_material_feature;

typedef struct ufbx_material_fbx_maps {
	union {
		ufbx_material_map maps[UFBX_MATERIAL_FBX_MAP_COUNT];
		struct {
			ufbx_material_map diffuse_factor;
			ufbx_material_map diffuse_color;
			ufbx_material_map specular_factor;
			ufbx_material_map specular_color;
			ufbx_material_map specular_exponent;
			ufbx_material_map reflection_factor;
			ufbx_material_map reflection_color;
			ufbx_material_map transparency_factor;
			ufbx_material_map transparency_color;
			ufbx_material_map emission_factor;
			ufbx_material_map emission_color;
			ufbx_material_map ambient_factor;
			ufbx_material_map ambient_color;
			ufbx_material_map normal_map;
			ufbx_material_map bump;
			ufbx_material_map bump_factor;
			ufbx_material_map displacement_factor;
			ufbx_material_map displacement;
			ufbx_material_map vector_displacement_factor;
			ufbx_material_map vector_displacement;
		};
	};
} ufbx_material_fbx_maps;

typedef struct ufbx_material_pbr_maps {
	union {
		ufbx_material_map maps[UFBX_MATERIAL_PBR_MAP_COUNT];
		struct {
			ufbx_material_map base_factor;
			ufbx_material_map base_color;
			ufbx_material_map roughness;
			ufbx_material_map metalness;
			ufbx_material_map diffuse_roughness;
			ufbx_material_map specular_factor;
			ufbx_material_map specular_color;
			ufbx_material_map specular_ior;
			ufbx_material_map specular_anisotropy;
			ufbx_material_map specular_rotation;
			ufbx_material_map transmission_factor;
			ufbx_material_map transmission_color;
			ufbx_material_map transmission_depth;
			ufbx_material_map transmission_scatter;
			ufbx_material_map transmission_scatter_anisotropy;
			ufbx_material_map transmission_dispersion;
			ufbx_material_map transmission_roughness;
			ufbx_material_map transmission_extra_roughness;
			ufbx_material_map transmission_priority;
			ufbx_material_map transmission_enable_in_aov;
			ufbx_material_map subsurface_factor;
			ufbx_material_map subsurface_color;
			ufbx_material_map subsurface_radius;
			ufbx_material_map subsurface_scale;
			ufbx_material_map subsurface_anisotropy;
			ufbx_material_map subsurface_tint_color;
			ufbx_material_map subsurface_type;
			ufbx_material_map sheen_factor;
			ufbx_material_map sheen_color;
			ufbx_material_map sheen_roughness;
			ufbx_material_map coat_factor;
			ufbx_material_map coat_color;
			ufbx_material_map coat_roughness;
			ufbx_material_map coat_ior;
			ufbx_material_map coat_anisotropy;
			ufbx_material_map coat_rotation;
			ufbx_material_map coat_normal;
			ufbx_material_map coat_affect_base_color;
			ufbx_material_map coat_affect_base_roughness;
			ufbx_material_map thin_film_thickness;
			ufbx_material_map thin_film_ior;
			ufbx_material_map emission_factor;
			ufbx_material_map emission_color;
			ufbx_material_map opacity;
			ufbx_material_map indirect_diffuse;
			ufbx_material_map indirect_specular;
			ufbx_material_map normal_map;
			ufbx_material_map tangent_map;
			ufbx_material_map displacement_map;
			ufbx_material_map matte_factor;
			ufbx_material_map matte_color;
			ufbx_material_map ambient_occlusion;
		};
	};
} ufbx_material_pbr_maps;

typedef struct ufbx_material_features {
	union {
		ufbx_material_feature_info features[UFBX_MATERIAL_FEATURE_COUNT];
		struct {
			ufbx_material_feature_info metalness;
			ufbx_material_feature_info diffuse;
			ufbx_material_feature_info specular;
			ufbx_material_feature_info emission;
			ufbx_material_feature_info transmission;
			ufbx_material_feature_info coat;
			ufbx_material_feature_info sheen;
			ufbx_material_feature_info opacity;
			ufbx_material_feature_info ambient_occlusion;
			ufbx_material_feature_info matte;
			ufbx_material_feature_info unlit;
			ufbx_material_feature_info ior;
			ufbx_material_feature_info diffuse_roughness;
			ufbx_material_feature_info transmission_roughness;
			ufbx_material_feature_info thin_walled;
			ufbx_material_feature_info caustics;
			ufbx_material_feature_info exit_to_background;
			ufbx_material_feature_info internal_reflections;
			ufbx_material_feature_info double_sided;
		};
	};
} ufbx_material_features;



struct ufbx_material {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
	}; };



	ufbx_material_fbx_maps fbx;



	ufbx_material_pbr_maps pbr;


	ufbx_material_features features;


	ufbx_shader_type shader_type;
	ufbx_nullable ufbx_shader *shader;
	ufbx_string shading_model_name;





	ufbx_string shader_prop_prefix;



	ufbx_material_texture_list textures;
};

typedef enum ufbx_texture_type {





	UFBX_TEXTURE_FILE,


	UFBX_TEXTURE_LAYERED,


	UFBX_TEXTURE_PROCEDURAL,



	UFBX_TEXTURE_SHADER,

	UFBX_TEXTURE_TYPE_COUNT,
	UFBX_TEXTURE_TYPE_FORCE_32BIT = 0x7fffffff,
} ufbx_texture_type;





typedef enum ufbx_blend_mode {
	UFBX_BLEND_TRANSLUCENT,
	UFBX_BLEND_ADDITIVE,
	UFBX_BLEND_MULTIPLY,
	UFBX_BLEND_MULTIPLY_2X,
	UFBX_BLEND_OVER,
	UFBX_BLEND_REPLACE,
	UFBX_BLEND_DISSOLVE,
	UFBX_BLEND_DARKEN,
	UFBX_BLEND_COLOR_BURN,
	UFBX_BLEND_LINEAR_BURN,
	UFBX_BLEND_DARKER_COLOR,
	UFBX_BLEND_LIGHTEN,
	UFBX_BLEND_SCREEN,
	UFBX_BLEND_COLOR_DODGE,
	UFBX_BLEND_LINEAR_DODGE,
	UFBX_BLEND_LIGHTER_COLOR,
	UFBX_BLEND_SOFT_LIGHT,
	UFBX_BLEND_HARD_LIGHT,
	UFBX_BLEND_VIVID_LIGHT,
	UFBX_BLEND_LINEAR_LIGHT,
	UFBX_BLEND_PIN_LIGHT,
	UFBX_BLEND_HARD_MIX,
	UFBX_BLEND_DIFFERENCE,
	UFBX_BLEND_EXCLUSION,
	UFBX_BLEND_SUBTRACT,
	UFBX_BLEND_DIVIDE,
	UFBX_BLEND_HUE,
	UFBX_BLEND_SATURATION,
	UFBX_BLEND_COLOR,
	UFBX_BLEND_LUMINOSITY,
	UFBX_BLEND_OVERLAY,

	UFBX_BLEND_MODE_COUNT,
	UFBX_BLEND_MODE_FORCE_32BIT = 0x7fffffff,
} ufbx_blend_mode;


typedef enum ufbx_wrap_mode {
	UFBX_WRAP_REPEAT,
	UFBX_WRAP_CLAMP,

	UFBX_WRAP_MODE_COUNT,
	UFBX_WRAP_MODE_FORCE_32BIT = 0x7fffffff,
} ufbx_wrap_mode;


typedef struct ufbx_texture_layer {
	ufbx_texture *texture;
	ufbx_blend_mode blend_mode;
	ufbx_real alpha;
} ufbx_texture_layer;

UFBX_LIST_TYPE(ufbx_texture_layer_list, ufbx_texture_layer);

typedef enum ufbx_shader_texture_type {
	UFBX_SHADER_TEXTURE_UNKNOWN,




	UFBX_SHADER_TEXTURE_SELECT_OUTPUT,



	UFBX_SHADER_TEXTURE_OSL,

	UFBX_SHADER_TEXTURE_TYPE_COUNT,
	UFBX_SHADER_TEXTURE_TYPE_FORCE_32BIT = 0x7fffffff,
} ufbx_shader_texture_type;

typedef struct ufbx_shader_texture_input {


	ufbx_string name;


	union {
		ufbx_real value_real;
		ufbx_vec2 value_vec2;
		ufbx_vec3 value_vec3;
		ufbx_vec4 value_vec4;
	};
	int64_t value_int;
	ufbx_string value_str;
	ufbx_blob value_blob;


	ufbx_nullable ufbx_texture *texture;


	int64_t texture_output_index;



	bool texture_enabled;


	ufbx_prop *prop;


	ufbx_nullable ufbx_prop *texture_prop;


	ufbx_nullable ufbx_prop *texture_enabled_prop;

} ufbx_shader_texture_input;

UFBX_LIST_TYPE(ufbx_shader_texture_input_list, ufbx_shader_texture_input);

typedef struct ufbx_shader_texture {


	ufbx_shader_texture_type type;


	ufbx_string shader_name;


	uint64_t shader_type_id;



	ufbx_shader_texture_input_list inputs;


	ufbx_string shader_source;
	ufbx_blob raw_shader_source;




	ufbx_texture *main_texture;


	int64_t main_texture_output_index;



	ufbx_string prop_prefix;

} ufbx_shader_texture;


struct ufbx_texture {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
	}; };


	ufbx_texture_type type;


	ufbx_string filename;
	ufbx_string absolute_filename;
	ufbx_string relative_filename;
	ufbx_blob raw_filename;
	ufbx_blob raw_absolute_filename;
	ufbx_blob raw_relative_filename;


	ufbx_blob content;


	ufbx_nullable ufbx_video *video;


	ufbx_texture_layer_list layers;




	ufbx_nullable ufbx_shader_texture *shader;



	ufbx_texture_list file_textures;


	ufbx_string uv_set;


	ufbx_wrap_mode wrap_u;
	ufbx_wrap_mode wrap_v;


	ufbx_transform transform;
	ufbx_matrix texture_to_uv;
	ufbx_matrix uv_to_texture;
};


struct ufbx_video {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
	}; };


	ufbx_string filename;
	ufbx_string absolute_filename;
	ufbx_string relative_filename;
	ufbx_blob raw_filename;
	ufbx_blob raw_absolute_filename;
	ufbx_blob raw_relative_filename;


	ufbx_blob content;
};



struct ufbx_shader {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
	}; };


	ufbx_shader_type type;





	ufbx_shader_binding_list bindings;
};


typedef struct ufbx_shader_prop_binding {
	ufbx_string shader_prop;
	ufbx_string material_prop;
} ufbx_shader_prop_binding;

UFBX_LIST_TYPE(ufbx_shader_prop_binding_list, ufbx_shader_prop_binding);


struct ufbx_shader_binding {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
	}; };

	ufbx_shader_prop_binding_list prop_bindings;
};



typedef struct ufbx_anim_layer_desc {
	ufbx_anim_layer *layer;
	ufbx_real weight;
} ufbx_anim_layer_desc;

UFBX_LIST_TYPE(ufbx_const_anim_layer_desc_list, const ufbx_anim_layer_desc);

typedef struct ufbx_prop_override {


	uint32_t element_id;


	const char *prop_name;



	ufbx_vec3 value;
	const char *value_str;
	int64_t value_int;


	uint32_t _internal_key;
} ufbx_prop_override;

UFBX_LIST_TYPE(ufbx_const_prop_override_list, const ufbx_prop_override);

typedef struct ufbx_anim {
	ufbx_const_anim_layer_desc_list layers;



	ufbx_const_prop_override_list prop_overrides;

	bool ignore_connections;


	double time_begin;
	double time_end;
} ufbx_anim;

struct ufbx_anim_stack {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
	}; };

	double time_begin;
	double time_end;

	ufbx_anim_layer_list layers;
	ufbx_anim anim;
};

typedef struct ufbx_anim_prop {
	ufbx_element *element;
	uint32_t _internal_key;
	ufbx_string prop_name;
	ufbx_anim_value *anim_value;
} ufbx_anim_prop;

UFBX_LIST_TYPE(ufbx_anim_prop_list, ufbx_anim_prop);

struct ufbx_anim_layer {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
	}; };

	ufbx_real weight;
	bool weight_is_animated;
	bool blended;
	bool additive;
	bool compose_rotation;
	bool compose_scale;

	ufbx_anim_value_list anim_values;
	ufbx_anim_prop_list anim_props;

	ufbx_anim anim;

	uint32_t _min_element_id;
	uint32_t _max_element_id;
	uint32_t _element_id_bitmask[4];
};

struct ufbx_anim_value {
	union { ufbx_element element; struct { ufbx_string name; ufbx_props props; }; };

	ufbx_vec3 default_value;
	ufbx_nullable ufbx_anim_curve *curves[3];
};


typedef enum ufbx_interpolation {
	UFBX_INTERPOLATION_CONSTANT_PREV,
	UFBX_INTERPOLATION_CONSTANT_NEXT,
	UFBX_INTERPOLATION_LINEAR,
	UFBX_INTERPOLATION_CUBIC,

	UFBX_INTERPOLATION_COUNT,
	UFBX_INTERPOLATION_FORCE_32BIT = 0x7fffffff,
} ufbx_interpolation;


typedef struct ufbx_tangent {
	float dx;
	float dy;
} ufbx_tangent;













typedef struct ufbx_keyframe {
	double time;
	ufbx_real value;
	ufbx_interpolation interpolation;
	ufbx_tangent left;
	ufbx_tangent right;
} ufbx_keyframe;

UFBX_LIST_TYPE(ufbx_keyframe_list, ufbx_keyframe);

struct ufbx_anim_curve {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
	}; };

	ufbx_keyframe_list keyframes;
};




struct ufbx_display_layer {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
	}; };


	ufbx_node_list nodes;


	bool visible;
	bool frozen;

	ufbx_vec3 ui_color;
};


struct ufbx_selection_set {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
	}; };


	ufbx_selection_node_list nodes;
};


struct ufbx_selection_node {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
	}; };


	ufbx_nullable ufbx_node *target_node;
	ufbx_nullable ufbx_mesh *target_mesh;
	bool include_node;




	ufbx_uint32_list vertices;
	ufbx_uint32_list edges;
	ufbx_uint32_list faces;
};



struct ufbx_character {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
	}; };
};


typedef enum ufbx_constraint_type {
	UFBX_CONSTRAINT_UNKNOWN,
	UFBX_CONSTRAINT_AIM,
	UFBX_CONSTRAINT_PARENT,
	UFBX_CONSTRAINT_POSITION,
	UFBX_CONSTRAINT_ROTATION,
	UFBX_CONSTRAINT_SCALE,


	UFBX_CONSTRAINT_SINGLE_CHAIN_IK,

	UFBX_CONSTRAINT_TYPE_COUNT,
	UFBX_CONSTRAINT_TYPE_FORCE_32BIT = 0x7fffffff,
} ufbx_constraint_type;


typedef struct ufbx_constraint_target {
	ufbx_node *node;
	ufbx_real weight;
	ufbx_transform transform;
} ufbx_constraint_target;

UFBX_LIST_TYPE(ufbx_constraint_target_list, ufbx_constraint_target);


typedef enum ufbx_constraint_aim_up_type {
	UFBX_CONSTRAINT_AIM_UP_SCENE,
	UFBX_CONSTRAINT_AIM_UP_TO_NODE,
	UFBX_CONSTRAINT_AIM_UP_ALIGN_NODE,
	UFBX_CONSTRAINT_AIM_UP_VECTOR,
	UFBX_CONSTRAINT_AIM_UP_NONE,

	UFBX_CONSTRAINT_AIM_UP_TYPE_COUNT,
	UFBX_CONSTRAINT_AIM_UP_TYPE_FORCE_32BIT = 0x7fffffff,
} ufbx_constraint_aim_up_type;


typedef enum ufbx_constraint_ik_pole_type {
	UFBX_CONSTRAINT_IK_POLE_VECTOR,
	UFBX_CONSTRAINT_IK_POLE_NODE,

	UFBX_CONSTRAINT_IK_POLE_TYPE_COUNT,
	UFBX_CONSTRAINT_IK_POLE_TYPE_FORCE_32BIT = 0x7fffffff,
} ufbx_constraint_ik_pole_type;

struct ufbx_constraint {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
	}; };


	ufbx_constraint_type type;
	ufbx_string type_name;


	ufbx_nullable ufbx_node *node;


	ufbx_constraint_target_list targets;


	ufbx_real weight;
	bool active;


	bool constrain_translation[3];
	bool constrain_rotation[3];
	bool constrain_scale[3];


	ufbx_transform transform_offset;


	ufbx_vec3 aim_vector;
	ufbx_constraint_aim_up_type aim_up_type;
	ufbx_nullable ufbx_node *aim_up_node;
	ufbx_vec3 aim_up_vector;


	ufbx_nullable ufbx_node *ik_effector;
	ufbx_nullable ufbx_node *ik_end_node;
	ufbx_vec3 ik_pole_vector;
};



typedef struct ufbx_bone_pose {
	ufbx_node *bone_node;
	ufbx_matrix bone_to_world;
} ufbx_bone_pose;

UFBX_LIST_TYPE(ufbx_bone_pose_list, ufbx_bone_pose);

struct ufbx_pose {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
	}; };

	bool bind_pose;
	ufbx_bone_pose_list bone_poses;
};

struct ufbx_metadata_object {
	union { ufbx_element element; struct {
		ufbx_string name;
		ufbx_props props;
		uint32_t element_id;
		uint32_t typed_id;
	}; };
};



typedef struct ufbx_name_element {
	ufbx_string name;
	ufbx_element_type type;
	uint32_t _internal_key;
	ufbx_element *element;
} ufbx_name_element;

UFBX_LIST_TYPE(ufbx_name_element_list, ufbx_name_element);





typedef enum ufbx_exporter {
	UFBX_EXPORTER_UNKNOWN,
	UFBX_EXPORTER_FBX_SDK,
	UFBX_EXPORTER_BLENDER_BINARY,
	UFBX_EXPORTER_BLENDER_ASCII,
	UFBX_EXPORTER_MOTION_BUILDER,
	UFBX_EXPORTER_BC_UNITY_EXPORTER,

	UFBX_EXPORTER_COUNT,
	UFBX_EXPORTER_FORCE_32BIT = 0x7fffffff,
} ufbx_exporter;

typedef struct ufbx_application {
	ufbx_string vendor;
	ufbx_string name;
	ufbx_string version;
} ufbx_application;


typedef struct ufbx_metadata {


	bool ascii;


	uint32_t version;



	bool may_contain_no_index;



	bool unsafe;

	ufbx_string creator;
	bool big_endian;

	ufbx_string filename;
	ufbx_string relative_root;

	ufbx_blob raw_filename;
	ufbx_blob raw_relative_root;

	ufbx_exporter exporter;
	uint32_t exporter_version;

	ufbx_props scene_props;

	ufbx_application original_application;
	ufbx_application latest_application;

	bool geometry_ignored;
	bool animation_ignored;
	bool embedded_ignored;

	size_t max_face_triangles;

	size_t result_memory_used;
	size_t temp_memory_used;
	size_t result_allocs;
	size_t temp_allocs;

	size_t element_buffer_size;
	size_t num_shader_textures;

	ufbx_real bone_prop_size_unit;
	bool bone_prop_limb_length_relative;
	double ktime_to_sec;

	ufbx_string original_file_path;
	ufbx_blob raw_original_file_path;

} ufbx_metadata;

typedef enum ufbx_coordinate_axis {
	UFBX_COORDINATE_AXIS_POSITIVE_X,
	UFBX_COORDINATE_AXIS_NEGATIVE_X,
	UFBX_COORDINATE_AXIS_POSITIVE_Y,
	UFBX_COORDINATE_AXIS_NEGATIVE_Y,
	UFBX_COORDINATE_AXIS_POSITIVE_Z,
	UFBX_COORDINATE_AXIS_NEGATIVE_Z,
	UFBX_COORDINATE_AXIS_UNKNOWN,

	UFBX_COORDINATE_AXIS_COUNT,
	UFBX_COORDINATE_AXIS_FORCE_32BIT = 0x7fffffff,
} ufbx_coordinate_axis;



typedef struct ufbx_coordinate_axes {
	ufbx_coordinate_axis right;
	ufbx_coordinate_axis up;
	ufbx_coordinate_axis front;
} ufbx_coordinate_axes;

typedef enum ufbx_time_mode {
	UFBX_TIME_MODE_DEFAULT,
	UFBX_TIME_MODE_120_FPS,
	UFBX_TIME_MODE_100_FPS,
	UFBX_TIME_MODE_60_FPS,
	UFBX_TIME_MODE_50_FPS,
	UFBX_TIME_MODE_48_FPS,
	UFBX_TIME_MODE_30_FPS,
	UFBX_TIME_MODE_30_FPS_DROP,
	UFBX_TIME_MODE_NTSC_DROP_FRAME,
	UFBX_TIME_MODE_NTSC_FULL_FRAME,
	UFBX_TIME_MODE_PAL,
	UFBX_TIME_MODE_24_FPS,
	UFBX_TIME_MODE_1000_FPS,
	UFBX_TIME_MODE_FILM_FULL_FRAME,
	UFBX_TIME_MODE_CUSTOM,
	UFBX_TIME_MODE_96_FPS,
	UFBX_TIME_MODE_72_FPS,
	UFBX_TIME_MODE_59_94_FPS,

	UFBX_TIME_MODE_COUNT,
	UFBX_TIME_MODE_FORCE_32BIT = 0x7fffffff,
} ufbx_time_mode;

typedef enum ufbx_time_protocol {
	UFBX_TIME_PROTOCOL_SMPTE,
	UFBX_TIME_PROTOCOL_FRAME_COUNT,
	UFBX_TIME_PROTOCOL_DEFAULT,

	UFBX_TIME_PROTOCOL_COUNT,
	UFBX_TIME_PROTOCOL_FORCE_32BIT = 0x7fffffff,
} ufbx_time_protocol;

typedef enum ufbx_snap_mode {
	UFBX_SNAP_MODE_NONE,
	UFBX_SNAP_MODE_SNAP,
	UFBX_SNAP_MODE_PLAY,
	UFBX_SNAP_MODE_SNAP_AND_PLAY,

	UFBX_SNAP_MODE_COUNT,
	UFBX_SNAP_MODE_FORCE_32BIT = 0x7fffffff,
} ufbx_snap_mode;


typedef struct ufbx_scene_settings {
	ufbx_props props;




	ufbx_coordinate_axes axes;




	ufbx_real unit_meters;

	double frames_per_second;

	ufbx_vec3 ambient_color;
	ufbx_string default_camera;

	ufbx_time_mode time_mode;
	ufbx_time_protocol time_protocol;
	ufbx_snap_mode snap_mode;


	ufbx_coordinate_axis original_axis_up;
	ufbx_real original_unit_meters;
} ufbx_scene_settings;

struct ufbx_scene {
	ufbx_metadata metadata;


	ufbx_scene_settings settings;


	ufbx_node *root_node;


	ufbx_anim anim;


	ufbx_anim combined_anim;

	union {
		struct {
			ufbx_unknown_list unknowns;


			ufbx_node_list nodes;


			ufbx_mesh_list meshes;
			ufbx_light_list lights;
			ufbx_camera_list cameras;
			ufbx_bone_list bones;
			ufbx_empty_list empties;


			ufbx_line_curve_list line_curves;
			ufbx_nurbs_curve_list nurbs_curves;
			ufbx_nurbs_surface_list nurbs_surfaces;
			ufbx_nurbs_trim_surface_list nurbs_trim_surfaces;
			ufbx_nurbs_trim_boundary_list nurbs_trim_boundaries;


			ufbx_procedural_geometry_list procedural_geometries;
			ufbx_stereo_camera_list stereo_cameras;
			ufbx_camera_switcher_list camera_switchers;
			ufbx_marker_list markers;
			ufbx_lod_group_list lod_groups;


			ufbx_skin_deformer_list skin_deformers;
			ufbx_skin_cluster_list skin_clusters;
			ufbx_blend_deformer_list blend_deformers;
			ufbx_blend_channel_list blend_channels;
			ufbx_blend_shape_list blend_shapes;
			ufbx_cache_deformer_list cache_deformers;
			ufbx_cache_file_list cache_files;


			ufbx_material_list materials;
			ufbx_texture_list textures;
			ufbx_video_list videos;
			ufbx_shader_list shaders;
			ufbx_shader_binding_list shader_bindings;


			ufbx_anim_stack_list anim_stacks;
			ufbx_anim_layer_list anim_layers;
			ufbx_anim_value_list anim_values;
			ufbx_anim_curve_list anim_curves;


			ufbx_display_layer_list display_layers;
			ufbx_selection_set_list selection_sets;
			ufbx_selection_node_list selection_nodes;


			ufbx_character_list characters;
			ufbx_constraint_list constraints;


			ufbx_pose_list poses;
			ufbx_metadata_object_list metadata_objects;
		};

		ufbx_element_list elements_by_type[UFBX_ELEMENT_TYPE_COUNT];
	};


	ufbx_element_list elements;
	ufbx_connection_list connections_src;
	ufbx_connection_list connections_dst;


	ufbx_name_element_list elements_by_name;


	ufbx_nullable ufbx_dom_node *dom_root;
};



typedef struct ufbx_curve_point {
	bool valid;
	ufbx_vec3 position;
	ufbx_vec3 derivative;
} ufbx_curve_point;

typedef struct ufbx_surface_point {
	bool valid;
	ufbx_vec3 position;
	ufbx_vec3 derivative_u;
	ufbx_vec3 derivative_v;
} ufbx_surface_point;



typedef enum ufbx_topo_flags {
	UFBX_TOPO_NON_MANIFOLD = 0x1,

	UFBX_TOPO_FLAGS_FORCE_32BIT = 0x7fffffff,
} ufbx_topo_flags;

typedef struct ufbx_topo_edge {
	uint32_t index;
	uint32_t next;
	uint32_t prev;
	uint32_t twin;
	uint32_t face;
	uint32_t edge;

	ufbx_topo_flags flags;
} ufbx_topo_edge;

typedef struct ufbx_vertex_stream {
	const void *data;
	size_t vertex_size;
} ufbx_vertex_stream;







typedef void *ufbx_alloc_fn(void *user, size_t size);





typedef void *ufbx_realloc_fn(void *user, void *old_ptr, size_t old_size, size_t new_size);


typedef void ufbx_free_fn(void *user, void *ptr, size_t size);


typedef void ufbx_free_allocator_fn(void *user);





typedef struct ufbx_allocator {

	ufbx_alloc_fn *alloc_fn;
	ufbx_realloc_fn *realloc_fn;
	ufbx_free_fn *free_fn;
	ufbx_free_allocator_fn *free_allocator_fn;
	void *user;
} ufbx_allocator;

typedef struct ufbx_allocator_opts {

	ufbx_allocator allocator;


	size_t memory_limit;


	size_t allocation_limit;





	size_t huge_threshold;











	size_t max_chunk_size;

} ufbx_allocator_opts;





typedef size_t ufbx_read_fn(void *user, void *data, size_t size);


typedef bool ufbx_skip_fn(void *user, size_t size);


typedef void ufbx_close_fn(void *user);

typedef struct ufbx_stream {
	ufbx_read_fn *read_fn;
	ufbx_skip_fn *skip_fn;
	ufbx_close_fn *close_fn;


	void *user;
} ufbx_stream;


typedef bool ufbx_open_file_fn(void *user, ufbx_stream *stream, const char *path, size_t path_len);

typedef struct ufbx_open_file_cb {
	ufbx_open_file_fn *fn;
	void *user;

	UFBX_CALLBACK_IMPL(ufbx_open_file_cb, ufbx_open_file_fn,
		(void *user, ufbx_stream *stream, const char *path, size_t path_len),
		(stream, path, path_len))
} ufbx_open_file_cb;


typedef struct ufbx_error_frame {
	uint32_t source_line;
	ufbx_string function;
	ufbx_string description;
} ufbx_error_frame;


typedef enum ufbx_error_type {


	UFBX_ERROR_NONE,



	UFBX_ERROR_UNKNOWN,


	UFBX_ERROR_FILE_NOT_FOUND,


	UFBX_ERROR_OUT_OF_MEMORY,


	UFBX_ERROR_MEMORY_LIMIT,


	UFBX_ERROR_ALLOCATION_LIMIT,


	UFBX_ERROR_TRUNCATED_FILE,



	UFBX_ERROR_IO,


	UFBX_ERROR_CANCELLED,


	UFBX_ERROR_NOT_FBX,





	UFBX_ERROR_UNINITIALIZED_OPTIONS,


	UFBX_ERROR_ZERO_VERTEX_SIZE,


	UFBX_ERROR_INVALID_UTF8,


	UFBX_ERROR_FEATURE_DISABLED,



	UFBX_ERROR_BAD_NURBS,


	UFBX_ERROR_BAD_INDEX,


	UFBX_ERROR_UNSAFE_OPTIONS,

	UFBX_ERROR_TYPE_COUNT,
	UFBX_ERROR_TYPE_FORCE_32BIT = 0x7fffffff,
} ufbx_error_type;



typedef struct ufbx_error {
	ufbx_error_type type;
	ufbx_string description;
	uint32_t stack_size;
	ufbx_error_frame stack[UFBX_ERROR_STACK_MAX_DEPTH];
} ufbx_error;



typedef struct ufbx_progress {
	uint64_t bytes_read;
	uint64_t bytes_total;
} ufbx_progress;

typedef enum ufbx_progress_result {
	UFBX_PROGRESS_CONTINUE = 0x100,
	UFBX_PROGRESS_CANCEL = 0x200,

	UFBX_PROGRESS_FORCE_32BIT = 0x7fffffff,
} ufbx_progress_result;



typedef ufbx_progress_result ufbx_progress_fn(void *user, const ufbx_progress *progress);

typedef struct ufbx_progress_cb {
	ufbx_progress_fn *fn;
	void *user;

	UFBX_CALLBACK_IMPL(ufbx_progress_cb, ufbx_progress_fn,
		(void *user, const ufbx_progress *progress),
		(progress))
} ufbx_progress_cb;



typedef struct ufbx_inflate_input ufbx_inflate_input;
typedef struct ufbx_inflate_retain ufbx_inflate_retain;


struct ufbx_inflate_input {

	size_t total_size;


	const void *data;
	size_t data_size;


	void *buffer;
	size_t buffer_size;


	ufbx_read_fn *read_fn;
	void *read_user;


	ufbx_progress_cb progress_cb;
	uint64_t progress_interval_hint;


	uint64_t progress_size_before;
	uint64_t progress_size_after;


	bool no_header;


	bool no_checksum;
};



struct ufbx_inflate_retain {
	bool initialized;
	uint64_t data[512];
};

typedef enum ufbx_index_error_handling {

	UFBX_INDEX_ERROR_HANDLING_CLAMP,




	UFBX_INDEX_ERROR_HANDLING_NO_INDEX,

	UFBX_INDEX_ERROR_HANDLING_ABORT_LOADING,




	UFBX_INDEX_ERROR_HANDLING_UNSAFE_IGNORE,

	UFBX_INDEX_ERROR_HANDLING_COUNT,
	UFBX_INDEX_ERROR_HANDLING_FORCE_32BIT = 0x7fffffff,
} ufbx_index_error_handling;

typedef enum ufbx_unicode_error_handling {

	UFBX_UNICODE_ERROR_HANDLING_REPLACEMENT_CHARACTER,

	UFBX_UNICODE_ERROR_HANDLING_UNDERSCORE,

	UFBX_UNICODE_ERROR_HANDLING_QUESTION_MARK,

	UFBX_UNICODE_ERROR_HANDLING_REMOVE,

	UFBX_UNICODE_ERROR_HANDLING_ABORT_LOADING,



	UFBX_UNICODE_ERROR_HANDLING_UNSAFE_IGNORE,

	UFBX_UNICODE_ERROR_HANDLING_COUNT,
	UFBX_UNICODE_ERROR_HANDLING_FORCE_32BIT = 0x7fffffff,
} ufbx_unicode_error_handling;





typedef struct ufbx_load_opts {
	uint32_t _begin_zero;

	ufbx_allocator_opts temp_allocator;
	ufbx_allocator_opts result_allocator;


	bool ignore_geometry;
	bool ignore_animation;
	bool ignore_embedded;
	bool evaluate_skinning;
	bool evaluate_caches;


	bool load_external_files;



	bool skip_skin_vertices;


	bool disable_quirks;


	bool strict;



	ufbx_unsafe bool allow_unsafe;


	ufbx_index_error_handling index_error_handling;




	bool connect_broken_elements;



	bool allow_nodes_out_of_root;




	bool allow_null_material;


	bool generate_missing_normals;


	char path_separator;


	uint64_t file_size_estimate;


	size_t read_buffer_size;




	ufbx_string filename;



	ufbx_blob raw_filename;


	ufbx_progress_cb progress_cb;
	uint64_t progress_interval_hint;


	ufbx_open_file_cb open_file_cb;



	ufbx_coordinate_axes target_axes;



	ufbx_real target_unit_meters;


	bool no_prop_unit_scaling;


	bool no_anim_curve_unit_scaling;


	bool use_root_transform;
	ufbx_transform root_transform;


	ufbx_unicode_error_handling unicode_error_handling;


	bool retain_dom;

	uint32_t _end_zero;
} ufbx_load_opts;



typedef struct ufbx_evaluate_opts {
	uint32_t _begin_zero;

	ufbx_allocator_opts temp_allocator;
	ufbx_allocator_opts result_allocator;

	bool evaluate_skinning;
	bool evaluate_caches;


	bool load_external_files;


	ufbx_open_file_cb open_file_cb;

	uint32_t _end_zero;
} ufbx_evaluate_opts;



typedef struct ufbx_tessellate_curve_opts {
	uint32_t _begin_zero;

	ufbx_allocator_opts temp_allocator;
	ufbx_allocator_opts result_allocator;


	uint32_t span_subdivision;

	uint32_t _end_zero;
} ufbx_tessellate_curve_opts;



typedef struct ufbx_tessellate_surface_opts {
	uint32_t _begin_zero;

	ufbx_allocator_opts temp_allocator;
	ufbx_allocator_opts result_allocator;






	uint32_t span_subdivision_u;
	uint32_t span_subdivision_v;

	uint32_t _end_zero;
} ufbx_tessellate_surface_opts;



typedef struct ufbx_subdivide_opts {
	uint32_t _begin_zero;

	ufbx_allocator_opts temp_allocator;
	ufbx_allocator_opts result_allocator;

	ufbx_subdivision_boundary boundary;
	ufbx_subdivision_boundary uv_boundary;


	bool ignore_normals;



	bool interpolate_normals;


	bool interpolate_tangents;



	bool evaluate_source_vertices;


	size_t max_source_vertices;


	bool evaluate_skin_weights;


	size_t max_skin_weights;


	size_t skin_deformer_index;

	uint32_t _end_zero;
} ufbx_subdivide_opts;



typedef struct ufbx_geometry_cache_opts {
	uint32_t _begin_zero;

	ufbx_allocator_opts temp_allocator;
	ufbx_allocator_opts result_allocator;


	ufbx_open_file_cb open_file_cb;


	double frames_per_second;

	uint32_t _end_zero;
} ufbx_geometry_cache_opts;



typedef struct ufbx_geometry_cache_data_opts {
	uint32_t _begin_zero;


	ufbx_open_file_cb open_file_cb;

	bool additive;
	bool use_weight;
	ufbx_real weight;

	uint32_t _end_zero;
} ufbx_geometry_cache_data_opts;

typedef struct ufbx_panic {
	bool did_panic;
	size_t message_length;
	char message[UFBX_PANIC_MESSAGE_LENGTH];
} ufbx_panic;



#ifdef __cplusplus
extern "C" {
#endif


extern const ufbx_string ufbx_empty_string;
extern const ufbx_blob ufbx_empty_blob;
extern const ufbx_matrix ufbx_identity_matrix;
extern const ufbx_transform ufbx_identity_transform;
extern const ufbx_vec2 ufbx_zero_vec2;
extern const ufbx_vec3 ufbx_zero_vec3;
extern const ufbx_vec4 ufbx_zero_vec4;
extern const ufbx_quat ufbx_identity_quat;



extern const ufbx_coordinate_axes ufbx_axes_right_handed_y_up;
extern const ufbx_coordinate_axes ufbx_axes_right_handed_z_up;
extern const ufbx_coordinate_axes ufbx_axes_left_handed_y_up;
extern const ufbx_coordinate_axes ufbx_axes_left_handed_z_up;


extern const size_t ufbx_element_type_size[UFBX_ELEMENT_TYPE_COUNT];


extern const uint32_t ufbx_source_version;














ufbx_abi bool ufbx_is_thread_safe(void);


ufbx_abi ufbx_scene *ufbx_load_memory(
	const void *data, size_t data_size,
	const ufbx_load_opts *opts, ufbx_error *error);


ufbx_abi ufbx_scene *ufbx_load_file(
	const char *filename,
	const ufbx_load_opts *opts, ufbx_error *error);
ufbx_abi ufbx_scene *ufbx_load_file_len(
	const char *filename, size_t filename_len,
	const ufbx_load_opts *opts, ufbx_error *error);



ufbx_abi ufbx_scene *ufbx_load_stdio(
	void *file,
	const ufbx_load_opts *opts, ufbx_error *error);



ufbx_abi ufbx_scene *ufbx_load_stdio_prefix(
	void *file,
	const void *prefix, size_t prefix_size,
	const ufbx_load_opts *opts, ufbx_error *error);


ufbx_abi ufbx_scene *ufbx_load_stream(
	const ufbx_stream *stream,
	const ufbx_load_opts *opts, ufbx_error *error);


ufbx_abi ufbx_scene *ufbx_load_stream_prefix(
	const ufbx_stream *stream,
	const void *prefix, size_t prefix_size,
	const ufbx_load_opts *opts, ufbx_error *error);


ufbx_abi void ufbx_free_scene(ufbx_scene *scene);


ufbx_abi void ufbx_retain_scene(ufbx_scene *scene);




ufbx_abi size_t ufbx_format_error(char *dst, size_t dst_size, const ufbx_error *error);





ufbx_abi ufbx_prop *ufbx_find_prop_len(const ufbx_props *props, const char *name, size_t name_len);
ufbx_inline ufbx_prop *ufbx_find_prop(const ufbx_props *props, const char *name) { return ufbx_find_prop_len(props, name, strlen(name));}




ufbx_abi ufbx_real ufbx_find_real_len(const ufbx_props *props, const char *name, size_t name_len, ufbx_real def);
ufbx_inline ufbx_real ufbx_find_real(const ufbx_props *props, const char *name, ufbx_real def) { return ufbx_find_real_len(props, name, strlen(name), def); }
ufbx_abi ufbx_vec3 ufbx_find_vec3_len(const ufbx_props *props, const char *name, size_t name_len, ufbx_vec3 def);
ufbx_inline ufbx_vec3 ufbx_find_vec3(const ufbx_props *props, const char *name, ufbx_vec3 def) { return ufbx_find_vec3_len(props, name, strlen(name), def); }
ufbx_abi int64_t ufbx_find_int_len(const ufbx_props *props, const char *name, size_t name_len, int64_t def);
ufbx_inline int64_t ufbx_find_int(const ufbx_props *props, const char *name, int64_t def) { return ufbx_find_int_len(props, name, strlen(name), def); }
ufbx_abi bool ufbx_find_bool_len(const ufbx_props *props, const char *name, size_t name_len, bool def);
ufbx_inline bool ufbx_find_bool(const ufbx_props *props, const char *name, bool def) { return ufbx_find_bool_len(props, name, strlen(name), def); }
ufbx_abi ufbx_string ufbx_find_string_len(const ufbx_props *props, const char *name, size_t name_len, ufbx_string def);
ufbx_inline ufbx_string ufbx_find_string(const ufbx_props *props, const char *name, ufbx_string def) { return ufbx_find_string_len(props, name, strlen(name), def); }
ufbx_abi ufbx_blob ufbx_find_blob_len(const ufbx_props *props, const char *name, size_t name_len, ufbx_blob def);
ufbx_inline ufbx_blob ufbx_find_blob(const ufbx_props *props, const char *name, ufbx_blob def) { return ufbx_find_blob_len(props, name, strlen(name), def); }


ufbx_abi ufbx_element *ufbx_get_prop_element(const ufbx_element *element, const ufbx_prop *prop, ufbx_element_type type);




ufbx_abi ufbx_element *ufbx_find_element_len(const ufbx_scene *scene, ufbx_element_type type, const char *name, size_t name_len);
ufbx_inline ufbx_element *ufbx_find_element(const ufbx_scene *scene, ufbx_element_type type, const char *name) { return ufbx_find_element_len(scene, type, name, strlen(name)); }


ufbx_abi ufbx_node *ufbx_find_node_len(const ufbx_scene *scene, const char *name, size_t name_len);
ufbx_inline ufbx_node *ufbx_find_node(const ufbx_scene *scene, const char *name) { return ufbx_find_node_len(scene, name, strlen(name)); }


ufbx_abi ufbx_anim_stack *ufbx_find_anim_stack_len(const ufbx_scene *scene, const char *name, size_t name_len);
ufbx_inline ufbx_anim_stack *ufbx_find_anim_stack(const ufbx_scene *scene, const char *name) { return ufbx_find_anim_stack_len(scene, name, strlen(name)); }



ufbx_abi ufbx_anim_prop *ufbx_find_anim_prop_len(const ufbx_anim_layer *layer, const ufbx_element *element, const char *prop, size_t prop_len);
ufbx_inline ufbx_anim_prop *ufbx_find_anim_prop(const ufbx_anim_layer *layer, const ufbx_element *element, const char *prop) { return ufbx_find_anim_prop_len(layer, element, prop, strlen(prop)); }


ufbx_abi ufbx_anim_prop_list ufbx_find_anim_props(const ufbx_anim_layer *layer, const ufbx_element *element);





ufbx_abi ufbx_matrix ufbx_get_compatible_matrix_for_normals(const ufbx_node *node);







ufbx_abi ptrdiff_t ufbx_inflate(void *dst, size_t dst_size, const ufbx_inflate_input *input, ufbx_inflate_retain *retain);




ufbx_abi bool ufbx_open_file(void *user, ufbx_stream *stream, const char *path, size_t path_len);





ufbx_abi ufbx_real ufbx_evaluate_curve(const ufbx_anim_curve *curve, double time, ufbx_real default_value);


ufbx_abi ufbx_real ufbx_evaluate_anim_value_real(const ufbx_anim_value *anim_value, double time);
ufbx_abi ufbx_vec2 ufbx_evaluate_anim_value_vec2(const ufbx_anim_value *anim_value, double time);
ufbx_abi ufbx_vec3 ufbx_evaluate_anim_value_vec3(const ufbx_anim_value *anim_value, double time);



ufbx_abi ufbx_prop ufbx_evaluate_prop_len(const ufbx_anim *anim, const ufbx_element *element, const char *name, size_t name_len, double time);
ufbx_inline ufbx_prop ufbx_evaluate_prop(const ufbx_anim *anim, const ufbx_element *element, const char *name, double time) {
	return ufbx_evaluate_prop_len(anim, element, name, strlen(name), time);
}




ufbx_abi ufbx_props ufbx_evaluate_props(const ufbx_anim *anim, const ufbx_element *element, double time, ufbx_prop *buffer, size_t buffer_size);

ufbx_abi ufbx_transform ufbx_evaluate_transform(const ufbx_anim *anim, const ufbx_node *node, double time);
ufbx_abi ufbx_real ufbx_evaluate_blend_weight(const ufbx_anim *anim, const ufbx_blend_channel *channel, double time);

ufbx_abi ufbx_const_prop_override_list ufbx_prepare_prop_overrides(ufbx_prop_override *overrides, size_t num_overrides);








ufbx_abi ufbx_scene *ufbx_evaluate_scene(const ufbx_scene *scene, const ufbx_anim *anim, double time, const ufbx_evaluate_opts *opts, ufbx_error *error);



ufbx_abi ufbx_texture *ufbx_find_prop_texture_len(const ufbx_material *material, const char *name, size_t name_len);
ufbx_inline ufbx_texture *ufbx_find_prop_texture(const ufbx_material *material, const char *name) {
	return ufbx_find_prop_texture_len(material, name, strlen(name));
}

ufbx_abi ufbx_string ufbx_find_shader_prop_len(const ufbx_shader *shader, const char *name, size_t name_len);
ufbx_inline ufbx_string ufbx_find_shader_prop(const ufbx_shader *shader, const char *name) {
	return ufbx_find_shader_prop_len(shader, name, strlen(name));
}

ufbx_abi ufbx_shader_prop_binding_list ufbx_find_shader_prop_bindings_len(const ufbx_shader *shader, const char *name, size_t name_len);
ufbx_inline ufbx_shader_prop_binding_list ufbx_find_shader_prop_bindings(const ufbx_shader *shader, const char *name) {
	return ufbx_find_shader_prop_bindings_len(shader, name, strlen(name));
}

ufbx_abi ufbx_shader_texture_input *ufbx_find_shader_texture_input_len(const ufbx_shader_texture *shader, const char *name, size_t name_len);
ufbx_inline ufbx_shader_texture_input *ufbx_find_shader_texture_input(const ufbx_shader_texture *shader, const char *name) {
	return ufbx_find_shader_texture_input_len(shader, name, strlen(name));
}



ufbx_abi bool ufbx_coordinate_axes_valid(ufbx_coordinate_axes axes);

ufbx_abi ufbx_real ufbx_quat_dot(ufbx_quat a, ufbx_quat b);
ufbx_abi ufbx_quat ufbx_quat_mul(ufbx_quat a, ufbx_quat b);
ufbx_abi ufbx_quat ufbx_quat_normalize(ufbx_quat q);
ufbx_abi ufbx_quat ufbx_quat_fix_antipodal(ufbx_quat q, ufbx_quat reference);
ufbx_abi ufbx_quat ufbx_quat_slerp(ufbx_quat a, ufbx_quat b, ufbx_real t);
ufbx_abi ufbx_vec3 ufbx_quat_rotate_vec3(ufbx_quat q, ufbx_vec3 v);
ufbx_abi ufbx_vec3 ufbx_quat_to_euler(ufbx_quat q, ufbx_rotation_order order);
ufbx_abi ufbx_quat ufbx_euler_to_quat(ufbx_vec3 v, ufbx_rotation_order order);

ufbx_abi ufbx_matrix ufbx_matrix_mul(const ufbx_matrix *a, const ufbx_matrix *b);
ufbx_abi ufbx_real ufbx_matrix_determinant(const ufbx_matrix *m);
ufbx_abi ufbx_matrix ufbx_matrix_invert(const ufbx_matrix *m);
ufbx_abi ufbx_matrix ufbx_matrix_for_normals(const ufbx_matrix *m);
ufbx_abi ufbx_vec3 ufbx_transform_position(const ufbx_matrix *m, ufbx_vec3 v);
ufbx_abi ufbx_vec3 ufbx_transform_direction(const ufbx_matrix *m, ufbx_vec3 v);
ufbx_abi ufbx_matrix ufbx_transform_to_matrix(const ufbx_transform *t);
ufbx_abi ufbx_transform ufbx_matrix_to_transform(const ufbx_matrix *m);



ufbx_abi ufbx_matrix ufbx_catch_get_skin_vertex_matrix(ufbx_panic *panic, const ufbx_skin_deformer *skin, size_t vertex, const ufbx_matrix *fallback);
ufbx_inline ufbx_matrix ufbx_get_skin_vertex_matrix(const ufbx_skin_deformer *skin, size_t vertex, const ufbx_matrix *fallback) {
	return ufbx_catch_get_skin_vertex_matrix(NULL, skin, vertex, fallback);
}

ufbx_abi ufbx_vec3 ufbx_get_blend_shape_vertex_offset(const ufbx_blend_shape *shape, size_t vertex);
ufbx_abi ufbx_vec3 ufbx_get_blend_vertex_offset(const ufbx_blend_deformer *blend, size_t vertex);

ufbx_abi void ufbx_add_blend_shape_vertex_offsets(const ufbx_blend_shape *shape, ufbx_vec3 *vertices, size_t num_vertices, ufbx_real weight);
ufbx_abi void ufbx_add_blend_vertex_offsets(const ufbx_blend_deformer *blend, ufbx_vec3 *vertices, size_t num_vertices, ufbx_real weight);



ufbx_abi size_t ufbx_evaluate_nurbs_basis(const ufbx_nurbs_basis *basis, ufbx_real u, ufbx_real *weights, size_t num_weights, ufbx_real *derivatives, size_t num_derivatives);

ufbx_abi ufbx_curve_point ufbx_evaluate_nurbs_curve(const ufbx_nurbs_curve *curve, ufbx_real u);
ufbx_abi ufbx_surface_point ufbx_evaluate_nurbs_surface(const ufbx_nurbs_surface *surface, ufbx_real u, ufbx_real v);

ufbx_abi ufbx_line_curve *ufbx_tessellate_nurbs_curve(const ufbx_nurbs_curve *curve, const ufbx_tessellate_curve_opts *opts, ufbx_error *error);
ufbx_abi ufbx_mesh *ufbx_tessellate_nurbs_surface(const ufbx_nurbs_surface *surface, const ufbx_tessellate_surface_opts *opts, ufbx_error *error);

ufbx_abi void ufbx_free_line_curve(ufbx_line_curve *curve);
ufbx_abi void ufbx_retain_line_curve(ufbx_line_curve *curve);



ufbx_abi uint32_t ufbx_catch_triangulate_face(ufbx_panic *panic, uint32_t *indices, size_t num_indices, const ufbx_mesh *mesh, ufbx_face face);
ufbx_inline uint32_t ufbx_triangulate_face(uint32_t *indices, size_t num_indices, const ufbx_mesh *mesh, ufbx_face face) {
	return ufbx_catch_triangulate_face(NULL, indices, num_indices, mesh, face);
}


ufbx_abi void ufbx_catch_compute_topology(ufbx_panic *panic, const ufbx_mesh *mesh, ufbx_topo_edge *topo, size_t num_topo);
ufbx_inline void ufbx_compute_topology(const ufbx_mesh *mesh, ufbx_topo_edge *topo, size_t num_topo) {
	ufbx_catch_compute_topology(NULL, mesh, topo, num_topo);
}




ufbx_abi uint32_t ufbx_catch_topo_next_vertex_edge(ufbx_panic *panic, const ufbx_topo_edge *topo, size_t num_topo, uint32_t index);
ufbx_inline uint32_t ufbx_topo_next_vertex_edge(const ufbx_topo_edge *topo, size_t num_topo, uint32_t index) {
	return ufbx_catch_topo_next_vertex_edge(NULL, topo, num_topo, index);
}

ufbx_abi uint32_t ufbx_catch_topo_prev_vertex_edge(ufbx_panic *panic, const ufbx_topo_edge *topo, size_t num_topo, uint32_t index);
ufbx_inline uint32_t ufbx_topo_prev_vertex_edge(const ufbx_topo_edge *topo, size_t num_topo, uint32_t index) {
	return ufbx_catch_topo_prev_vertex_edge(NULL, topo, num_topo, index);
}

ufbx_abi ufbx_vec3 ufbx_catch_get_weighted_face_normal(ufbx_panic *panic, const ufbx_vertex_vec3 *positions, ufbx_face face);
ufbx_inline ufbx_vec3 ufbx_get_weighted_face_normal(const ufbx_vertex_vec3 *positions, ufbx_face face) {
	return ufbx_catch_get_weighted_face_normal(NULL, positions, face);
}

ufbx_abi size_t ufbx_catch_generate_normal_mapping(ufbx_panic *panic, const ufbx_mesh *mesh,
	const ufbx_topo_edge *topo, size_t num_topo,
	uint32_t *normal_indices, size_t num_normal_indices, bool assume_smooth);
ufbx_abi size_t ufbx_generate_normal_mapping(const ufbx_mesh *mesh,
	const ufbx_topo_edge *topo, size_t num_topo,
	uint32_t *normal_indices, size_t num_normal_indices, bool assume_smooth);

ufbx_abi void ufbx_catch_compute_normals(ufbx_panic *panic, const ufbx_mesh *mesh, const ufbx_vertex_vec3 *positions,
	const uint32_t *normal_indices, size_t num_normal_indices,
	ufbx_vec3 *normals, size_t num_normals);
ufbx_abi void ufbx_compute_normals(const ufbx_mesh *mesh, const ufbx_vertex_vec3 *positions,
	const uint32_t *normal_indices, size_t num_normal_indices,
	ufbx_vec3 *normals, size_t num_normals);

ufbx_abi ufbx_mesh *ufbx_subdivide_mesh(const ufbx_mesh *mesh, size_t level, const ufbx_subdivide_opts *opts, ufbx_error *error);

ufbx_abi void ufbx_free_mesh(ufbx_mesh *mesh);
ufbx_abi void ufbx_retain_mesh(ufbx_mesh *mesh);



ufbx_abi ufbx_geometry_cache *ufbx_load_geometry_cache(
	const char *filename,
	const ufbx_geometry_cache_opts *opts, ufbx_error *error);
ufbx_abi ufbx_geometry_cache *ufbx_load_geometry_cache_len(
	const char *filename, size_t filename_len,
	const ufbx_geometry_cache_opts *opts, ufbx_error *error);

ufbx_abi void ufbx_free_geometry_cache(ufbx_geometry_cache *cache);
ufbx_abi void ufbx_retain_geometry_cache(ufbx_geometry_cache *cache);

ufbx_abi size_t ufbx_get_read_geometry_cache_real_num_data(const ufbx_cache_frame *frame);
ufbx_abi size_t ufbx_get_sample_geometry_cache_real_num_data(const ufbx_cache_channel *channel, double time);
ufbx_abi size_t ufbx_get_read_geometry_cache_vec3_num_data(const ufbx_cache_frame *frame);
ufbx_abi size_t ufbx_get_sample_geometry_cache_vec3_num_data(const ufbx_cache_channel *channel, double time);

ufbx_abi size_t ufbx_read_geometry_cache_real(const ufbx_cache_frame *frame, ufbx_real *data, size_t num_data, const ufbx_geometry_cache_data_opts *opts);
ufbx_abi size_t ufbx_sample_geometry_cache_real(const ufbx_cache_channel *channel, double time, ufbx_real *data, size_t num_data, const ufbx_geometry_cache_data_opts *opts);
ufbx_abi size_t ufbx_read_geometry_cache_vec3(const ufbx_cache_frame *frame, ufbx_vec3 *data, size_t num_data, const ufbx_geometry_cache_data_opts *opts);
ufbx_abi size_t ufbx_sample_geometry_cache_vec3(const ufbx_cache_channel *channel, double time, ufbx_vec3 *data, size_t num_data, const ufbx_geometry_cache_data_opts *opts);



ufbx_abi ufbx_dom_node *ufbx_dom_find_len(const ufbx_dom_node *parent, const char *name, size_t name_len);
ufbx_inline ufbx_dom_node *ufbx_dom_find(const ufbx_dom_node *parent, const char *name) { return ufbx_dom_find_len(parent, name, strlen(name)); }



ufbx_abi size_t ufbx_generate_indices(const ufbx_vertex_stream *streams, size_t num_streams, uint32_t *indices, size_t num_indices, const ufbx_allocator_opts *allocator, ufbx_error *error);



ufbx_abi ufbx_real ufbx_catch_get_vertex_real(ufbx_panic *panic, const ufbx_vertex_real *v, size_t index);
ufbx_abi ufbx_vec2 ufbx_catch_get_vertex_vec2(ufbx_panic *panic, const ufbx_vertex_vec2 *v, size_t index);
ufbx_abi ufbx_vec3 ufbx_catch_get_vertex_vec3(ufbx_panic *panic, const ufbx_vertex_vec3 *v, size_t index);
ufbx_abi ufbx_vec4 ufbx_catch_get_vertex_vec4(ufbx_panic *panic, const ufbx_vertex_vec4 *v, size_t index);

ufbx_inline ufbx_real ufbx_get_vertex_real(const ufbx_vertex_real *v, size_t index) { ufbx_assert(index < v->indices.count); return v->values.data[(int32_t)v->indices.data[index]]; }
ufbx_inline ufbx_vec2 ufbx_get_vertex_vec2(const ufbx_vertex_vec2 *v, size_t index) { ufbx_assert(index < v->indices.count); return v->values.data[(int32_t)v->indices.data[index]]; }
ufbx_inline ufbx_vec3 ufbx_get_vertex_vec3(const ufbx_vertex_vec3 *v, size_t index) { ufbx_assert(index < v->indices.count); return v->values.data[(int32_t)v->indices.data[index]]; }
ufbx_inline ufbx_vec4 ufbx_get_vertex_vec4(const ufbx_vertex_vec4 *v, size_t index) { ufbx_assert(index < v->indices.count); return v->values.data[(int32_t)v->indices.data[index]]; }

ufbx_abi size_t ufbx_get_triangulate_face_num_indices(ufbx_face face);

ufbx_abi ufbx_unknown *ufbx_as_unknown(const ufbx_element *element);
ufbx_abi ufbx_node *ufbx_as_node(const ufbx_element *element);
ufbx_abi ufbx_mesh *ufbx_as_mesh(const ufbx_element *element);
ufbx_abi ufbx_light *ufbx_as_light(const ufbx_element *element);
ufbx_abi ufbx_camera *ufbx_as_camera(const ufbx_element *element);
ufbx_abi ufbx_bone *ufbx_as_bone(const ufbx_element *element);
ufbx_abi ufbx_empty *ufbx_as_empty(const ufbx_element *element);
ufbx_abi ufbx_line_curve *ufbx_as_line_curve(const ufbx_element *element);
ufbx_abi ufbx_nurbs_curve *ufbx_as_nurbs_curve(const ufbx_element *element);
ufbx_abi ufbx_nurbs_surface *ufbx_as_nurbs_surface(const ufbx_element *element);
ufbx_abi ufbx_nurbs_trim_surface *ufbx_as_nurbs_trim_surface(const ufbx_element *element);
ufbx_abi ufbx_nurbs_trim_boundary *ufbx_as_nurbs_trim_boundary(const ufbx_element *element);
ufbx_abi ufbx_procedural_geometry *ufbx_as_procedural_geometry(const ufbx_element *element);
ufbx_abi ufbx_stereo_camera *ufbx_as_stereo_camera(const ufbx_element *element);
ufbx_abi ufbx_camera_switcher *ufbx_as_camera_switcher(const ufbx_element *element);
ufbx_abi ufbx_marker *ufbx_as_marker(const ufbx_element *element);
ufbx_abi ufbx_lod_group *ufbx_as_lod_group(const ufbx_element *element);
ufbx_abi ufbx_skin_deformer *ufbx_as_skin_deformer(const ufbx_element *element);
ufbx_abi ufbx_skin_cluster *ufbx_as_skin_cluster(const ufbx_element *element);
ufbx_abi ufbx_blend_deformer *ufbx_as_blend_deformer(const ufbx_element *element);
ufbx_abi ufbx_blend_channel *ufbx_as_blend_channel(const ufbx_element *element);
ufbx_abi ufbx_blend_shape *ufbx_as_blend_shape(const ufbx_element *element);
ufbx_abi ufbx_cache_deformer *ufbx_as_cache_deformer(const ufbx_element *element);
ufbx_abi ufbx_cache_file *ufbx_as_cache_file(const ufbx_element *element);
ufbx_abi ufbx_material *ufbx_as_material(const ufbx_element *element);
ufbx_abi ufbx_texture *ufbx_as_texture(const ufbx_element *element);
ufbx_abi ufbx_video *ufbx_as_video(const ufbx_element *element);
ufbx_abi ufbx_shader *ufbx_as_shader(const ufbx_element *element);
ufbx_abi ufbx_shader_binding *ufbx_as_shader_binding(const ufbx_element *element);
ufbx_abi ufbx_anim_stack *ufbx_as_anim_stack(const ufbx_element *element);
ufbx_abi ufbx_anim_layer *ufbx_as_anim_layer(const ufbx_element *element);
ufbx_abi ufbx_anim_value *ufbx_as_anim_value(const ufbx_element *element);
ufbx_abi ufbx_anim_curve *ufbx_as_anim_curve(const ufbx_element *element);
ufbx_abi ufbx_display_layer *ufbx_as_display_layer(const ufbx_element *element);
ufbx_abi ufbx_selection_set *ufbx_as_selection_set(const ufbx_element *element);
ufbx_abi ufbx_selection_node *ufbx_as_selection_node(const ufbx_element *element);
ufbx_abi ufbx_character *ufbx_as_character(const ufbx_element *element);
ufbx_abi ufbx_constraint *ufbx_as_constraint(const ufbx_element *element);
ufbx_abi ufbx_pose *ufbx_as_pose(const ufbx_element *element);
ufbx_abi ufbx_metadata_object *ufbx_as_metadata_object(const ufbx_element *element);



ufbx_abi void ufbx_ffi_find_int_len(int64_t *retval, const ufbx_props *props, const char *name, size_t name_len, const int64_t *def);
ufbx_abi void ufbx_ffi_find_vec3_len(ufbx_vec3 *retval, const ufbx_props *props, const char *name, size_t name_len, const ufbx_vec3 *def);
ufbx_abi void ufbx_ffi_find_string_len(ufbx_string *retval, const ufbx_props *props, const char *name, size_t name_len, const ufbx_string *def);
ufbx_abi void ufbx_ffi_find_anim_props(ufbx_anim_prop_list *retval, const ufbx_anim_layer *layer, const ufbx_element *element);
ufbx_abi void ufbx_ffi_get_compatible_matrix_for_normals(ufbx_matrix *retval, const ufbx_node *node);
ufbx_abi void ufbx_ffi_evaluate_anim_value_vec2(ufbx_vec2 *retval, const ufbx_anim_value *anim_value, double time);
ufbx_abi void ufbx_ffi_evaluate_anim_value_vec3(ufbx_vec3 *retval, const ufbx_anim_value *anim_value, double time);
ufbx_abi void ufbx_ffi_evaluate_prop_len(ufbx_prop *retval, const ufbx_anim *anim, const ufbx_element *element, const char *name, size_t name_len, double time);
ufbx_abi void ufbx_ffi_evaluate_props(ufbx_props *retval, const ufbx_anim *anim, ufbx_element *element, double time, ufbx_prop *buffer, size_t buffer_size);
ufbx_abi void ufbx_ffi_evaluate_transform(ufbx_transform *retval, const ufbx_anim *anim, const ufbx_node *node, double time);
ufbx_abi ufbx_real ufbx_ffi_evaluate_blend_weight(const ufbx_anim *anim, const ufbx_blend_channel *channel, double time);
ufbx_abi void ufbx_ffi_prepare_prop_overrides(ufbx_const_prop_override_list *retval, ufbx_prop_override *overrides, size_t num_overrides);
ufbx_abi void ufbx_ffi_quat_mul(ufbx_quat *retval, const ufbx_quat *a, const ufbx_quat *b);
ufbx_abi void ufbx_ffi_quat_normalize(ufbx_quat *retval, const ufbx_quat *q);
ufbx_abi void ufbx_ffi_quat_fix_antipodal(ufbx_quat *retval, const ufbx_quat *q, const ufbx_quat *reference);
ufbx_abi void ufbx_ffi_quat_slerp(ufbx_quat *retval, const ufbx_quat *a, const ufbx_quat *b, ufbx_real t);
ufbx_abi void ufbx_ffi_quat_rotate_vec3(ufbx_vec3 *retval, const ufbx_quat *q, const ufbx_vec3 *v);
ufbx_abi void ufbx_ffi_quat_to_euler(ufbx_vec3 *retval, const ufbx_quat *q, ufbx_rotation_order order);
ufbx_abi void ufbx_ffi_euler_to_quat(ufbx_quat *retval, const ufbx_vec3 *v, ufbx_rotation_order order);
ufbx_abi void ufbx_ffi_matrix_mul(ufbx_matrix *retval, const ufbx_matrix *a, const ufbx_matrix *b);
ufbx_abi void ufbx_ffi_matrix_invert(ufbx_matrix *retval, const ufbx_matrix *m);
ufbx_abi void ufbx_ffi_matrix_for_normals(ufbx_matrix *retval, const ufbx_matrix *m);
ufbx_abi void ufbx_ffi_transform_position(ufbx_vec3 *retval, const ufbx_matrix *m, const ufbx_vec3 *v);
ufbx_abi void ufbx_ffi_transform_direction(ufbx_vec3 *retval, const ufbx_matrix *m, const ufbx_vec3 *v);
ufbx_abi void ufbx_ffi_transform_to_matrix(ufbx_matrix *retval, const ufbx_transform *t);
ufbx_abi void ufbx_ffi_matrix_to_transform(ufbx_transform *retval, const ufbx_matrix *m);
ufbx_abi void ufbx_ffi_get_skin_vertex_matrix(ufbx_matrix *retval, const ufbx_skin_deformer *skin, size_t vertex, const ufbx_matrix *fallback);
ufbx_abi void ufbx_ffi_get_blend_shape_vertex_offset(ufbx_vec3 *retval, const ufbx_blend_shape *shape, size_t vertex);
ufbx_abi void ufbx_ffi_get_blend_vertex_offset(ufbx_vec3 *retval, const ufbx_blend_deformer *blend, size_t vertex);
ufbx_abi void ufbx_ffi_evaluate_nurbs_curve(ufbx_curve_point *retval, const ufbx_nurbs_curve *curve, ufbx_real u);
ufbx_abi void ufbx_ffi_evaluate_nurbs_surface(ufbx_surface_point *retval, const ufbx_nurbs_surface *surface, ufbx_real u, ufbx_real v);
ufbx_abi void ufbx_ffi_get_weighted_face_normal(ufbx_vec3 *retval, const ufbx_vertex_vec3 *positions, const ufbx_face *face);
ufbx_abi size_t ufbx_ffi_get_triangulate_face_num_indices(const ufbx_face *face);
ufbx_abi uint32_t ufbx_ffi_triangulate_face(uint32_t *indices, size_t num_indices, const ufbx_mesh *mesh, const ufbx_face *face);

ufbx_inline size_t ufbx_check_index(size_t index, size_t count) {
	(void)count;
	ufbx_assert(index < count);
	return index;
}

#define ufbx_at(list, index) ((list).data[ufbx_check_index((index), (list).count)])

#ifdef __cplusplus
}
#endif

#if defined(_MSC_VER)
	#pragma warning(pop)
#endif

#endif

