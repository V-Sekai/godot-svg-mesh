/*************************************************************************/
/*  register_types.cpp                                                   */
/*************************************************************************/

#include "register_types.h"
#include "image_loader_svg_spatial.h"
#include "vector_graphics_adaptive_renderer.h"
#include "vector_graphics_color.h"
#include "vector_graphics_gradient.h"
#include "vector_graphics_linear_gradient.h"
#include "vector_graphics_paint.h"
#include "vector_graphics_path.h"
#include "vector_graphics_radial_gradient.h"
#include "vector_graphics_renderer.h"

#include "core/reference.h"

#ifdef TOOLS_ENABLED
static void editor_init_callback() {
	Ref<EditorSceneImporterSVG> svg_spatial_loader;
	svg_spatial_loader.instance();
	ResourceImporterScene::get_singleton()->add_importer(svg_spatial_loader);
}
#endif

void register_svg_mesh_types() {
	ClassDB::register_class<VGPath>();
	ClassDB::register_virtual_class<VGPaint>();
	ClassDB::register_class<VGColor>();
	ClassDB::register_class<VGGradient>();
	ClassDB::register_class<VGLinearGradient>();
	ClassDB::register_class<VGRadialGradient>();

	ClassDB::register_virtual_class<VGRenderer>();
	ClassDB::register_class<VGMeshRenderer>();
#ifdef TOOLS_ENABLED
	ClassDB::register_class<EditorSceneImporterSVG>();
	EditorNode::add_init_callback(editor_init_callback);
#endif
}

void unregister_svg_mesh_types() {
}
