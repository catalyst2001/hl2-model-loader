#include <stdio.h>
#include "vbsp.h"

// 
// bsp_dump_entities
// 
// dump entities key-values string
// 
bool bsp_dump_entities(const bsp_t *p_bsp)
{
	lump_t *p_entities_lump = bsp_lump(p_bsp, LUMP_ENTITIES);
	if (bsp_lump_valid(p_entities_lump)) {
		char *p_string = bsp_lump_data(p_bsp, p_entities_lump);
		p_string[p_entities_lump->filelen - 1] = '\0';
		printf("--- LUMP_ENTITIES ---\n");
		printf("%s\n", p_string);
		printf("---------------------\n");
	}
	return true;
}

// 
// bsp_dump_planes
// 
// dump planes
// 
bool bsp_dump_planes(const bsp_t *p_bsp)
{
	lump_t *p_planes_lump = bsp_lump(p_bsp, LUMP_PLANES);
	if (bsp_lump_valid(p_planes_lump)) {
		int bits;
		size_t i, j;
		cplane_t *p_in_plane, *p_out_plane;
		p_in_plane = (cplane_t *)bsp_lump_data(p_bsp, p_planes_lump);
		cplane_t *p_planes = (cplane_t *)malloc(p_planes_lump->filelen);
		size_t planes_count = p_planes_lump->filelen / sizeof(cplane_t);
		if (p_planes) {
			printf("--- LUMP_PLANES ---\n");
			for (i = 0; i < planes_count; i++, p_in_plane++) {
				p_out_plane = &p_planes[i];
				bits = 0;
				for (size_t j = 0; j < 3; j++) {
					p_out_plane->normal.v[j] = p_in_plane->normal.v[j];
					if (p_out_plane->normal.v[j] < 0)
						bits |= (1 << j);
				}
				p_out_plane->dist = p_in_plane->dist;
				p_out_plane->type = p_in_plane->type;
				p_out_plane->signbits = bits;

				/* print each plane info */
				printf(" norm( %f %f %f ) dist: %f\n", p_out_plane->normal.x, p_out_plane->normal.y, p_out_plane->normal.z, p_out_plane->dist);
			}
			printf("---------------------\n");
		}
	}
	return true;
}

// 
// bsp_dump_textures
// 
// dump textures
// 
bool bsp_dump_textures(const bsp_t *p_bsp)
{
	size_t textures_count;
	csurface_t *p_bsp_surfaces;
	dtexdata_t *p_bsp_in_texture;
	int *p_string_table;
	char *p_string_table_data, *p_texture_name;
	lump_t *p_texdata_lump, *p_texdata_strings_lump, *p_texdata_strings_table_lump;

	//find texdata
	p_texdata_lump = bsp_lump(p_bsp, LUMP_TEXDATA);
	if (!bsp_lump_valid(p_texdata_lump))
		return false;

	textures_count = p_texdata_lump->filelen / sizeof(dtexdata_t);
	p_bsp_in_texture = (dtexdata_t *)bsp_lump_data(p_bsp, p_texdata_lump); 

	//find texdata string
	p_texdata_strings_lump = bsp_lump(p_bsp, LUMP_TEXDATA_STRING_DATA);
	if (!bsp_lump_valid(p_texdata_strings_lump))
		return false;

	p_string_table_data = (char *)bsp_lump_data(p_bsp, p_texdata_strings_lump);

	// find texdata strings table
	p_texdata_strings_table_lump = bsp_lump(p_bsp, LUMP_TEXDATA_STRING_TABLE);
	if (!bsp_lump_valid(p_texdata_strings_table_lump))
		return false;

	p_bsp_surfaces = (csurface_t *)malloc(textures_count * sizeof(csurface_t)); //alloc surfaces
	if (!p_bsp_surfaces)
		return false;

	int idx = 0;
	csurface_t *p_out_surface;
	p_string_table = (int *)bsp_lump_data(p_bsp, p_texdata_strings_table_lump); //get base of string table
	if (textures_count > 0) {
		printf("--- LUMP_TEXDATA, LUMP_TEXDATA_STRING_DATA, LUMP_TEXDATA_STRING_TABLE ---\n");
		/* enum textures */
		for (size_t i = 0; i < textures_count; i++, p_bsp_in_texture++) {
			if (p_bsp_in_texture->nameStringTableID >= 0 && p_string_table[p_bsp_in_texture->nameStringTableID] >= 0) {
				p_texture_name = &p_string_table_data[p_string_table[p_bsp_in_texture->nameStringTableID]];
				idx = (int)(p_texture_name - p_string_table_data);

				p_out_surface = &p_bsp_surfaces[i];
				p_out_surface->name = &p_string_table_data[idx];
				p_out_surface->surfaceProps = 0;
				p_out_surface->flags = 0;

				printf("texture '%s'\n", p_out_surface->name);
			}

		}
		printf("------------------------------------------------------------------------\n");
	}
	return true;
}

bool bsp_dump_disp_info(const bsp_t *p_bsp, bool b_hdr)
{
	size_t faces_count;
	size_t disp_count;
	int *p_surf_edges;
	int face_lump_to_load;
	dvertex_t *p_vertices;
	dedge_t *p_edges;
	dface_t *p_faces_list;
	lump_t *p_vertexes_lump, *p_edges_lump, *p_surf_edges_lump, *p_faces_hdr, *p_faces_lump;

	// how many displacements in the map?
	disp_count = bsp_lump(p_bsp, LUMP_DISPINFO)->filelen / sizeof(ddispinfo_t);
	if (disp_count == 0) {
		printf("BSP is not contain displacements\n");
		return false;
	}

	// find vertices
	p_vertexes_lump = bsp_lump(p_bsp, LUMP_VERTEXES);
	if (!bsp_lump_valid(p_vertexes_lump))
		return false;

	p_vertices = (dvertex_t *)bsp_lump_data(p_bsp, p_vertexes_lump);
	if (p_vertexes_lump->filelen % sizeof(dvertex_t)) {
		printf("LUMP_VERTEXES: data must be a multiple!\n");
		return false;
	}

	// find edges
	p_edges_lump = bsp_lump(p_bsp, LUMP_EDGES);
	if (!bsp_lump_valid(p_edges_lump))
		return false;

	p_edges = (dedge_t *)bsp_lump_data(p_bsp, p_edges_lump);
	if (p_edges_lump->filelen % sizeof(dedge_t)) {
		printf("LUMP_EDGES: data must be a multiple!\n");
		return false;
	}


	// find surf edges data
	p_surf_edges_lump = bsp_lump(p_bsp, LUMP_SURFEDGES);
	if (!bsp_lump_valid(p_surf_edges_lump))
		return false;

	p_surf_edges = (int *)bsp_lump_data(p_bsp, p_surf_edges_lump);
	if (p_surf_edges_lump->filelen % sizeof(int)) {
		printf("LUMP_SURFEDGES: data must be a multiple!\n");
		return false;
	}


	// get face data
	face_lump_to_load = LUMP_FACES;
	p_faces_hdr = bsp_lump(p_bsp, LUMP_FACES_HDR);
	if (b_hdr && bsp_lump_valid(p_faces_hdr))
		face_lump_to_load = LUMP_FACES_HDR;
	
	p_faces_lump = bsp_lump(p_bsp, face_lump_to_load);
	p_faces_list = (dface_t *)bsp_lump_data(p_bsp, p_faces_lump);
	if (p_faces_lump->filelen % sizeof(dface_t)) {
		printf("LUMP(%d): data must be a multiple!\n", face_lump_to_load);
		return false;
	}

	faces_count = p_faces_lump->filelen / sizeof(dface_t);

	size_t size = disp_count * sizeof(unsigned short);
	unsigned short *p_disp_idx_to_face_idx = (unsigned short *)malloc(size);
	if (!p_disp_idx_to_face_idx) {
		printf("Faield to allocate p_disp_idx_to_face_idx\n");
		return false;
	}
	memset(p_disp_idx_to_face_idx, 0xFF, size);

	dface_t *p_faces = p_faces_list;
	for (size_t i = 0; i < faces_count; i++, p_faces++) {
		// check face for displacement data
		if (p_faces->dispinfo == -1)
			continue;

		// get the current displacement build surface
		if (p_faces->dispinfo >= disp_count)
			continue;

		p_disp_idx_to_face_idx[p_faces->dispinfo] = (unsigned short)i;
	}

	// load one dispinfo from disk a time and set it up.
	size_t curr_vert = 0;
	size_t curr_tri = 0;
	cdisp_vert_t disp_verts[MAX_DISPVERTS];
	cdisp_tri_t disp_tris[MAX_DISPTRIS];
	int nSize = 0;
	int nCacheSize = 0;
	int nPowerCount[3] = { 0, 0, 0 };

	ddispinfo_t *p_curr_dispinfo;
	lump_t *p_dispinfo = bsp_lump(p_bsp, LUMP_DISPINFO);
	lump_t *p_dispverts = bsp_lump(p_bsp, LUMP_DISP_VERTS);
	lump_t *p_disptris = bsp_lump(p_bsp, LUMP_DISP_TRIS);
	for (size_t i = 0; i < disp_count; i++) {
		// Find the face associated with this dispinfo
		unsigned short nFaceIndex = p_disp_idx_to_face_idx[i];
		if (nFaceIndex == 0xFFFF)
			continue;

		// Load up the dispinfo and create the CCoreDispInfo from it.
		p_curr_dispinfo = (ddispinfo_t *)bsp_lump_data_element(p_bsp, p_dispinfo, sizeof(ddispinfo_t), i);

		// Read in the vertices.
		size_t nVerts = NUM_DISP_POWER_VERTS(p_curr_dispinfo->power);
		bsp_load_lump_data(disp_verts, curr_vert * sizeof(cdisp_vert_t), nVerts * sizeof(cdisp_vert_t), p_bsp, p_dispinfo);
		curr_vert += nVerts;

		// Read in the tris.
		size_t nTris = NUM_DISP_POWER_TRIS(p_curr_dispinfo->power);
		bsp_load_lump_data(disp_tris, curr_tri * sizeof(cdisp_tri_t), nTris * sizeof(cdisp_tri_t), p_bsp, p_dispinfo);
		curr_tri += nTris;

		// Hook the disp surface to the face
		p_faces = &p_faces_list[nFaceIndex];

		// get points
		if (p_faces->numedges > 4)
			continue;

		//vec3_t surfPoints[4];
		//pFaces->numedges // point count
		//int j;
		//for (j = 0; j < p_faces->numedges; j++) {
		//	int eIndex = p_surf_edges[p_faces->firstedge + j];
		//	if (eIndex < 0)
		//		surfPoints[j] = p_vertices[p_edges[-eIndex].v[1]].point;
		//	
		//	else surfPoints[j] = p_vertices[p_edges[eIndex].v[0]].point;
		//}

		//for (j = 0; j < 4; j++)
		//	pDispSurf->SetPoint(j, surfPoints[j]);
	}
	free(p_disp_idx_to_face_idx);

	dphysdisp_t *p_physdisps = (dphysdisp_t *)bsp_lump_data(p_bsp, bsp_lump(p_bsp, LUMP_PHYSDISP));

	// create the vphysics collision models for each displacement
	// TODO: ADD CODE

	return true;
}

void bsp_dump_info(const bsp_t *p_bsp)
{
	bsp_dump_entities(p_bsp); // dump entities key-values string
	bsp_dump_planes(p_bsp); // dump planes
	bsp_dump_textures(p_bsp); // dump textures

	

}

int main()
{
	bsp_t bsp;
    printf("HL2 BSP loader\n");
	if (!bsp_load(&bsp, "$2000$.bsp")) {
		printf("Failed to load BSP file!\n");
		return 1;
	}

	bsp_dump_info(&bsp);


	bsp_free(&bsp);
	return 0;
}