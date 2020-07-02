#pragma once

#include "core/bind/core_bind.h"
#include "core/engine.h"
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
class Layer : public Resource {
  GDCLASS(Layer, Resource);

protected:
  static void _bind_methods() {}

public:
  Layer() {}
};
class Marker : public Resource {
  GDCLASS(Marker, Resource);

protected:
  static void _bind_methods() {}

public:
  Marker() {}
};
class LottieFontCharacter : public Resource {
  GDCLASS(LottieFontCharacter, Resource);

protected:
  static void _bind_methods() {}

public:
  LottieFontCharacter() {}
};
/**
 * After Effects/Bodymovin composition model. This is the serialized model from
 * which the animation will be created.
 *
 * Based on https://github.com/airbnb/lottie-android
 */
class LottieComposition : public Resource {
  GDCLASS(LottieComposition, Resource);
  // PerformanceTracker performanceTracker = new PerformanceTracker();
  Set<String> warnings;
  Map<String, Vector<Layer>> precomps;
  Map<String, LottieImageAsset> images;
  /** Map of font names to fonts */
  Map<String, LottieFont> fonts;
  Vector<Marker> markers;
  Vector<LottieFontCharacter> characters;
  Vector<Layer> layer_map;
  Vector<Layer> layers;
  Rect2 bounds;
  float start_frame;
  float end_frame;
  float frame_rate;

protected:
  static void _bind_methods() {}

public:
  bool from_asset(const String p_path) {
    FileAccess *f = FileAccess::open(p_path, FileAccess::READ);
    ERR_FAIL_COND_V(!f, Error::ERR_DOES_NOT_EXIST);
    String json = f->get_as_utf8_string();
    Ref<JSONParseResult> result = _JSON::get_singleton()->parse(json);
    ERR_FAIL_COND_V(result.is_null(), Error::ERR_PARSE_ERROR);
    Dictionary data = result->get_result();
    return OK;
  }
  float get_duration_frames() { return end_frame - start_frame; }
  LottieComposition() {}
};