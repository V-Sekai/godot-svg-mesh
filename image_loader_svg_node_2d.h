#pragma once

#include "core/io/resource_importer.h"
#include "core/io/resource_saver.h"
#include "core/os/file_access.h"
#include "editor/editor_file_system.h"
#include "scene/3d/mesh_instance.h"
#include "scene/resources/packed_scene.h"
#include "scene/resources/texture.h"
#include "vector_graphics_adaptive_renderer.h"
#include "vector_graphics_path.h"

class ResourceImporterSVGNode2D : public ResourceImporter {
	GDCLASS(ResourceImporterSVGNode2D, ResourceImporter);

public:
	virtual String get_importer_name() const;
	virtual String get_visible_name() const;
	virtual void get_recognized_extensions(List<String> *p_extensions) const;
	virtual String get_save_extension() const;
	virtual String get_resource_type() const;

	virtual int get_preset_count() const;
	virtual String get_preset_name(int p_idx) const;

	virtual void get_import_options(List<ImportOption> *r_options, int p_preset = 0) const;
	virtual bool get_option_visibility(const String &p_option, const Map<StringName, Variant> &p_options) const;
	virtual Error import(const String &p_source_file, const String &p_save_path, const Map<StringName, Variant> &p_options, List<String> *r_platform_variants, List<String> *r_gen_files = NULL, Variant *r_metadata = NULL);

	ResourceImporterSVGNode2D();
	~ResourceImporterSVGNode2D();
};

String ResourceImporterSVGNode2D::get_importer_name() const {

	return "svgnode2d";
}

String ResourceImporterSVGNode2D::get_visible_name() const {

	return "SVGNode2D";
}

void ResourceImporterSVGNode2D::get_recognized_extensions(List<String> *p_extensions) const {
	p_extensions->push_back("svg");
	p_extensions->push_back("svgz");
}

String ResourceImporterSVGNode2D::get_save_extension() const {
	return "scn";
}

String ResourceImporterSVGNode2D::get_resource_type() const {

	return "PackedScene";
}

bool ResourceImporterSVGNode2D::get_option_visibility(const String &p_option, const Map<StringName, Variant> &p_options) const {

	return true;
}

int ResourceImporterSVGNode2D::get_preset_count() const {
	return 0;
}
String ResourceImporterSVGNode2D::get_preset_name(int p_idx) const {

	return String();
}

void ResourceImporterSVGNode2D::get_import_options(List<ImportOption> *r_options, int p_preset) const {
}

Error ResourceImporterSVGNode2D::import(const String &p_source_file, const String &p_save_path, const Map<StringName, Variant> &p_options, List<String> *r_platform_variants, List<String> *r_gen_files, Variant *r_metadata) {
	Node2D *root = memnew(Node2D);

	Ref<VGMeshRenderer> renderer;
	renderer.instance();

	String units = "px";
	float dpi = 100.0;

	Vector<uint8_t> buf = FileAccess::get_file_as_array(p_source_file);

	if (!buf.size()) {
		return FAILED;
	}

	String str;
	str.parse_utf8((const char *)buf.ptr(), buf.size());

	tove::GraphicsRef tove_graphics = tove::Graphics::createFromSVG(
			str.utf8().ptr(), units.utf8().ptr(), dpi);
	{
		const float *bounds = tove_graphics->getBounds();
		float s = 256.0f / MAX(bounds[2] - bounds[0], bounds[3] - bounds[1]);
		if (s > 1.0f) {
			tove::nsvg::Transform transform(s, 0, 0, 0, s, 0);
			transform.setWantsScaleLineWidth(true);
			tove_graphics->set(tove_graphics, transform);
		}
	}
	int32_t n = tove_graphics->getNumPaths();
	for (int i = 0; i < n; i++) {
		tove::PathRef tove_path = tove_graphics->getPath(i);
		VGPath *path = memnew(VGPath(tove_path));

		std::string name = tove_path->getName();
		if (name.empty()) {
			name = "Path";
		}

		path->set_renderer(renderer);

		MeshInstance2D *mesh_inst = memnew(MeshInstance2D);
		Ref<ArrayMesh> mesh;
		mesh.instance();
		Ref<Texture> texture;
		Ref<Material> renderer_material;
		renderer->render_mesh(mesh, renderer_material, texture, path, true, false);
		mesh_inst->set_mesh(mesh);
		mesh_inst->set_material(renderer_material);
		mesh_inst->set_texture(texture);
		Transform2D path_xform = path->get_transform();
		mesh_inst->set_transform(path_xform);
		mesh_inst->set_name(String(name.c_str()));
		mesh_inst->set_z_index(i);
		root->add_child(mesh_inst);
		mesh_inst->set_owner(root);
	}

	Ref<PackedScene> vg_scene;
	vg_scene.instance();
	vg_scene->pack(root);
	String save_path = p_save_path + ".scn";
	r_gen_files->push_back(save_path);
	return ResourceSaver::save(save_path, vg_scene);
}

ResourceImporterSVGNode2D::ResourceImporterSVGNode2D() {
}

ResourceImporterSVGNode2D::~ResourceImporterSVGNode2D() {
}
