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

class ResourceImporterSVGSpatial : public ResourceImporter {
	GDCLASS(ResourceImporterSVGSpatial, ResourceImporter);

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

	static Point2 compute_center(const tove::PathRef &p_path) {
		const float *bounds = p_path->getBounds();
		return Point2((bounds[0] + bounds[2]) / 2, (bounds[1] + bounds[3]) / 2);
	}

	ResourceImporterSVGSpatial();
	~ResourceImporterSVGSpatial();
};


String ResourceImporterSVGSpatial::get_importer_name() const {

	return "svgspatial";
}

String ResourceImporterSVGSpatial::get_visible_name() const {

	return "SVGSpatial";
}

void ResourceImporterSVGSpatial::get_recognized_extensions(List<String> *p_extensions) const {
	p_extensions->push_back("svg");
	p_extensions->push_back("svgz");
}

String ResourceImporterSVGSpatial::get_save_extension() const {
	return "scn";
}

String ResourceImporterSVGSpatial::get_resource_type() const {

	return "PackedScene";
}

bool ResourceImporterSVGSpatial::get_option_visibility(const String &p_option, const Map<StringName, Variant> &p_options) const {

	return true;
}

int ResourceImporterSVGSpatial::get_preset_count() const {
	return 0;
}
String ResourceImporterSVGSpatial::get_preset_name(int p_idx) const {

	return String();
}

void ResourceImporterSVGSpatial::get_import_options(List<ImportOption> *r_options, int p_preset) const {
}

Error ResourceImporterSVGSpatial::import(const String &p_source_file, const String &p_save_path, const Map<StringName, Variant> &p_options, List<String> *r_platform_variants, List<String> *r_gen_files, Variant *r_metadata) {
	Spatial *root = memnew(Spatial);

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
	Ref<VGMeshRenderer> renderer;
	renderer.instance();
	for (int i = 0; i < n; i++) {
		tove::PathRef tove_path = tove_graphics->getPath(i);		
		Point2 center = compute_center(tove_path);
		tove_path->set(tove_path, tove::nsvg::Transform(1, 0, -center.x, 0, 1, -center.y));
		VGPath *path = memnew(VGPath(tove_path));
		path->set_position(center);
		std::string name = tove_path->getName();
		if (name.empty()) {
			name = "Path";
		}

		path->set_renderer(renderer);

		MeshInstance *mesh_inst = memnew(MeshInstance);
		Ref<ArrayMesh> mesh;
		mesh.instance();
		Ref<Texture> texture;
		Ref<Material> renderer_material;
		renderer->render_mesh(mesh, renderer_material, texture, path, true, true);
		mesh_inst->set_mesh(mesh);
		for (int32_t i = 0; i < mesh->get_surface_count(); i++) {
			Ref<SpatialMaterial> material;
			material.instance();
			material->set_flag(SpatialMaterial::FLAG_ALBEDO_FROM_VERTEX_COLOR, true);
            material->set_cull_mode(SpatialMaterial::CULL_DISABLED);
			if (texture.is_valid()) {
				material->set_texture(SpatialMaterial::TEXTURE_ALBEDO, texture);
			}
			if (!renderer_material.is_valid()) {
				mesh->surface_set_material(i, material);
			} else {
				mesh->surface_set_material(i, renderer_material);
			}
		}

		Transform2D path_xform = path->get_transform();
		Transform xform;
		Quat rot;
		xform.basis.set_quat_scale(rot, Vector3(path_xform.get_scale().x * 0.01f, -path_xform.get_scale().y * 0.01f, 1.0f * 0.01f));
		Vector2 origin = path_xform.get_origin();
		xform.origin = Vector3(origin.x * 0.01f, origin.y * 0.01f, i * 0.003f);
		mesh_inst->set_transform(xform);
		mesh_inst->set_name(String(name.c_str()));
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

ResourceImporterSVGSpatial::ResourceImporterSVGSpatial() {
}

ResourceImporterSVGSpatial::~ResourceImporterSVGSpatial() {
}
