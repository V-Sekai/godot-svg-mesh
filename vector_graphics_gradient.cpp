/*************************************************************************/
/*  vg_gradient.cp          	                                             */
/*************************************************************************/

#include "vector_graphics_gradient.h"
#include "core/core_string_names.h"

void VGGradient::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_color_ramp", "color_ramp"), &VGGradient::set_color_ramp);
	ClassDB::bind_method(D_METHOD("get_color_ramp"), &VGGradient::get_color_ramp);

	ClassDB::bind_method(D_METHOD("_gradient_changed"), &VGGradient::_gradient_changed);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "color_ramp", PROPERTY_HINT_RESOURCE_TYPE, "Gradient"), "set_color_ramp", "get_color_ramp");
}

VGGradient::VGGradient() {
	Ref<Gradient> g;
	g.instantiate();
	set_color_ramp(g);
}

void VGGradient::_gradient_changed() {
}

void VGGradient::set_color_ramp(const Ref<Gradient> &p_color_ramp) {
	if (color_ramp.is_valid()) {
		color_ramp->disconnect(SNAME("_gradient_changed"), callable_mp(this, &VGGradient::_gradient_changed));
	}

	color_ramp = p_color_ramp;

	if (color_ramp.is_valid()) {
		color_ramp->connect(SNAME("_gradient_changed"), callable_mp(this, &VGGradient::_gradient_changed));
	}
}

Ref<Gradient> VGGradient::get_color_ramp() const {
	return color_ramp;
}
