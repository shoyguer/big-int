#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/dictionary.hpp>

using namespace godot;

class BigInt : public RefCounted {
	GDCLASS(BigInt, RefCounted)

public:
	// Constants
	static const double MANTISSA_MAX;
	static const double MANTISSA_PRECISION;

	BigInt();
	BigInt(double p_mantissa, int64_t p_exponent);
	~BigInt();

	void set_mantissa(double p_mantissa);
	double get_mantissa() const;

	void set_exponent(int64_t p_exponent);
	int64_t get_exponent() const;

	void normalize();

	String _to_string() const;

protected:
	static void _bind_methods();

private:
	double mantissa = 1.0;
	int64_t exponent = 0;
};
