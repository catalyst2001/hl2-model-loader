#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

#define MAX_WEIGHTS 4

struct smd_vec3 {
	float x, y, z;

	smd_vec3() {}
	smd_vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

	void flip_zy() {
		float temp = z;
		z = y;
		y = temp;
	}

	bool operator==(smd_vec3 &_cmp_vec_ref) { return x == _cmp_vec_ref.x && y == _cmp_vec_ref.y && z == _cmp_vec_ref.z; }
	smd_vec3 operator+(smd_vec3 &vec) { return smd_vec3(x + vec.x, y + vec.y, z + vec.z); }
	smd_vec3 &operator+=(smd_vec3 &vec) {
		x += vec.x; y += vec.y; z += vec.z;
		return *this;
	}
	smd_vec3 &operator-=(smd_vec3 &vec) {
		x -= vec.x; y -= vec.y; z -= vec.z;
		return *this;
	}
};

struct smd_vec2 {
	float u, v;

	bool operator==(smd_vec2 &_cmp_vec_ref) { return u == _cmp_vec_ref.u && v == _cmp_vec_ref.v; }
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
	glm::vec3 position;
	glm::vec3 rotation;
	glm::mat4x4 matrix;
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
	glm::vec3 vertex;
	glm::vec3 normal;
	glm::vec2 uv;

	int num_weights;
	smd_weight weights[MAX_WEIGHTS];

	bool operator==(smd_vertex &_cmp_vert_ref) {
		return _cmp_vert_ref.vertex == vertex && _cmp_vert_ref.normal == normal && _cmp_vert_ref.uv == uv;
	}
};

struct smd_mesh {
	char name[64];
	char texture[64];
	std::vector<smd_vertex> vertices;
	std::vector<unsigned int> triangles;
};

struct smd_anim_keyframe {
	std::vector<smd_kfbone> positions;
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

/* LOAD MODEL FROM FILE */
int smd_load_model(smd_model *p_dst_smdmodel, const char *p_smdpath);

/* CURRENT OPENED SMD IS ANIM FILE? */
bool smd_model_is_animation(smd_model *p_src_smdmodel);

/* REMOVE ALL DATA FROM MODEL */
void smd_clear_model(smd_model *p_src_model);

