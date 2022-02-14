
#ifndef LOADER_SVG_SPATIAL_H
#define LOADER_SVG_SPATIAL_H

#include "core/os/file_access.h"
#include "editor/editor_file_system.h"
#include "editor/import/resource_importer_scene.h"
#include "scene/3d/mesh_instance.h"
#include "scene/resources/mesh_data_tool.h"
#include "scene/resources/packed_scene.h"
#include "scene/resources/surface_tool.h"
#include "scene/resources/texture.h"
#include "vector_graphics_adaptive_renderer.h"
#include "vector_graphics_path.h"

class ResourceImporterSVGSpatial : public ResourceImporterScene {
	GDCLASS(ResourceImporterSVGSpatial, ResourceImporterScene);

public:
	ResourceImporterSVGSpatial();
	~ResourceImporterSVGSpatial();

	static Point2 compute_center(const tove::PathRef &p_path) {
		const float *bounds = p_path->getBounds();
		return Point2((bounds[0] + bounds[2]) / 2, (bounds[1] + bounds[3]) / 2);
	}
	virtual uint32_t get_import_flags() const { return 0; }
	virtual void get_extensions(List<String> *r_extensions) const { r_extensions->push_back("svg"); }
	virtual Node *import_scene(const String &p_path, uint32_t p_flags, int p_bake_fps, uint32_t p_compress_flags, List<String> *r_missing_deps, Error *r_err = nullptr);
	virtual Ref<Animation> import_animation(const String &p_path, uint32_t p_flags, int p_bake_fps) { return Ref<Animation>(); }
};

#endif