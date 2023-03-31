#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <vector>

#define MAX_WEIGHTS 4

struct smd_vec3 {
	float x, y, z;
};

struct smd_vec2 {
	float u, v;
};

/* keyframe bone */
struct smd_kfbone {
	smd_kfbone() {
		parent_id = -1;
		position = { 0.f, 0.f, 0.f };
		rotation = { 0.f, 0.f, 0.f };
	}
	~smd_kfbone() {}

	int parent_id;
	smd_vec3 position;
	smd_vec3 rotation;
};

/* bone hierarchy */
struct smd_hbone {
	int parent_id;
	char name[64];

	smd_hbone() { name[0] = 0; }
	~smd_hbone() {}
};

struct smd_triangle {
	union {
		struct { unsigned int v0, v1, v2; };
		unsigned int indices[3];
	};
};

struct smd_weight {
	int boneid;
	float weight;
};

struct smd_vertex {
	smd_vec3 vertex;
	smd_vec3 normal;
	smd_vec2 uv;

	int num_weights;
	smd_weight weights[MAX_WEIGHTS];
};

struct smd_mesh {
	char name[64];
	char texture[64];
	std::vector<smd_vertex> vertices;
	std::vector<smd_triangle> triangles;
};

struct smd_anim_keyframe {
	std::vector<smd_kfbone> frames;
};

struct smd_model {
	char name[64];
	std::vector<smd_hbone> hierarchy;
	std::vector<smd_anim_keyframe> keyframes;
	std::vector<smd_mesh> meshes;
};

enum SMD_LOADER_STATUS {
	SMDLDR_STATUS_OK = 0,
	SMDLDR_STATUS_FILE_ERROR,
	SMDLDR_STATUS_VERSION_NOT_FOUND,
	SMDLDR_STATUS_INVALID_LINE,
	SMDLDR_STATUS_UNSUPPORTED_VERSION,
	SMDLDR_STATUS_INCORRECT_INFO,
	SMDLDR_STATUS_INCORRECT_KEYFRAME_INDEX,
	SMDLDR_STATUS_INCORRECT_KEYFRAME_BONE_DATA,
	SMDLDR_STATUS_UNEXPECTED_END_OF_FILE,
	SMDLDR_STATUS_WEIGHTS_LIMIT_EXCEEDED
};

int smd_load_model(smd_model *p_dst_smdmodel, const char *p_smdpath);
void smd_clear_model(smd_model *p_src_model);

