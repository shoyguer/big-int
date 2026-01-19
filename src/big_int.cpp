#include "big_int.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/math.hpp>

using namespace godot;

const double BigInt::MANTISSA_MAX = 1209600.0;
const double BigInt::MANTISSA_PRECISION = 0.0000001;

BigInt::BigInt() {
	mantissa = 1.0;
	exponent = 0;
}

BigInt::BigInt(double p_mantissa, int64_t p_exponent) {
	mantissa = p_mantissa;
	exponent = p_exponent;
	normalize();
}

BigInt::~BigInt() {
}

void BigInt::set_mantissa(double p_mantissa) {
	mantissa = p_mantissa;
	normalize();
}

double BigInt::get_mantissa() const {
	return mantissa;
}

void BigInt::set_exponent(int64_t p_exponent) {
	exponent = p_exponent;
	normalize();
}

int64_t BigInt::get_exponent() const {
	return exponent;
}

void BigInt::normalize() {
	if (mantissa == 0.0) {
		exponent = 0;
		return;
	}

	// Handle signs
	bool is_negative = mantissa < 0.0;
	if (is_negative) {
		mantissa = -mantissa;
	}

	mantissa = Math::snapped(mantissa, MANTISSA_PRECISION);

	// If still effectively zero after snap
	if (mantissa == 0.0) {
		exponent = 0;
		return;
	}

	if (mantissa < 1.0 || mantissa >= 10.0) {
		int64_t diff = (int64_t)floor(Math::log(mantissa) / Math::log(10.0)); // Equivalent to log10

		// Only apply large shifts if reasonable to avoid overflow in pow
		if (diff > -10 && diff < 248) {
			double div = Math::pow(10.0, (double)diff);
			if (div > MANTISSA_PRECISION) {
				mantissa /= div;
				exponent += diff;
			}
		}
	}

	// Catch edge cases from floating point errors
	while (mantissa < 1.0) {
		mantissa *= 10.0;
		exponent -= 1;
	}
	while (mantissa >= 10.0) {
		mantissa /= 10.0;
		exponent += 1;
	}

	mantissa = Math::snapped(mantissa, MANTISSA_PRECISION);
	if (mantissa == 0.0) {
		mantissa = 0.0;
		exponent = 0;
	}

	if (is_negative) {
		mantissa = -mantissa;
	}
}

String BigInt::_to_string() const {
	return String::num_scientific(mantissa) + "e" + String::num_int64(exponent);
}

void BigInt::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_mantissa", "mantissa"), &BigInt::set_mantissa);
	ClassDB::bind_method(D_METHOD("get_mantissa"), &BigInt::get_mantissa);
	ClassDB::bind_method(D_METHOD("set_exponent", "exponent"), &BigInt::set_exponent);
	ClassDB::bind_method(D_METHOD("get_exponent"), &BigInt::get_exponent);
	ClassDB::bind_method(D_METHOD("normalize"), &BigInt::normalize);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "mantissa"), "set_mantissa", "get_mantissa");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "exponent"), "set_exponent", "get_exponent");
}

