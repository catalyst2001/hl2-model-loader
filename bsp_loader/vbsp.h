#pragma once
#include <stdio.h>
#include <string.h>
#include <assert.h>

typedef struct vec3_s {
	float x, y, z;
} vec3_t;

// little-endian "VBSP"   0x50534256
#define IDBSPHEADER	(('P'<<24)+('S'<<16)+('B'<<8)+'V')

// little-endian "LZMA"
#define LZMA_ID	(('A'<<24)|('M'<<16)|('Z'<<8)|('L'))

#define HEADER_LUMPS 64

#pragma pack(1) //disable alignment
typedef struct lump_s {
	int    fileofs;      // offset into file (bytes)
	int    filelen;      // length of lump (bytes)
	int    version;      // lump format version. default to zero.
	char   fourCC[4];    // lump ident code. default to ( char )0, ( char )0, ( char )0, ( char )0.
} lump_t;

// BSP header
struct dheader_s {
	int     ident;                  // BSP file identifier
	int     version;                // BSP file version
	lump_t  lumps[HEADER_LUMPS];    // lump directory array
	int     mapRevision;            // the map's revision (iteration, version) number
} dheader_t;

// level feature flags
#define LVLFLAGS_BAKED_STATIC_PROP_LIGHTING_NONHDR 0x00000001	// was processed by vrad with -staticproplighting, no hdr data
#define LVLFLAGS_BAKED_STATIC_PROP_LIGHTING_HDR    0x00000002   // was processed by vrad with -staticproplighting, in hdr

struct dflagslump_t {
	unsigned int m_LevelFlags; //LVLFLAGS_xxx
};

struct lumpfileheader_t {
	int lumpOffset;
	int lumpID;
	int lumpVersion;
	int lumpLength;
	int mapRevision; // the map's revision (iteration, version) number (added BSPVERSION 6)
};

struct dgamelumpheader_t {
	int lumpCount;
	// dgamelump_t follow this
};

// This is expected to be a four-CC code ('lump')
typedef int GameLumpId_t;

// 360 only: game lump is compressed, filelen reflects original size
// use next entry fileofs to determine actual disk lump compressed size
// compression stage ensures a terminal null dictionary entry
#define GAMELUMPFLAG_COMPRESSED	0x0001

struct dgamelump_t {
	GameLumpId_t	id;
	unsigned short	flags;
	unsigned short	version;
	int				fileofs;
	int				filelen;
};

struct dmodel_t {
	vec3_t mins, maxs;
	vec3_t origin; // for sounds or lights
	int headnode;
	int firstface, numfaces;	// submodels just draw faces without walking the bsp tree
};

struct dphysmodel_t {
	int modelIndex;
	int dataSize;
	int keydataSize;
	int solidCount;
};

// contains the binary blob for each displacement surface's virtual hull
struct dphysdisp_t {
	unsigned short numDisplacements;
	//unsigned short dataSize[numDisplacements];
};

struct dvertex_t {
	vec3_t point;
};

// planes (x&~1) and (x&~1)+1 are always opposites
struct dplane_t {
	vec3_t	normal;
	float	dist;
	int		type;		// PLANE_X - PLANE_ANYZ ?remove? trivial to regenerate
};

// --- BSP FLAGS ---
// contents flags are seperate bits
// a given brush can contribute multiple content bits
// multiple brushes can be in a single leaf

// these definitions also need to be in q_shared.h!

// lower bits are stronger, and will eat weaker brushes completely
#define	CONTENTS_EMPTY			0		// No contents

#define	CONTENTS_SOLID			0x1		// an eye is never valid in a solid
#define	CONTENTS_WINDOW			0x2		// translucent, but not watery (glass)
#define	CONTENTS_AUX			0x4
#define	CONTENTS_GRATE			0x8		// alpha-tested "grate" textures.  Bullets/sight pass through, but solids don't
#define	CONTENTS_SLIME			0x10
#define	CONTENTS_WATER			0x20
#define	CONTENTS_BLOCKLOS		0x40	// block AI line of sight
#define CONTENTS_OPAQUE			0x80	// things that cannot be seen through (may be non-solid though)
#define	LAST_VISIBLE_CONTENTS	0x80

#define ALL_VISIBLE_CONTENTS (LAST_VISIBLE_CONTENTS | (LAST_VISIBLE_CONTENTS-1))

#define CONTENTS_TESTFOGVOLUME	0x100
#define CONTENTS_UNUSED			0x200	

// unused 
// NOTE: If it's visible, grab from the top + update LAST_VISIBLE_CONTENTS
// if not visible, then grab from the bottom.
#define CONTENTS_UNUSED6		0x400

#define CONTENTS_TEAM1			0x800	// per team contents used to differentiate collisions 
#define CONTENTS_TEAM2			0x1000	// between players and objects on different teams

// ignore CONTENTS_OPAQUE on surfaces that have SURF_NODRAW
#define CONTENTS_IGNORE_NODRAW_OPAQUE	0x2000

// hits entities which are MOVETYPE_PUSH (doors, plats, etc.)
#define CONTENTS_MOVEABLE		0x4000

// remaining contents are non-visible, and don't eat brushes
#define	CONTENTS_AREAPORTAL		0x8000

#define	CONTENTS_PLAYERCLIP		0x10000
#define	CONTENTS_MONSTERCLIP	0x20000

// currents can be added to any other contents, and may be mixed
#define	CONTENTS_CURRENT_0		0x40000
#define	CONTENTS_CURRENT_90		0x80000
#define	CONTENTS_CURRENT_180	0x100000
#define	CONTENTS_CURRENT_270	0x200000
#define	CONTENTS_CURRENT_UP		0x400000
#define	CONTENTS_CURRENT_DOWN	0x800000

#define	CONTENTS_ORIGIN			0x1000000	// removed before bsping an entity

#define	CONTENTS_MONSTER		0x2000000	// should never be on a brush, only in game
#define	CONTENTS_DEBRIS			0x4000000
#define	CONTENTS_DETAIL			0x8000000	// brushes to be added after vis leafs
#define	CONTENTS_TRANSLUCENT	0x10000000	// auto set if any surface has trans
#define	CONTENTS_LADDER			0x20000000
#define CONTENTS_HITBOX			0x40000000	// use accurate hitboxes on trace


// NOTE: These are stored in a short in the engine now.  Don't use more than 16 bits
#define	SURF_LIGHT		0x0001		// value will hold the light strength
#define	SURF_SKY2D		0x0002		// don't draw, indicates we should skylight + draw 2d sky but not draw the 3D skybox
#define	SURF_SKY		0x0004		// don't draw, but add to skybox
#define	SURF_WARP		0x0008		// turbulent water warp
#define	SURF_TRANS		0x0010
#define SURF_NOPORTAL	0x0020	// the surface can not have a portal placed on it
#define	SURF_TRIGGER	0x0040	// FIXME: This is an xbox hack to work around elimination of trigger surfaces, which breaks occluders
#define	SURF_NODRAW		0x0080	// don't bother referencing the texture

#define	SURF_HINT		0x0100	// make a primary bsp splitter

#define	SURF_SKIP		0x0200	// completely ignore, allowing non-closed brushes
#define SURF_NOLIGHT	0x0400	// Don't calculate light
#define SURF_BUMPLIGHT	0x0800	// calculate three lightmaps for the surface for bumpmapping
#define SURF_NOSHADOWS	0x1000	// Don't receive shadows
#define SURF_NODECALS	0x2000	// Don't receive decals
#define SURF_NOCHOP		0x4000	// Don't subdivide patches on this surface 
#define SURF_HITBOX		0x8000	// surface is part of a hitbox


// -----------------------------------------------------
// spatial content masks - used for spatial queries (traceline,etc.)
// -----------------------------------------------------
#define	MASK_ALL					(0xFFFFFFFF)
// everything that is normally solid
#define	MASK_SOLID					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
// everything that blocks player movement
#define	MASK_PLAYERSOLID			(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
// blocks npc movement
#define	MASK_NPCSOLID				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
// water physics in these contents
#define	MASK_WATER					(CONTENTS_WATER|CONTENTS_MOVEABLE|CONTENTS_SLIME)
// everything that blocks lighting
#define	MASK_OPAQUE					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_OPAQUE)
// everything that blocks lighting, but with monsters added.
#define MASK_OPAQUE_AND_NPCS		(MASK_OPAQUE|CONTENTS_MONSTER)
// everything that blocks line of sight for AI
#define MASK_BLOCKLOS				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_BLOCKLOS)
// everything that blocks line of sight for AI plus NPCs
#define MASK_BLOCKLOS_AND_NPCS		(MASK_BLOCKLOS|CONTENTS_MONSTER)
// everything that blocks line of sight for players
#define	MASK_VISIBLE					(MASK_OPAQUE|CONTENTS_IGNORE_NODRAW_OPAQUE)
// everything that blocks line of sight for players, but with monsters added.
#define MASK_VISIBLE_AND_NPCS		(MASK_OPAQUE_AND_NPCS|CONTENTS_IGNORE_NODRAW_OPAQUE)
// bullets see these as solid
#define	MASK_SHOT					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_HITBOX)
// non-raycasted weapons see this as solid (includes grates)
#define MASK_SHOT_HULL				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_GRATE)
// hits solids (not grates) and passes through everything else
#define MASK_SHOT_PORTAL			(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER)
// everything normally solid, except monsters (world+brush only)
#define MASK_SOLID_BRUSHONLY		(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_GRATE)
// everything normally solid for player movement, except monsters (world+brush only)
#define MASK_PLAYERSOLID_BRUSHONLY	(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_PLAYERCLIP|CONTENTS_GRATE)
// everything normally solid for npc movement, except monsters (world+brush only)
#define MASK_NPCSOLID_BRUSHONLY		(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
// just the world, used for route rebuilding
#define MASK_NPCWORLDSTATIC			(CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
// These are things that can split areaportals
#define MASK_SPLITAREAPORTAL		(CONTENTS_WATER|CONTENTS_SLIME)

// UNDONE: This is untested, any moving water
#define MASK_CURRENT				(CONTENTS_CURRENT_0|CONTENTS_CURRENT_90|CONTENTS_CURRENT_180|CONTENTS_CURRENT_270|CONTENTS_CURRENT_UP|CONTENTS_CURRENT_DOWN)

// everything that blocks corpse movement
// UNDONE: Not used yet / may be deleted
#define	MASK_DEADSOLID				(CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_GRATE)

// --- BSP NODE ---
struct dnode_t {
	int planenum;
	int children[2];	// negative numbers are -(leafs+1), not nodes
	short mins[3];		// for frustom culling
	short maxs[3];
	unsigned short firstface;
	unsigned short numfaces;	// counting both sides
	short area;		// If all leaves below this node are in the same area, then this is the area index. If not, this is -1.
};

typedef struct texinfo_s {
	float textureVecsTexelsPerWorldUnits[2][4];			// [s/t][xyz offset]
	float lightmapVecsLuxelsPerWorldUnits[2][4];			// [s/t][xyz offset] - length is in units of texels/area
	int flags;				// miptex flags + overrides
	int texdata;			// Pointer to texture name, size, etc.
} texinfo_t;

#define TEXTURE_NAME_LENGTH	 128			// changed from 64 BSPVERSION 8

struct dtexdata_t {
	vec3_t reflectivity;
	int nameStringTableID; // index into g_StringTable for the texture name
	int width, height; // source image
	int view_width, view_height; //
};

//-----------------------------------------------------------------------------
// Occluders are simply polygons
//-----------------------------------------------------------------------------
// Flags field of doccluderdata_t
enum
{
	OCCLUDER_FLAGS_INACTIVE = 0x1,
};

struct doccluderdata_t {
	int flags;
	int firstpoly; // index into doccluderpolys
	int polycount;
	vec3_t mins;
	vec3_t maxs;
	int area;
};

struct doccluderdataV1_t {
	int flags;
	int firstpoly; // index into doccluderpolys
	int polycount;
	vec3_t mins;
	vec3_t maxs;
};

struct doccluderpolydata_t {
	int firstvertexindex; // index into doccludervertindices
	int vertexcount;
	int planenum;
};

#define NEIGHBOR_INVALID 0xFFFF

// --- DISP SUB NEIGHBOR ---
// NOTE: see the section above titled "displacement neighbor rules".
typedef struct disp_sub_neighbor_s {
	unsigned short m_iNeighbor;		// This indexes into ddispinfos.  0xFFFF (NEIGHBOR_INVALID) if there is no neighbor here.
	unsigned char m_NeighborOrientation; // (CCW) rotation of the neighbor wrt this displacement.

	// These use the NeighborSpan type.
	unsigned char m_Span; // Where the neighbor fits onto this side of our displacement.
	unsigned char m_NeighborSpan; // Where we fit onto our neighbor.
} disp_sub_neighbor_t;

#define neighbor_set_invalid(p) ((p)->m_iNeighbor = NEIGHBOR_INVALID)
#define neighbor_is_valid(p) ((p)->m_iNeighbor != NEIGHBOR_INVALID)

// NOTE: see the section above titled "displacement neighbor rules".
typedef struct disp_neighbor_s {
	disp_sub_neighbor_t SubNeighbors[2];
} disp_neighbor_t;

inline void disp_neighbor_set_invalid(disp_neighbor_t *p_nb)
{
	neighbor_set_invalid(&p_nb->SubNeighbors[0]);
	neighbor_set_invalid(&p_nb->SubNeighbors[1]);
}

inline bool disp_neighbor_is_invalid(const disp_neighbor_t *p_nb)
{
	return (neighbor_is_valid(&p_nb->SubNeighbors[0]) || neighbor_is_valid(&p_nb->SubNeighbors[1]));
}

// Max # of neighboring displacement touching a displacement's corner.
#define MAX_DISP_CORNER_NEIGHBORS	4

typedef struct disp_corner_neighbors_s {
	unsigned short	m_Neighbors[MAX_DISP_CORNER_NEIGHBORS];	// indices of neighbors.
	unsigned char	m_nNeighbors;
} disp_corner_neighbors_t;

#define disp_corner_neighbors_set_invalid(p) ((p)->m_nNeighbors = 0)

typedef struct disp_vert_s {
	vec3_t m_vVector; // Vector field defining displacement volume.
	float m_flDist; // Displacement distances.
	float m_flAlpha; // "per vertex" alpha values.
} disp_vert_t;

#define DISPTRI_TAG_SURFACE			(1<<0)
#define DISPTRI_TAG_WALKABLE		(1<<1)
#define DISPTRI_TAG_BUILDABLE		(1<<2)
#define DISPTRI_FLAG_SURFPROP1		(1<<3)
#define DISPTRI_FLAG_SURFPROP2		(1<<4)

typedef struct disp_tri_s {
	unsigned short m_uiTags;		// Displacement triangle tags.
} disp_tri_t;

#define NUM_DISP_POWER_VERTS(power)	( ((1 << (power)) + 1) * ((1 << (power)) + 1) )
#define NUM_DISP_POWER_TRIS(power)	( (1 << (power)) * (1 << (power)) * 2 )

// upper design bounds
#define MIN_MAP_DISP_POWER		2	// Minimum and maximum power a displacement can be.
#define MAX_MAP_DISP_POWER		4	

// Pad a number so it lies on an N byte boundary.
// So PAD_NUMBER(0,4) is 0 and PAD_NUMBER(1,4) is 4
#define PAD_NUMBER(number, boundary) ( ((number) + ((boundary)-1)) / (boundary) ) * (boundary)

// Common limits
// leaffaces, leafbrushes, planes, and verts are still bounded by
// 16 bit short limits
#define	MAX_MAP_MODELS					1024
#define	MAX_MAP_BRUSHES					8192
#define	MAX_MAP_ENTITIES				8192
#define	MAX_MAP_TEXINFO					12288
#define MAX_MAP_TEXDATA					2048
#define MAX_MAP_DISPINFO				2048
#define MAX_MAP_DISP_VERTS				( MAX_MAP_DISPINFO * ((1<<MAX_MAP_DISP_POWER)+1) * ((1<<MAX_MAP_DISP_POWER)+1) )
#define MAX_MAP_DISP_TRIS				( (1 << MAX_MAP_DISP_POWER) * (1 << MAX_MAP_DISP_POWER) * 2 )
#define MAX_DISPVERTS					NUM_DISP_POWER_VERTS( MAX_MAP_DISP_POWER )
#define MAX_DISPTRIS					NUM_DISP_POWER_TRIS( MAX_MAP_DISP_POWER )
#define	MAX_MAP_AREAS					256
#define MAX_MAP_AREA_BYTES				(MAX_MAP_AREAS/8)
#define	MAX_MAP_AREAPORTALS				1024
// Planes come in pairs, thus an even number.
#define	MAX_MAP_PLANES					65536
#define	MAX_MAP_NODES					65536
#define	MAX_MAP_BRUSHSIDES				65536
#define	MAX_MAP_LEAFS					65536
#define	MAX_MAP_VERTS					65536
#define MAX_MAP_VERTNORMALS				256000
#define MAX_MAP_VERTNORMALINDICES		256000
#define	MAX_MAP_FACES					65536
#define	MAX_MAP_LEAFFACES				65536
#define	MAX_MAP_LEAFBRUSHES 			65536
#define	MAX_MAP_PORTALS					65536
#define MAX_MAP_CLUSTERS				65536
#define MAX_MAP_LEAFWATERDATA			32768
#define MAX_MAP_PORTALVERTS				128000
#define	MAX_MAP_EDGES					256000
#define	MAX_MAP_SURFEDGES				512000
#define	MAX_MAP_LIGHTING				0x1000000
#define	MAX_MAP_VISIBILITY				0x1000000			// increased BSPVERSION 7
#define	MAX_MAP_TEXTURES				1024
#define MAX_MAP_WORLDLIGHTS				8192
#define MAX_MAP_CUBEMAPSAMPLES			1024
#define MAX_MAP_OVERLAYS				512 
#define MAX_MAP_WATEROVERLAYS			16384
#define MAX_MAP_TEXDATA_STRING_DATA		256000
#define MAX_MAP_TEXDATA_STRING_TABLE	65536
// this is stuff for trilist/tristrips, etc.
#define MAX_MAP_PRIMITIVES				32768
#define MAX_MAP_PRIMVERTS				65536
#define MAX_MAP_PRIMINDICES				65536

typedef struct ddispinfo_s
{
	vec3_t startPosition;						// start position used for orientation -- (added BSPVERSION 6)
	int m_iDispVertStart;					// Index into LUMP_DISP_VERTS.
	int m_iDispTriStart;					// Index into LUMP_DISP_TRIS.

	int power;                              // power - indicates size of map (2^power + 1)
	int minTess;                            // minimum tesselation allowed
	float smoothingAngle;                     // lighting smoothing angle
	int contents;                           // surface contents

	unsigned short	m_iMapFace;						// Which map face this displacement comes from.

	int m_iLightmapAlphaStart;				// Index into ddisplightmapalpha.
													// The count is m_pParent->lightmapTextureSizeInLuxels[0]*m_pParent->lightmapTextureSizeInLuxels[1].

	int m_iLightmapSamplePositionStart;		// Index into LUMP_DISP_LIGHTMAP_SAMPLE_POSITIONS.

	disp_neighbor_t m_EdgeNeighbors[4];		// Indexed by NEIGHBOREDGE_ defines.
	disp_corner_neighbors_t m_CornerNeighbors[4];	// Indexed by CORNER_ defines.

	enum unnamed { ALLOWEDVERTS_SIZE = PAD_NUMBER(MAX_DISPVERTS, 32) / 32 };
	unsigned long	m_AllowedVerts[ALLOWEDVERTS_SIZE];	// This is built based on the layout and sizes of our neighbors
														// and tells us which vertices are allowed to be active.
} ddispinfo_t;

#define ddisp_info_num_vertices(p) (NUM_DISP_POWER_VERTS((p)->power))
#define ddisp_info_num_tris(p) (NUM_DISP_POWER_TRIS((p)->power))

// note that edge 0 is never used, because negative edge nums are used for
// counterclockwise use of the edge in a face
struct dedge_t {
	unsigned short	v[2];		// vertex numbers
};

#define	MAXLIGHTMAPS	4

enum dprimitive_type {
	PRIM_TRILIST = 0,
	PRIM_TRISTRIP = 1,
};

struct dprimitive_t {
	unsigned char type;
	unsigned short	firstIndex;
	unsigned short	indexCount;
	unsigned short	firstVert;
	unsigned short	vertCount;
};

struct dprimvert_t {
	vec3_t pos;
};

struct dface_t
{
	unsigned short	planenum;
	unsigned char side;	// faces opposite to the node's plane direction
	unsigned char onNode; // 1 of on node, 0 if in leaf

	int firstedge;		// we must support > 64k edges
	short numedges;
	short texinfo;
	// This is a union under the assumption that a fog volume boundary (ie. water surface)
	// isn't a displacement map.
	// FIXME: These should be made a union with a flags or type field for which one it is
	// if we can add more to this.
//	union
//	{
	short dispinfo;
	// This is only for surfaces that are the boundaries of fog volumes
	// (ie. water surfaces)
	// All of the rest of the surfaces can look at their leaf to find out
	// what fog volume they are in.
	short surfaceFogVolumeID;
	//	};

	// lighting info
	unsigned char styles[MAXLIGHTMAPS];
	int lightofs; // start of [numstyles * surfsize] samples
	float area;

	// TODO: make these unsigned chars?
	int m_LightmapTextureMinsInLuxels[2];
	int m_LightmapTextureSizeInLuxels[2];

	int origFace;				// reference the original face this face was derived from

	// non-polygon primitives (strips and lists)
	unsigned short m_NumPrims;	// Top bit, if set, disables shadows on this surface (this is why there are accessors).

	unsigned short	firstPrimID;
	unsigned int	smoothingGroups;
};

#define dface_get_num_prims(p) ((p)->m_NumPrims & 0x7FFF)
inline void dface_set_num_prims(dface_t *p_face, unsigned short n_prims)
{
	assert((n_prims & 0x8000) == 0);
	p_face->m_NumPrims &= ~0x7FFF;
	p_face->m_NumPrims |= (n_prims & 0x7FFF);
}

#define dface_dynamic_shadows_enabled(p) (((p)->m_NumPrims & 0x8000) == 0)
inline void dface_set_dynamic_shadows_enabled(dface_t *p_face, bool b_enabled)
{
	if (b_enabled)
		p_face->m_NumPrims &= ~0x8000;
	else
		p_face->m_NumPrims |= 0x8000;
}

struct dfaceid_t {
	unsigned short	hammerfaceid;
};

// NOTE: Only 7-bits stored!!!
#define LEAF_FLAGS_SKY			0x01		// This leaf has 3D sky in its PVS
#define LEAF_FLAGS_RADIAL		0x02		// This leaf culled away some portals due to radial vis
#define LEAF_FLAGS_SKY2D		0x04		// This leaf has 2D sky in its PVS

// helps get the offset of a bitfield
#define BEGIN_BITFIELD( name ) \
	union \
	{ \
		char name; \
		struct \
		{

#define END_BITFIELD() \
		}; \
	};

struct dleaf_version_0_t {
	int contents;			// OR of all brushes (not needed?)
	short cluster;

	BEGIN_BITFIELD(bf);
	short area : 9;
	short flags : 7;			// Per leaf flags.
	END_BITFIELD();

	short mins[3]; // for frustum culling
	short maxs[3];

	unsigned short	firstleafface;
	unsigned short	numleaffaces;

	unsigned short	firstleafbrush;
	unsigned short	numleafbrushes;
	short			leafWaterDataID; // -1 for not in water

	// Precaculated light info for entities.
	CompressedLightCube m_AmbientLighting;
};

// version 1
struct dleaf_t {
	int				contents;			// OR of all brushes (not needed?)

	short			cluster;

	BEGIN_BITFIELD(bf);
	short			area : 9;
	short			flags : 7;			// Per leaf flags.
	END_BITFIELD();

	short			mins[3];			// for frustum culling
	short			maxs[3];

	unsigned short	firstleafface;
	unsigned short	numleaffaces;

	unsigned short	firstleafbrush;
	unsigned short	numleafbrushes;
	short			leafWaterDataID; // -1 for not in water

	// NOTE: removed this for version 1 and moved into separate lump "LUMP_LEAF_AMBIENT_LIGHTING" or "LUMP_LEAF_AMBIENT_LIGHTING_HDR"
	// Precaculated light info for entities.
//	CompressedLightCube m_AmbientLighting;
};

// each leaf contains N samples of the ambient lighting
// each sample contains a cube of ambient light projected on to each axis
// and a sampling position encoded as a 0.8 fraction (mins=0,maxs=255) of the leaf's bounding box
struct dleafambientlighting_t {
	CompressedLightCube	cube;
	unsigned char x; // fixed point fraction of leaf bounds
	unsigned char y; // fixed point fraction of leaf bounds
	unsigned char z; // fixed point fraction of leaf bounds
	unsigned char pad; // unused
};

struct dleafambientindex_t {
	unsigned short ambientSampleCount;
	unsigned short firstAmbientSample;
};

struct dbrushside_t {
	unsigned short planenum;		// facing out of the leaf
	short texinfo;
	short dispinfo;		// displacement info (BSPVERSION 7)
	short bevel;			// is the side a bevel plane? (BSPVERSION 7)
};

struct dbrush_t {
	int firstside;
	int numsides;
	int contents;
};

#define	ANGLE_UP	-1
#define	ANGLE_DOWN	-2

// the visibility lump consists of a header with a count, then
// byte offsets for the PVS and PHS of each cluster, then the raw
// compressed bit vectors
#define	DVIS_PVS	0
#define	DVIS_PAS	1
struct dvis_t {
	int			numclusters;
	int			bitofs[8][2];	// bitofs[numclusters][2]
};

// each area has a list of portals that lead into other areas
// when portals are closed, other areas may not be visible or
// hearable even if the vis info says that it should be
struct dareaportal_t {
	unsigned short	m_PortalKey;		// Entities have a key called portalnumber (and in vbsp a variable
									// called areaportalnum) which is used
									// to bind them to the area portals by comparing with this value.

	unsigned short	otherarea;		// The area this portal looks into.

	unsigned short	m_FirstClipPortalVert;	// Portal geometry.
	unsigned short	m_nClipPortalVerts;

	int planenum;
};

struct darea_t {
	int numareaportals;
	int firstareaportal;
};

struct dleafwaterdata_t {
	float surfaceZ;
	float minZ;
	short surfaceTexInfoID;
};


//TODO: CFaceMacroTextureInfo

// lights that were used to illuminate the world
enum emittype_t
{
	emit_surface,		// 90 degree spotlight
	emit_point,			// simple point light source
	emit_spotlight,		// spotlight with penumbra
	emit_skylight,		// directional light with no falloff (surface must trace to SKY texture)
	emit_quakelight,	// linear falloff, non-lambertian
	emit_skyambient,	// spherical light source with no falloff (surface must trace to SKY texture)
};

// Flags for dworldlight_t::flags
#define DWL_FLAGS_INAMBIENTCUBE		0x0001	// This says that the light was put into the per-leaf ambient cubes.

struct dworldlight_t {
	vec3_t origin;
	vec3_t intensity;
	vec3_t normal; // for surfaces and spotlights
	int cluster;
	emittype_t	type;
	int style;
	float stopdot; // start of penumbra for emit_spotlight
	float stopdot2; // end of penumbra for emit_spotlight
	float exponent; // 
	float radius; // cutoff distance
	// falloff for emit_spotlight + emit_point: 
	// 1 / (constant_attn + linear_attn * dist + quadratic_attn * dist^2)
	float constant_attn;
	float linear_attn;
	float quadratic_attn;
	int flags; // Uses a combination of the DWL_FLAGS_ defines.
	int texinfo; // 
	int owner; // entity that this light it relative to
};

struct dcubemapsample_t {
	int			origin[3];			// position of light snapped to the nearest integer
									// the filename for the vtf file is derived from the position
	unsigned char size;				// 0 - default
									// otherwise, 1<<(size-1)
};

#define OVERLAY_BSP_FACE_COUNT	64

#define OVERLAY_NUM_RENDER_ORDERS		(1<<OVERLAY_RENDER_ORDER_NUM_BITS)
#define OVERLAY_RENDER_ORDER_NUM_BITS	2
#define OVERLAY_RENDER_ORDER_MASK		0xC000	// top 2 bits set

struct doverlay_t
{
	int			nId;
	short		nTexInfo;

	// Accessors..
	//void			SetFaceCount(unsigned short count);
	//unsigned short	GetFaceCount() const;

	//void			SetRenderOrder(unsigned short order);
	//unsigned short	GetRenderOrder() const;

	unsigned short	m_nFaceCountAndRenderOrder;
	int aFaces[OVERLAY_BSP_FACE_COUNT];
	float flU[2];
	float flV[2];
	vec3_t vecUVPoints[4];
	vec3_t vecOrigin;
	vec3_t vecBasisNormal;
};

//TODO: --- CONTINUE THIS! ---
//inline void dwateroverlay_t::SetFaceCount(unsigned short count)
//{
//	Assert(count >= 0 && (count & WATEROVERLAY_RENDER_ORDER_MASK) == 0);
//	m_nFaceCountAndRenderOrder &= WATEROVERLAY_RENDER_ORDER_MASK;
//	m_nFaceCountAndRenderOrder |= (count & ~WATEROVERLAY_RENDER_ORDER_MASK);
//}
//
//inline unsigned short dwateroverlay_t::GetFaceCount() const
//{
//	return m_nFaceCountAndRenderOrder & ~WATEROVERLAY_RENDER_ORDER_MASK;
//}
//
//inline void dwateroverlay_t::SetRenderOrder(unsigned short order)
//{
//	Assert(order >= 0 && order < WATEROVERLAY_NUM_RENDER_ORDERS);
//	m_nFaceCountAndRenderOrder &= ~WATEROVERLAY_RENDER_ORDER_MASK;
//	m_nFaceCountAndRenderOrder |= (order << (16 - WATEROVERLAY_RENDER_ORDER_NUM_BITS));	// leave 2 bits for render order.
//}
//
//inline unsigned short dwateroverlay_t::GetRenderOrder() const
//{
//	return (m_nFaceCountAndRenderOrder >> (16 - WATEROVERLAY_RENDER_ORDER_NUM_BITS));
//}
//
//#ifndef _DEF_BYTE_
//#define _DEF_BYTE_
//typedef unsigned char	byte;
//typedef unsigned short	word;
//#endif
//
//
//#define	ANGLE_UP	-1
//#define	ANGLE_DOWN	-2
//
//
////===============
//
//
//struct epair_t
//{
//	epair_t	*next;
//	char	*key;
//	char	*value;
//};
//
//// finalized page of surface's lightmaps
//#define MAX_LIGHTMAPPAGE_WIDTH	256
//#define MAX_LIGHTMAPPAGE_HEIGHT	128
//typedef struct nameForDatadesc_dlightmappage_t // unnamed structs collide in the datadesc macros
//{
//	DECLARE_BYTESWAP_DATADESC();
//	byte	data[MAX_LIGHTMAPPAGE_WIDTH*MAX_LIGHTMAPPAGE_HEIGHT];
//	byte	palette[256 * 4];
//} dlightmappage_t;
//
//typedef struct nameForDatadesc_dlightmappageinfo_t // unnamed structs collide in the datadesc macros
//{
//	DECLARE_BYTESWAP_DATADESC();
//	byte			page;			// lightmap page [0..?]
//	byte			offset[2];		// offset into page (s,t)
//	byte			pad;			// unused
//	ColorRGBExp32	avgColor;		// average used for runtime lighting calcs
//} dlightmappageinfo_t;



/* other structs */
//typedef struct lzma_header_s {
//	unsigned int    id;
//	unsigned int    actualSize;         // always little endian
//	unsigned int    lzmaSize;           // always little endian
//	unsigned char   properties[5];
//} lzma_header_t;
//
//struct ZIPEndOfCentralDirectoryRecord
//{
//	hlUInt uiSignature; // 4 bytes (0x06054b50)
//	hlUInt16 uiNumberOfThisDisk;  // 2 bytes
//	hlUInt16 uiNumberOfTheDiskWithStartOfCentralDirectory; // 2 bytes
//	hlUInt16 uiCentralDirectoryEntriesThisDisk;	// 2 bytes
//	hlUInt16 uiCentralDirectoryEntriesTotal;	// 2 bytes
//	hlUInt uiCentralDirectorySize; // 4 bytes
//	hlUInt uiStartOfCentralDirOffset; // 4 bytes
//	hlUInt16 uiCommentLength; // 2 bytes
//	// zip file comment follows
//};
//
//struct ZIPFileHeader
//{
//	hlUInt uiSignature; //  4 bytes (0x02014b50) 
//	hlUInt16 uiVersionMadeBy; // version made by 2 bytes 
//	hlUInt16 uiVersionNeededToExtract; // version needed to extract 2 bytes 
//	hlUInt16 uiFlags; // general purpose bit flag 2 bytes 
//	hlUInt16 uiCompressionMethod; // compression method 2 bytes 
//	hlUInt16 uiLastModifiedTime; // last mod file time 2 bytes 
//	hlUInt16 uiLastModifiedDate; // last mod file date 2 bytes 
//	hlUInt uiCRC32; // crc-32 4 bytes 
//	hlUInt uiCompressedSize; // compressed size 4 bytes 
//	hlUInt uiUncompressedSize; // uncompressed size 4 bytes 
//	hlUInt16 uiFileNameLength; // file name length 2 bytes 
//	hlUInt16 uiExtraFieldLength; // extra field length 2 bytes 
//	hlUInt16 uiFileCommentLength; // file comment length 2 bytes 
//	hlUInt16 uiDiskNumberStart; // disk number start 2 bytes 
//	hlUInt16 uiInternalFileAttribs; // internal file attributes 2 bytes 
//	hlUInt uiExternalFileAttribs; // external file attributes 4 bytes 
//	hlUInt uiRelativeOffsetOfLocalHeader; // relative offset of local header 4 bytes 
//	// file name (variable size) 
//	// extra field (variable size) 
//	// file comment (variable size) 
//};
//
//struct ZIPLocalFileHeader
//{
//	hlUInt uiSignature; //local file header signature 4 bytes (0x04034b50) 
//	hlUInt16 uiVersionNeededToExtract; // version needed to extract 2 bytes 
//	hlUInt16 uiFlags; // general purpose bit flag 2 bytes 
//	hlUInt16 uiCompressionMethod; // compression method 2 bytes 
//	hlUInt16 uiLastModifiedTime; // last mod file time 2 bytes 
//	hlUInt16 uiLastModifiedDate; // last mod file date 2 bytes 
//	hlUInt uiCRC32; // crc-32 4 bytes 
//	hlUInt uiCompressedSize; // compressed size 4 bytes 
//	hlUInt uiUncompressedSize; // uncompressed size 4 bytes 
//	hlUInt16 uiFileNameLength; // file name length 2 bytes 
//	hlUInt16 uiExtraFieldLength; // extra field length 2 bytes 
//	// file name (variable size) 
//	// extra field (variable size) 
//	// file data (variable size) 
//};
#pragma pack()