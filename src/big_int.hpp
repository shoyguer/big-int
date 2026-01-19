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

	Ref<BigInt> mod(const Variant &n) const;
	Ref<BigInt> power(const Variant &n) const;
	Ref<BigInt> power_equals(const Variant &n);
	Ref<BigInt> square_root() const;
	Ref<BigInt> absolute() const;

	double log10() const;
	double ln() const;
	void floor_value();
	double to_float() const;
	String to_plain_scientific() const;

	String _to_string() const;

	// Formatting methods
	String to_scientific(bool no_decimals_on_small_values = false, bool force_decimals = false) const;
	String to_prefix(bool no_decimals_on_small_values = false, bool use_thousand_symbol = true, bool force_decimals = true, bool scientific_prefix = false) const;
	String to_aa(bool no_decimals_on_small_values = false, bool use_thousand_symbol = true, bool force_decimals = false) const;
	String to_metric_symbol(bool no_decimals_on_small_values = false) const;
	String to_metric_name(bool no_decimals_on_small_values = false) const;

	// Static configuration
	static Dictionary get_options();
	
protected:
	static void _bind_methods();

private:
	static Ref<BigInt> _type_check(const Variant &n);
	static void _size_check(double p_mantissa);
	static void _get_values(const Variant &n, double &r_mantissa, int64_t &r_exponent);

	double mantissa = 1.0;
	int64_t exponent = 0;
};
