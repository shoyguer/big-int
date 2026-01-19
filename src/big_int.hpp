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
	BigInt(const String &p_string);
	BigInt(int64_t p_int);
	BigInt(double p_float);
	BigInt(double p_mantissa, int64_t p_exponent);
	BigInt(const Variant &p_val);
	~BigInt();

	void set_mantissa(double p_mantissa);
	double get_mantissa() const;

	void set_exponent(int64_t p_exponent);
	int64_t get_exponent() const;

	void normalize();

	bool is_less_than(const Variant &n) const;
	bool is_equal_to(const Variant &n) const;
	bool is_greater_than(const Variant &n) const;
	bool is_less_than_or_equal_to(const Variant &n) const;
	bool is_greater_than_or_equal_to(const Variant &n) const;

	Ref<BigInt> plus(const Variant &n) const;
	Ref<BigInt> plus_equals(const Variant &n);
	Ref<BigInt> minus(const Variant &n) const;
	Ref<BigInt> minus_equals(const Variant &n);
	Ref<BigInt> multiply(const Variant &n) const;
	Ref<BigInt> multiply_equals(const Variant &n);
	Ref<BigInt> divide(const Variant &n) const;
	Ref<BigInt> divide_equals(const Variant &n);

	String _to_string() const;

protected:
	static void _bind_methods();

private:
	static Ref<BigInt> _type_check(const Variant &n);

	double mantissa = 1.0;
	int64_t exponent = 0;
};
