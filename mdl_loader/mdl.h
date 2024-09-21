#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "fileutil.h"

#define STUDIOMDL_ID (('T'<<24)|('S'<<16)|('D'<<8)|('I'))

typedef struct {
	int id;
	int version;
	char name[64];
	int length;
} mdl_header_t;

typedef struct {
	float x, y, z;
} vec3_t;

typedef struct {
	int num_tris;
	int tri_offset;
} mdl_mesh_t;

typedef struct {
	int indices[3];
} mdl_triangle_t;

typedef struct {
	int type;
	float values[4];
} mdl_bone_t;

typedef struct {
	int num_vertices;
	int vertex_offset;
	int normal_offset;
	int uv_offset;
} mdl_vertex_buffer_t;

typedef struct {
	vec3_t position;
	vec3_t rotation;
	vec3_t mins;
	vec3_t maxs;
	int num_bones;
	int bone_index;
	int num_meshes;
	int mesh_offset;
	int num_verts;
	int vert_offset;
	int num_triangles;
	int tri_offset;
	int num_uv;
	int uv_offset;
	mdl_bone_t *p_bones;
	mdl_mesh_t *p_meshes;
	vec3_t *vertices;
	vec3_t *normals;
	vec3_t *uvs;
	mdl_triangle_t *triangles;
	uint8_t *p_base;
} mdl_model_t;

mdl_model_t *mdl_load(int *p_dst_status, const char *p_filename);