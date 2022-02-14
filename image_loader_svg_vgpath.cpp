#include "image_loader_svg_vgpath.h"

String ResourceImporterSVGVGPath::get_importer_name() const {

	return "svgvgpath";
}

String ResourceImporterSVGVGPath::get_visible_name() const {

	return "SVGVGPath";
}

void ResourceImporterSVGVGPath::get_recognized_extensions(List<String> *p_extensions) const {
	p_extensions->push_back("svg");
	p_extensions->push_back("svgz");
}

String ResourceImporterSVGVGPath::get_save_extension() const {
	return "scn";
}

String ResourceImporterSVGVGPath::get_resource_type() const {

	return "PackedScene";
}

bool ResourceImporterSVGVGPath::get_option_visibility(const String &p_option, const Map<StringName, Variant> &p_options) const {

	return true;
}

int ResourceImporterSVGVGPath::get_preset_count() const {
	return 0;
}
String ResourceImporterSVGVGPath::get_preset_name(int p_idx) const {

	return String();
}

void ResourceImporterSVGVGPath::get_import_options(List<ImportOption> *r_options, int p_preset) const {
}

Error ResourceImporterSVGVGPath::import(const String &p_source_file, const String &p_save_path, const Map<StringName, Variant> &p_options, List<String> *r_platform_variants, List<String> *r_gen_files, Variant *r_metadata) {
	Node2D *root = memnew(Node2D);

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
	VGPath *root_path = memnew(VGPath(tove::tove_make_shared<tove::Path>()));
	root->add_child(root_path);
	root_path->set_owner(root);
	root_path->set_renderer(renderer);
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
		path->set_name(String(name.c_str()));
		root_path->add_child(path);
		path->set_owner(root);
	}

	Ref<PackedScene> vg_scene;
	vg_scene.instance();
	vg_scene->pack(root);
	String save_path = p_save_path + ".scn";
	r_gen_files->push_back(save_path);
	return ResourceSaver::save(save_path, vg_scene);
}

ResourceImporterSVGVGPath::ResourceImporterSVGVGPath() {
}

ResourceImporterSVGVGPath::~ResourceImporterSVGVGPath() {
}
