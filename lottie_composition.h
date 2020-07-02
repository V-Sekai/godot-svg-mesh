#pragma once

#include "core/bind/core_bind.h"
#include "core/engine.h"
#include "core/io/resource_importer.h"
#include "core/map.h"
#include "core/resource.h"
#include "core/vector.h"

class LottieFont : public Resource {
	GDCLASS(LottieFont, Resource);

protected:
	static void _bind_methods() {}

public:
	LottieFont() {}
};

class LottieImageAsset : public Resource {
	GDCLASS(LottieImageAsset, Resource);

protected:
	static void _bind_methods() {}

public:
	LottieImageAsset() {}
};

class LottieLayer : public Resource {
	GDCLASS(LottieLayer, Resource);

protected:
	static void _bind_methods() {}

public:
	LottieLayer() {}
};

class LottieMarker : public Resource {
	GDCLASS(LottieMarker, Resource);

	String name;
	float start_frame;
	float duration_frames;

protected:
	static void _bind_methods() {}

public:
	bool matches_name(String p_name) {
		if (p_name == name.to_lower()) {
			return true;
		}

		// Trim the name to prevent confusion in design.
		if (p_name == name.to_lower().strip_edges(false, true)) {
			return true;
		}
		return false;
	}
	LottieMarker(){};
};

class LottieFontCharacter : public Resource {
	GDCLASS(LottieFontCharacter, Resource);

protected:
	static void _bind_methods() {}

public:
	LottieFontCharacter() {}
};

/**
 * After Effects/Bodymovin composition model. This is the serialized model
 * from which the animation will be created.
 *
 * Based on https://github.com/airbnb/lottie-android
 */
class LottieComposition : public Resource {
	GDCLASS(LottieComposition, Resource);
	// PerformanceTracker performanceTracker = new PerformanceTracker();
	Set<String> warnings;
	Map<String, Vector<LottieLayer> > precomps;
	Map<String, LottieImageAsset> images;
	/** Map of font names to fonts */
	Map<String, LottieFont> fonts;
	Vector<LottieMarker> markers;
	Vector<LottieFontCharacter> characters;
	Vector<LottieLayer> layer_map;
	Vector<LottieLayer> layers;
	Rect2 bounds;
	float start_frame = 0.0f;
	float end_frame = 0.0f;
	float frame_rate = 0.0f;
	String version;
	bool three_dimentional = false;

protected:
	static void _bind_methods() {}

public:
	Error import(const String p_raw_document) {
		Ref<JSONParseResult> result = _JSON::get_singleton()->parse(p_raw_document);
		ERR_FAIL_COND_V(result.is_null(), Error::ERR_PARSE_ERROR);
		Dictionary data = result->get_result();

		version = data["v"];
		frame_rate = data["fr"];
		set_name(data["nm"]);
		three_dimentional = data["ddd"];
		start_frame = data["ip"];
		end_frame = data["op"];
		bounds = Rect2(0, 0, data["w"], data["h"]);
		Array _assets = data["assets"];
		Array _layers = data["layers"];
		Array _fonts = data["fonts"];
		Array _chars = data["chars"];

		return OK;
	}
	float get_duration_frames() { return end_frame - start_frame; }
	LottieComposition() {}
};

class LottieFormatLoader : public ResourceImporter {
	GDCLASS(LottieFormatLoader, ResourceImporter);

public:
	virtual String get_importer_name() const { return "lottie"; }
	virtual String get_visible_name() const { return "Lottie"; }
	virtual void get_recognized_extensions(List<String> *p_extensions) const {
		p_extensions->push_back("json");
	}
	virtual String get_save_extension() const { return "tres"; }
	virtual String get_resource_type() const { return "LottieComposition"; }

	virtual int get_preset_count() const { return 0; }
	virtual String get_preset_name(int p_idx) const { return ""; }

	virtual void get_import_options(List<ImportOption> *r_options, int p_preset = 0) const {}
	virtual bool get_option_visibility(const String &p_option, const Map<StringName, Variant> &p_options) const { return true; }
	virtual Error import(const String &p_source_file, const String &p_save_path, const Map<StringName, Variant> &p_options, List<String> *r_platform_variants, List<String> *r_gen_files = NULL, Variant *r_metadata = NULL) {
		FileAccess *f = FileAccess::open(p_source_file, FileAccess::READ);
		ERR_FAIL_COND_V(!f, FAILED);
		String json = f->get_as_utf8_string();
		Ref<LottieComposition> lottie_composition;
		lottie_composition.instance();
		Error err = lottie_composition->import(json);
		ERR_FAIL_COND_V(err != OK, FAILED);
		return ResourceSaver::save(p_save_path, lottie_composition);
	}
	LottieFormatLoader() {}
	~LottieFormatLoader() {}
};
