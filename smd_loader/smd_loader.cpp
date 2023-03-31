#include "smd_loader.h"

#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#ifdef _DEBUG
#include <stdio.h>
#define SMD_DBG(text, ...) printf(text "\n", __VA_ARGS__)
#else
#define SMD_DBG(text, ...)
#endif

#define IS_EMPTY_LINE(line) (!line[0] || line[0] == '\n')

#define cstrcmp(str, cstr) (strncmp(str, cstr, sizeof(cstr) - 1))

bool smd_next_token(char **p_dst_tok_addr, char **p_tokptr, int token)
{
	// no more tokens
	if (!(*p_tokptr))
		return false;

	*p_dst_tok_addr = *p_tokptr; //save previous token address
	(*p_tokptr) = strchr(*p_tokptr, token);
	if (!(*p_tokptr))
		return true;
	
	*(*p_tokptr) = '\0'; // null-terminate token
	(*p_tokptr)++; // increment pointer from '\0'
	return true;
}

bool smd_next_token_int(int *p_dst_int, char **p_tokptr, int delim)
{
	char *p_token;
	if (!smd_next_token(&p_token, p_tokptr, delim))
		return false;

	*p_dst_int = atoi(p_token);
	return true;
}

bool smd_next_token_float(float *p_dst_float, char **p_tokptr, int delim)
{
	char *p_token;
	if (!smd_next_token(&p_token, p_tokptr, delim))
		return false;

	*p_dst_float = atof(p_token);
	return true;
}

bool smd_next_token_vector(smd_vec3 *p_dst_vector, char **p_tokptr, int delim)
{
	return smd_next_token_float(&p_dst_vector->x, p_tokptr, delim) &&
		smd_next_token_float(&p_dst_vector->y, p_tokptr, delim) &&
		smd_next_token_float(&p_dst_vector->z, p_tokptr, delim);
}

/*
 count frames in smd

 returns: -1 if skeleton block not found, otherwise returns frames count
*/
int smd_frames_count(FILE *fp)
{
	char line[1024];
	int num_kframes = -1;
	fpos_t previous_position;
	fgetpos(fp, &previous_position);
	while (!feof(fp) && !ferror(fp)) {
		fgets(line, sizeof(line), fp);
		if (IS_EMPTY_LINE(line))
			continue;

		if (!cstrcmp(line, "skeleton")) {
			num_kframes = 0;
			while (!feof(fp) && !ferror(fp) && cstrcmp(line, "end") != 0) {
				fgets(line, sizeof(line), fp);
				if (IS_EMPTY_LINE(line))
					continue;

				if (!cstrcmp(line, "time"))
					num_kframes++;
			}
		}
	}
	fsetpos(fp, &previous_position);
	SMD_DBG(__PRETTY_FUNCTION__ " : keyframes in smd (%d)", num_kframes);
	return num_kframes;
}

int smd_load_model(smd_model *p_dst_smdmodel, const char *p_smdpath)
{
	int num_frames;
	int smd_version;
	char line[1024];
	FILE *fp;

	fopen_s(&fp, p_smdpath, "rt");
	if (fp) {
		smd_clear_model(p_dst_smdmodel); //clear old data (if exists)

		/* skip empty lines in start of file */
		while (!feof(fp) && !ferror(fp)) {
			fgets(line, sizeof(line), fp);
			if (!IS_EMPTY_LINE(line))
				break;
		}

		/* version must be first line in file! */
		if (sscanf_s(line, "version %d", &smd_version) == 1) {
			if (smd_version != 1) {
				fclose(fp);
				return SMDLDR_STATUS_UNSUPPORTED_VERSION;
			}

			/* count frames in smd */
			num_frames = smd_frames_count(fp);
			if (num_frames == -1) {
				fclose(fp);
				return SMDLDR_STATUS_INCORRECT_INFO; // skeleton block has not found
			}

			/* find other data */
			while (!feof(fp) && !ferror(fp)) {
				fgets(line, sizeof(line), fp);
				if (IS_EMPTY_LINE(line))
					continue;

				/* find skeleton hierarchy */
				if (!cstrcmp(line, "nodes")) {
					SMD_DBG("Loader: Enter 'nodes' section");
					smd_hbone bone;
					int bone_number;
					while (!feof(fp) && !ferror(fp)) {
						fgets(line, sizeof(line), fp);
						if (IS_EMPTY_LINE(line))
							continue;

						if (!cstrcmp(line, "end"))
							goto __exit_nodes_read_cycle;

						// 0 "ValveBiped.Bip01_Pelvis" -1
						if (sscanf(line, " %d %64s %d", &bone_number, bone.name, &bone.parent_id) != 3) {
							fclose(fp);
							return SMDLDR_STATUS_INVALID_LINE;
						}
						p_dst_smdmodel->hierarchy.push_back(bone); // save skeleton bone
					}

				__exit_nodes_read_cycle:
					SMD_DBG("Loader: Leave 'nodes' section. Loaded hierarchy for %d bones", p_dst_smdmodel->hierarchy.size());
					continue; // continue main loop
				}

				/* load keyframes */
				if (!cstrcmp(line, "skeleton")) {
					int current_frame;
					SMD_DBG("Loader: Enter 'skeleton' section");
					while (!feof(fp) && !ferror(fp)) {
						fgets(line, sizeof(line), fp);
						if (IS_EMPTY_LINE(line))
							continue;

						if (!cstrcmp(line, "end"))
							goto __exit_skeleton_load;

						/* find keyframe */
						if (!cstrcmp(line, "time")) {
							if (sscanf_s(line, "time %d", &current_frame) == 1) {
								smd_anim_keyframe keyframe;
								SMD_DBG("Loader: Enter 'time %d' section", current_frame);

								/* check current keyframe index */
								if (current_frame < 0 || current_frame > num_frames) {
									fclose(fp);
									return SMDLDR_STATUS_INCORRECT_KEYFRAME_INDEX;
								}

								/* read keyframe data */
								int bone_idx = 0;
								int file_bone_idx;
								while (!feof(fp) && !ferror(fp) && (bone_idx < p_dst_smdmodel->hierarchy.size())) {
									fgets(line, sizeof(line), fp);
									if (IS_EMPTY_LINE(line))
										continue;

									/* fill bone from keyframe line data */
									smd_kfbone bone;
									if (sscanf_s(line, " %d %f %f %f %f %f %f", &file_bone_idx,
										&bone.position.x, &bone.position.y, &bone.position.z,
										&bone.rotation.x, &bone.rotation.y, &bone.rotation.z) != 7) {

										fclose(fp);
										return SMDLDR_STATUS_INCORRECT_KEYFRAME_BONE_DATA;
									}
									keyframe.frames.push_back(bone); // add bone to keyframe
									bone_idx++;
								}
								p_dst_smdmodel->keyframes.push_back(keyframe); // add keyframe
							}
						}
					}

				__exit_skeleton_load:
					SMD_DBG("Loader: Leave 'skeleton' section. Loaded %d keyframes", p_dst_smdmodel->keyframes.size());
					continue; // continue main loop
				}

				/* load triangles */
				if (!cstrcmp(line, "triangles")) {
					char *p_tokptr;
					char previous_material[1024];
					previous_material[0] = 0;
					smd_mesh *p_current_mesh = NULL;
					SMD_DBG("Loader: Enter 'triangles' section");
					line[0] = '\0';
					while (!feof(fp) && !ferror(fp)) {
						/* read material */
						// <string|material>
						if (fgets(line, sizeof(line), fp) == NULL) {
							fclose(fp);
							return SMDLDR_STATUS_UNEXPECTED_END_OF_FILE;
						}

						if (IS_EMPTY_LINE(line))
							continue;

						if (!cstrcmp(line, "end"))
							goto __leave_triangles;

						/* check beginning new mesh group */
						if (strcmp(previous_material, line) != 0) {
							SMD_DBG("Loader: new group: %s", line);
							p_dst_smdmodel->meshes.resize(p_dst_smdmodel->meshes.size() + 1);
							p_current_mesh = &p_dst_smdmodel->meshes[p_dst_smdmodel->meshes.size() - 1];
							strcpy_s(p_current_mesh->name, line);
						}

						//TODO: 
						// короче идея состоит в том, чтобы добавить цикл чтения строк продолжающийся до тех пор, пока текущий материал будет отличаться от материала на считанной строке
						// как дочитали, ниже цикла который идет до end, добавляем этот меш
						// 

						/* copy current material */
						strcpy_s(previous_material, sizeof(previous_material), line);
						for (int k = 0; k < 3; k++) {
							/* read vertex data */
							// <int|Parent bone> <float|PosX PosY PosZ> <normal|NormX NormY NormZ> <float|U V> <int|links> <int|Bone ID> <float|Weight> [...]
							if (fgets(line, sizeof(line), fp) == NULL) {
								fclose(fp);
								return SMDLDR_STATUS_UNEXPECTED_END_OF_FILE;
							}

							if (IS_EMPTY_LINE(line))
								continue;

							/* check "end" tag */
							if (!cstrcmp(line, "end"))
								goto __leave_triangles;

							p_tokptr = (char *)line;

							int parent;
							smd_vertex vert;
							if (!(smd_next_token_int(&parent, &p_tokptr, ' ') &&
								smd_next_token_vector(&vert.vertex, &p_tokptr, ' ') &&
								smd_next_token_vector(&vert.normal, &p_tokptr, ' ') &&
								smd_next_token_float(&vert.uv.u, &p_tokptr, ' ') &&
								smd_next_token_float(&vert.uv.v, &p_tokptr, ' '))) {

								fclose(fp);
								return SMDLDR_STATUS_INVALID_LINE;
							}

							/* if weights exists */
							if (smd_next_token_int(&vert.num_weights, &p_tokptr, ' ')) {

								/* check weights count */
								if (vert.num_weights > MAX_WEIGHTS) {
									fclose(fp);
									return SMDLDR_STATUS_WEIGHTS_LIMIT_EXCEEDED;
								}

								/* read weights info from line tokens */
								for (int i = 0; i < vert.num_weights; i++) {
									if (!(smd_next_token_int(&vert.weights[i].boneid, &p_tokptr, ' ') &&
										smd_next_token_float(&vert.weights[i].weight, &p_tokptr, ' '))) {
										return SMDLDR_STATUS_INVALID_LINE;
									}
								}
							}
							p_current_mesh->vertices.push_back(vert);
						}
					}

				__leave_triangles:
#ifdef _DEBUG
					SMD_DBG("Loader: Leave 'triangles' section. Loaded %d meshes", p_dst_smdmodel->meshes.size());
					for (int j = 0; j < p_dst_smdmodel->meshes.size(); j++) {
						smd_mesh *p_currmesh = &p_dst_smdmodel->meshes[j];
						SMD_DBG("Mesh material: %s  (Verts: %d)", p_currmesh->name, p_currmesh->vertices.size());
					}
#endif
					continue; // continue main loop
				}
			}
			return SMDLDR_STATUS_OK;
		}
		return SMDLDR_STATUS_VERSION_NOT_FOUND; // version is not found in start of file
	}
	return SMDLDR_STATUS_FILE_ERROR;
}

void smd_clear_model(smd_model *p_src_model)
{
	p_src_model->hierarchy.clear();
	p_src_model->keyframes.clear();
	p_src_model->meshes.clear();
}
