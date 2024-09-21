#include "mdl.h"

mdl_model_t *mdl_load(int *p_dst_status, const char *p_filename)
{
	FILE *fp;
	fpos_t fsize;
	mdl_model_t *p_model;
	size_t file_size;

	fopen_s(&fp, p_filename, "rb");
	if (!fp)
		return NULL; // failed to open file

	fu_get_size(&fsize, fp);
	if (fsize < sizeof(mdl_header_t))
		return NULL; // invalid file size

	file_size = (size_t)fsize;
	p_model = (mdl_model_t *)malloc(file_size + sizeof(mdl_model_t));
	if (!p_model)
		return NULL; // allocation failed

	p_model->p_base = ((uint8_t *)p_model) + sizeof(mdl_model_t); // compute start of model data address
	if (fread(p_model->p_base, 1, file_size, fp) != file_size)
		return NULL; // reading failed

	fclose(fp); // close file

	/* identity model data */
	mdl_header_t *p_hdr = (mdl_header_t *)p_model->p_base;
	if (p_hdr->id != STUDIOMDL_ID)
		return NULL; // id validation failed

	p_model->p_bones = (mdl_bone_t *)p_model->p_base + p_hdr->



	return 0;
}
