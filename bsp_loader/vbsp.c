#include "vbsp.h"

#define MAX_SIZE ((size_t)-1)

bool bsp_load(bsp_t *p_bsp, const char *p_filename)
{
	fpos_t size;
	FILE *fp = fopen(p_filename, "rb");
	if (!fp)
		return false;

	fseek(fp, 0, SEEK_END);
	fgetpos(fp, &size);
	rewind(fp);

	if (size >= MAX_SIZE) {
		fclose(fp);
		return false;
	}

	p_bsp->data_size = (size_t)size;
	p_bsp->p_data = (unsigned char *)malloc(p_bsp->data_size);
	if (!p_bsp->p_data) {
		fclose(fp);
		return false;
	}

	if (fread(p_bsp->p_data, 1, p_bsp->data_size, fp) != p_bsp->data_size) {
		free(p_bsp->p_data);
		fclose(fp);
		return false;
	}
	fclose(fp);

	p_bsp->type = BSP_HALF_LIFE_2;
	p_bsp->p_header = (dheader_t *)p_bsp->p_data;
	if (p_bsp->p_header->ident != IDBSPHEADER) {
		free(p_bsp->p_data);
		return false;
	}
	return true;
}

//TODO: https://github.com/VSES/SourceEngine2007/blob/43a5c90a5ada1e69ca044595383be67f40b33c61/se2007/engine/cmodel_bsp.cpp
//void CollisionBSPData_LoadTextures(CCollisionBSPData *pBSPData);
//void CollisionBSPData_LoadTexinfo(CCollisionBSPData *pBSPData, CUtlVector<unsigned short> &map_texinfo);
//void CollisionBSPData_LoadLeafs_Version_0(CCollisionBSPData *pBSPData);
//void CollisionBSPData_LoadLeafs_Version_1(CCollisionBSPData *pBSPData);
//void CollisionBSPData_LoadLeafs(CCollisionBSPData *pBSPData);
//void CollisionBSPData_LoadLeafBrushes(CCollisionBSPData *pBSPData);
//void CollisionBSPData_LoadPlanes(CCollisionBSPData *pBSPData);
//void CollisionBSPData_LoadBrushes(CCollisionBSPData *pBSPData);
//void CollisionBSPData_LoadBrushSides(CCollisionBSPData *pBSPData, CUtlVector<unsigned short> &map_texinfo);
//void CollisionBSPData_LoadSubmodels(CCollisionBSPData *pBSPData);
//void CollisionBSPData_LoadNodes(CCollisionBSPData *pBSPData);
//void CollisionBSPData_LoadAreas(CCollisionBSPData *pBSPData);
//void CollisionBSPData_LoadAreaPortals(CCollisionBSPData *pBSPData);
//void CollisionBSPData_LoadVisibility(CCollisionBSPData *pBSPData);
//void CollisionBSPData_LoadEntityString(CCollisionBSPData *pBSPData);
//void CollisionBSPData_LoadPhysics(CCollisionBSPData *pBSPData);
//void CollisionBSPData_LoadDispInfo(CCollisionBSPData *pBSPData);




void bsp_free(bsp_t *p_bsp)
{
	if (p_bsp->p_data) {
		free(p_bsp->p_data);
		p_bsp->p_data = NULL;
	}
}
