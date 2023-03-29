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

/*
 count frames in smd

 returns: -1 if skeleton block not found, otherwise returns frames count
*/
int smd_frames_count(FILE *fp)
{
	SMD_DBG(__PRETTY_FUNCTION__ " call");
	char line[1024];
	int num_kframes = -1;
	fpos_t previous_position;
	fgetpos(fp, &previous_position);
	while (!feof(fp) && !ferror(fp)) {
		if (IS_EMPTY_LINE(line))
			continue;

		if (!cstrcmp(line, "skeleton")) {
			num_kframes = 0;
			while (!feof(fp) && !ferror(fp) && cstrcmp(line, "end") != 0) {
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
			if (IS_EMPTY_LINE(line)) {
				continue;
			}
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
					while (!feof(fp) && !ferror(fp) && cstrcmp(line, "end") != 0) {
						fgets(line, sizeof(line), fp);
						if (IS_EMPTY_LINE(line))
							continue;

						// 0 "ValveBiped.Bip01_Pelvis" -1
						if (sscanf_s(line, " %d %64s %d", &bone_number, bone.name, &bone.parent_id) != 3) {
							fclose(fp);
							return SMDLDR_STATUS_INVALID_LINE;
						}
						p_dst_smdmodel->hierarchy.push_back(bone); // save skeleton bone
					}
					SMD_DBG("Loader: Leave 'nodes' section. Loaded hierarchy for %d bones", p_dst_smdmodel->hierarchy.size());
					continue; // continue main loop
				}

				/* load keyframes */
				if (!cstrcmp(line, "skeleton")) {
					int current_frame;
					SMD_DBG("Loader: Enter 'skeleton' section");
					while (!feof(fp) && !ferror(fp) && cstrcmp(line, "end") != 0) {
						fgets(line, sizeof(line), fp);
						if (IS_EMPTY_LINE(line))
							continue;

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
								while (!feof(fp) && !ferror(fp) && cstrcmp(line, "end") != 0) {
									fgets(line, sizeof(line), fp);
									if (IS_EMPTY_LINE(line))
										continue;

									/* fill bone from keyframe line data */
									if (bone_idx < p_dst_smdmodel->hierarchy.size()) {
										smd_kfbone bone;
										if (sscanf_s(line, " %d %f %f %f %f %f %f", &file_bone_idx,
											&bone.position.x, &bone.position.y, &bone.position.z,
											&bone.rotation.x, &bone.rotation.y, &bone.rotation.z) != 7) {

											fclose(fp);
											return SMDLDR_STATUS_INCORRECT_KEYFRAME_BONE_DATA;
										}
										keyframe.frames.push_back(bone); // add bone to keyframe
									}
									bone_idx++;
								}
								p_dst_smdmodel->keyframes.push_back(keyframe); // add keyframe
							}
						}
					}
					SMD_DBG("Loader: Leave 'skeleton' section. Loaded %d keyframes", p_dst_smdmodel->keyframes.size());
					continue; // continue main loop
				}

				/* load triangles */
				if (!cstrcmp(line, "triangles")) {
					char previous_material[256];
					previous_material[0] = 0;
					SMD_DBG("Loader: Enter 'triangles' section");
					while (!feof(fp) && !ferror(fp) && cstrcmp(line, "end") != 0) {
						fgets(line, sizeof(line), fp);
						if (IS_EMPTY_LINE(line))
							continue;

						// <string|material>
						if (fgets(line, sizeof(line), fp) == NULL) {
							fclose(fp);
							return SMDLDR_STATUS_UNEXPECTED_END_OF_FILE;
						}



						// <int|Parent bone> <float|PosX PosY PosZ> <normal|NormX NormY NormZ> <float|U V> <int|links> <int|Bone ID> <float|Weight> [...]
						int parent;
						smd_vec3 vertex;
						smd_vec3 normal;
						smd_vec2 uv;
						int links;
						int args_read = sscanf_s(line, "%d %f %f %f %f %f %f %f %f %d", &parent, 
							&vertex.x, &vertex.y, &vertex.z, &normal.x, &normal.y, &normal.z, &uv.u, &uv.v, &links);
						if (args_read < 10) {
							fclose(fp);
							return SMDLDR_STATUS_INVALID_LINE;
						}

						//TODO: PROCESS LINKS!
					}
					SMD_DBG("Loader: Leave 'skeleton' section. Loaded %d keyframes", p_dst_smdmodel->keyframes.size());
					continue; // continue main loop
				}
			}
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
