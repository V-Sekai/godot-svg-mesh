#include "image_loader_svg_spatial.h"

EditorSceneImporterSVG::EditorSceneImporterSVG() {
}

EditorSceneImporterSVG::~EditorSceneImporterSVG() {
}

Node *EditorSceneImporterSVG::import_scene(const String &p_path, uint32_t p_flags, const Map<StringName, Variant> &p_options, int p_bake_fps, List<String> *r_missing_deps, Error *r_err) {
	String units = "px";
	float dpi = 96.0;
	Vector<uint8_t> buf = FileAccess::get_file_as_array(p_path);
	if (!buf.size()) {
		return nullptr;
	}
	String str;
	str.parse_utf8((const char *)buf.ptr(), buf.size());
	tove::GraphicsRef tove_graphics = tove::Graphics::createFromSVG(
			str.utf8().ptr(), units.utf8().ptr(), dpi);
	const float *tove_bounds = tove_graphics->getBounds();
	float s = 256.0f / MAX(tove_bounds[2] - tove_bounds[0], tove_bounds[3] - tove_bounds[1]);
	if (s > 1.0f) {
		tove::nsvg::Transform transform(s, 0, 0, 0, s, 0);
		transform.setWantsScaleLineWidth(true);
		tove_graphics->set(tove_graphics, transform);
	}
	int32_t n = tove_graphics->getNumPaths();
	Ref<VGMeshRenderer> renderer;
	renderer.instantiate();
	renderer->set_quality(0.4);
	VGPath *root_path = memnew(VGPath(tove::tove_make_shared<tove::Path>()));
	root_path->set_renderer(renderer);
	Node3D *root = memnew(Node3D);
	Ref<SurfaceTool> st;
	st.instantiate();
	for (int i = 0; i < n; i++) {
		tove::PathRef tove_path = tove_graphics->getPath(i);
		Point2 center = compute_center(tove_path);
		tove_path->set(tove_path, tove::nsvg::Transform(1, 0, -center.x, 0, 1, -center.y));
		VGPath *path = memnew(VGPath(tove_path));
		path->set_position(center);
		root_path->add_child(path, true);
		path->set_owner(root);
		Ref<ArrayMesh> mesh;
		mesh.instantiate();
		Ref<Texture> texture;
		Ref<Material> renderer_material;
		Rect2 area = renderer->render_mesh(mesh, renderer_material, texture, path, true, true);
		if (area.is_equal_approx(Rect2())) {
			continue;
		}
		if (renderer_material.is_valid()) {
			mesh->surface_set_material(0, renderer_material);
		}
		Transform3D xform;
		xform.origin = Vector3(center.x * 0.001f, center.y * -0.001f, 0.0f);
		if (mesh.is_null()) {
			continue;
		}
		st->append_from(mesh, 0, xform);
	}
	memdelete(root_path);
	Ref<ArrayMesh> combined_mesh = st->commit();
	if (combined_mesh.is_null()) {
		return nullptr;
	}
	Ref<StandardMaterial3D> standard_material;
	standard_material.instantiate();
	standard_material->set_flag(StandardMaterial3D::FLAG_ALBEDO_FROM_VERTEX_COLOR, true);
	standard_material->set_depth_draw_mode(StandardMaterial3D::DEPTH_DRAW_ALWAYS);
	standard_material->set_flag(StandardMaterial3D::FLAG_DISABLE_DEPTH_TEST, true);
	standard_material->set_cull_mode(StandardMaterial3D::CULL_DISABLED);
	combined_mesh->surface_set_material(0, standard_material);
	MeshInstance3D *mesh_inst = memnew(MeshInstance3D);
	mesh_inst->set_mesh(combined_mesh);
	mesh_inst->set_name(String("Path"));
	root->add_child(mesh_inst, true);
	mesh_inst->set_owner(root);
	Vector3 translate = combined_mesh->get_aabb().get_size();
	translate.x = -translate.x / 2.0f;
	translate.y = translate.y / 2.0f;
	Transform3D xform;
	xform.origin = translate;
	mesh_inst->set_transform(xform);
	return root;
}

void EditorSceneImporterSVG::get_extensions(List<String> *r_extensions) const {
	r_extensions->push_back("svg");
}
