/*************************************************************************/
/*  vg_adaptive_renderer.h                                               */
/*************************************************************************/

#ifndef VG_ADAPTIVE_RENDERER_H
#define VG_ADAPTIVE_RENDERER_H

#include "vector_graphics_mesh_renderer.h"

class VGMeshRenderer : public VGAbstractMeshRenderer {
	GDCLASS(VGMeshRenderer, VGRenderer);

	float quality;

protected:
	void create_tesselator();

	static void _bind_methods();

public:
	VGMeshRenderer();

	float get_quality();
	void set_quality(float p_quality);
};

#endif // VG_ADAPTIVE_RENDERER_H
