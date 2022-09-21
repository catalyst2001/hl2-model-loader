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

bool bsp_dump_disp_info(const bsp_t *p_bsp)
{
	int *p_surf_edges;
	int face_lump_to_load;
	dvertex_t *p_vertices;
	dedge_t *p_edges;
	lump_t *p_vertexes_lump, *p_edges_lump, *p_surf_edges_lump;

	// find vertices
	p_vertexes_lump = bsp_lump(p_bsp, LUMP_VERTEXES);
	if (!bsp_lump_valid(p_vertexes_lump))
		return false;

	p_vertices = (dvertex_t *)bsp_lump_data(p_bsp, p_vertexes_lump);
	if (p_vertexes_lump->filelen % sizeof(dvertex_t)) {
		printf("LUMP_VERTEXES: data must be even!\n");
		return false;
	}


	// find edges
	p_edges_lump = bsp_lump(p_bsp, LUMP_EDGES);
	if (!bsp_lump_valid(p_edges_lump))
		return false;

	p_edges = (dedge_t *)bsp_lump_data(p_bsp, p_edges_lump);
	if (p_edges_lump->filelen % sizeof(dedge_t)) {
		printf("LUMP_EDGES: data must be even!\n");
		return false;
	}


	// find surf edges data
	p_surf_edges_lump = bsp_lump(p_bsp, LUMP_SURFEDGES);
	if (!bsp_lump_valid(p_surf_edges_lump))
		return false;

	p_surf_edges = (int *)bsp_lump_data(p_bsp, p_surf_edges_lump);
	if (p_surf_edges_lump->filelen % sizeof(int)) {
		printf("LUMP_SURFEDGES: data must be even!\n");
		return false;
	}


	// get face data
	face_lump_to_load = LUMP_FACES;


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