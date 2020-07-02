#pragma once

#include "core/engine.h"
#include "core/map.h"
#include "core/resource.h"
#include "core/vector.h"

// Based on https://github.com/airbnb/lottie-android
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
  /**
   * Used to determine if an animation can be drawn with hardware acceleration.
   */
  // bool hasDashPattern;
  /**
   * Counts the number of mattes and masks. Before Android switched to SKIA
   * for drawing in Oreo (API 28), using hardware acceleration with mattes and
   * masks was only faster until you had ~4 masks after which it would actually
   * become slower.
   */
  // int32_t maskAndMatteCount = 0;

protected:
  static void _bind_methods() {}

public:
  LottieComposition() {}
};