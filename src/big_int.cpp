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

BigInt::BigInt(const String &p_string) {
	PackedStringArray scientific = p_string.split("e");
	mantissa = scientific[0].to_float();
	exponent = scientific.size() > 1 ? scientific[1].to_int() : 0;
	normalize();
}

BigInt::BigInt(int64_t p_int) {
	mantissa = (double)p_int;
	exponent = 0;
	normalize();
}

BigInt::BigInt(double p_float) {
	_size_check(p_float);
	mantissa = p_float;
	exponent = 0;
	normalize();
}

BigInt::BigInt(double p_mantissa, int64_t p_exponent) {
	_size_check(p_mantissa);
	mantissa = p_mantissa;
	exponent = p_exponent;
	normalize();
}

BigInt::BigInt(const Variant &p_val) {
	if (p_val.get_type() == Variant::OBJECT) {
		Ref<BigInt> m = p_val;
		if (m.is_valid()) {
			mantissa = m->get_mantissa();
			exponent = m->get_exponent();
		} else {
			mantissa = 1.0;
			exponent = 0;
		}
	} else if (p_val.get_type() == Variant::STRING) {
		String s = p_val;
		PackedStringArray scientific = s.split("e");
		mantissa = scientific[0].to_float();
		exponent = scientific.size() > 1 ? scientific[1].to_int() : 0;
	} else if (p_val.get_type() == Variant::INT || p_val.get_type() == Variant::FLOAT) {
		mantissa = (double)p_val;
		exponent = 0;
	} else {
		mantissa = 1.0;
		exponent = 0;
		// Error handling?
	}
	_size_check(mantissa);
	normalize();
}

BigInt::~BigInt() {
}

void BigInt::set_mantissa(double p_mantissa) {
	_size_check(p_mantissa);
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
}

Ref<BigInt> BigInt::_type_check(const Variant &n) {
	if (n.get_type() == Variant::OBJECT) {
		Ref<BigInt> b = n;
		if (b.is_valid()) {
			return b;
		}
	}
	return memnew(BigInt(n));
}

void BigInt::_size_check(double p_mantissa) {
	if (p_mantissa > MANTISSA_MAX) {
		ERR_PRINT("BigInt Error: Mantissa \"" + String::num(p_mantissa) + "\" exceeds MANTISSA_MAX.");
	}
}

bool BigInt::is_less_than(const Variant &n) const {
	Ref<BigInt> other = _type_check(n);
	
	if (mantissa == 0.0) {
		return other->mantissa > 0.0; // 0 < 0 is false
	}
	
	if (exponent < other->exponent) {
		if (exponent == other->exponent - 1 && mantissa > 10.0 * other->mantissa) {
			return false;
		}
		return true;
	} else if (exponent == other->exponent) {
		return mantissa < other->mantissa;
	} else {
		if (exponent == other->exponent + 1 && mantissa * 10.0 < other->mantissa) {
			return true;
		}
		return false;
	}
}

bool BigInt::is_equal_to(const Variant &n) const {
	Ref<BigInt> other = _type_check(n);
	return other->exponent == exponent && Math::is_equal_approx(other->mantissa, mantissa);
}

bool BigInt::is_greater_than(const Variant &n) const {
	return !is_less_than_or_equal_to(n);
}

bool BigInt::is_less_than_or_equal_to(const Variant &n) const {
	Ref<BigInt> other = _type_check(n);
	if (is_less_than(other)) {
		return true;
	}
	if (other->exponent == exponent && Math::is_equal_approx(other->mantissa, mantissa)) {
		return true;
	}
	return false;
}

bool BigInt::is_greater_than_or_equal_to(const Variant &n) const {
	return !is_less_than(n);
}

Ref<BigInt> BigInt::plus(const Variant &n) const {
	Ref<BigInt> res = memnew(BigInt(mantissa, exponent));
	res->plus_equals(n);
	return res;
}

Ref<BigInt> BigInt::plus_equals(const Variant &n) {
	Ref<BigInt> other = _type_check(n);
	
	int64_t exp_diff = other->exponent - exponent;
	
	if (exp_diff == 0) {
		mantissa += other->mantissa;
	} else if (exp_diff > 0) {
		if (exp_diff >= 248) {
			mantissa = other->mantissa;
			exponent = other->exponent;
		} else {
			double scaled_mantissa = other->mantissa * Math::pow(10.0, (double)exp_diff);
			mantissa += scaled_mantissa;
		}
	} else {
		if (-exp_diff >= 248) {
			// Other too small
		} else {
			double scaled_mantissa = other->mantissa / Math::pow(10.0, (double)(-exp_diff));
			mantissa += scaled_mantissa;
		}
	}
	
	normalize();
	return Ref<BigInt>(this);
}

Ref<BigInt> BigInt::minus(const Variant &n) const {
	Ref<BigInt> res = memnew(BigInt(mantissa, exponent));
	res->minus_equals(n);
	return res;
}

Ref<BigInt> BigInt::minus_equals(const Variant &n) {
	Ref<BigInt> other = _type_check(n);
	
	double other_mantissa_neg = -other->mantissa;
	int64_t other_exponent = other->exponent;
	
	int64_t exp_diff = other_exponent - exponent;
	
	if (exp_diff == 0) {
		mantissa += other_mantissa_neg;
	} else if (exp_diff > 0) {
		if (exp_diff >= 248) {
			mantissa = other_mantissa_neg;
			exponent = other_exponent;
		} else {
			double scaled_mantissa = other_mantissa_neg * Math::pow(10.0, (double)exp_diff);
			mantissa += scaled_mantissa;
		}
	} else {
		if (-exp_diff < 248) {
			double scaled_mantissa = other_mantissa_neg / Math::pow(10.0, (double)(-exp_diff));
			mantissa += scaled_mantissa;
		}
	}
	
	normalize();
	return Ref<BigInt>(this);
}

Ref<BigInt> BigInt::multiply(const Variant &n) const {
	Ref<BigInt> res = memnew(BigInt(mantissa, exponent));
	res->multiply_equals(n);
	return res;
}

Ref<BigInt> BigInt::multiply_equals(const Variant &n) {
	Ref<BigInt> other = _type_check(n);
	
	exponent += other->exponent;
	mantissa *= other->mantissa;
	
	normalize();
	return Ref<BigInt>(this);
}

Ref<BigInt> BigInt::divide(const Variant &n) const {
	Ref<BigInt> res = memnew(BigInt(mantissa, exponent));
	res->divide_equals(n);
	return res;
}

Ref<BigInt> BigInt::divide_equals(const Variant &n) {
	Ref<BigInt> other = _type_check(n);
	
	if (other->mantissa == 0.0) {
		ERR_PRINT("BigInt Error: Divide by zero");
		return Ref<BigInt>(this);
	}
	
	exponent -= other->exponent;
	mantissa /= other->mantissa;
	
	normalize();
	return Ref<BigInt>(this);
}

Ref<BigInt> BigInt::mod(const Variant &n) const {
	Ref<BigInt> other = _type_check(n);
	
	Ref<BigInt> quot = divide(other);
	quot->floor_value();
	
	Ref<BigInt> product = quot->multiply(other);
	Ref<BigInt> res = minus(product);
	return res;
}

Ref<BigInt> BigInt::power(const Variant &n) const {
	Ref<BigInt> res = memnew(BigInt(mantissa, exponent));
	res->power_equals(n);
	return res;
}

Ref<BigInt> BigInt::power_equals(const Variant &n) {
	if (n.get_type() == Variant::INT) {
		int64_t p = (int64_t)n;
		if (p == 0) {
			mantissa = 1.0;
			exponent = 0;
			return Ref<BigInt>(this);
		}
		// Handling negative int power? Original GDScript says "not fully supported in simple power", 
		// but simple logic: x^-p = 1 / x^p.
		// For now adapting GDScript logic:
		/*
			var new_exponent: int = exponent * p
			var new_mantissa: float = mantissa ** float(p)
			
			mantissa = new_mantissa
			exponent = new_exponent
			normalize()
		*/
		int64_t new_exponent = exponent * p;
		double new_mantissa = Math::pow(mantissa, (double)p);
		
		mantissa = new_mantissa;
		exponent = new_exponent;
		normalize();
		return Ref<BigInt>(this);

	} else if (n.get_type() == Variant::FLOAT) {
		double p = (double)n;
		if (mantissa == 0.0) return Ref<BigInt>(this);
		
		double log_val = log10();
		double new_log = log_val * p;
		
		int64_t new_exponent = (int64_t)Math::floor(new_log);
		double remainder = new_log - (double)new_exponent;
		double new_mantissa = Math::pow(10.0, remainder);
		
		exponent = new_exponent;
		mantissa = new_mantissa;
		normalize();
		return Ref<BigInt>(this);

	} else if (n.get_type() == Variant::OBJECT) {
		Ref<BigInt> other = n;
		if (other.is_valid()) {
			return power_equals(other->to_float());
		}
	}
	// Fallback?
	return Ref<BigInt>(this);
}

Ref<BigInt> BigInt::square_root() const {
	Ref<BigInt> res = memnew(BigInt(mantissa, exponent));
	
	if (res->exponent % 2 == 0) {
		res->mantissa = Math::sqrt(res->mantissa);
		res->exponent = res->exponent / 2;
	} else {
		res->mantissa = Math::sqrt(res->mantissa * 10.0);
		// integer division in C++ rounds towards zero, but for negative odd numbers?
		// e.g. -1 / 2 = 0. We want floor behavior usually for exponent math?
		// check GDScript: (res.exponent - 1) / 2
		// If exp=5. (5-1)/2 = 2. sqrt(m*10)*10^2 -> (sqrt(m*10))^2 * 10^4 = m*10 * 10^4 = m*10^5. Correct.
		res->exponent = (res->exponent - 1) / 2;
	}
	
	res->normalize();
	return res;
}

Ref<BigInt> BigInt::absolute() const {
	Ref<BigInt> res = memnew(BigInt(mantissa, exponent));
	res->mantissa = Math::abs(res->mantissa);
	return res;
}

double BigInt::log10() const {
	return (double)exponent + (Math::log(mantissa) / Math::log(10.0));
}

double BigInt::ln() const {
	return log10() * 2.302585092994046;
}

void BigInt::floor_value() {
	if (exponent == 0) {
		mantissa = Math::floor(mantissa);
	} else if (exponent < 0) {
		mantissa = 0.0;
		exponent = 0;
	} else {
		// If exponent is positive but small enough to have fractional parts visible in double precision
		if (exponent < 16) {
			double val = to_float();
			val = Math::floor(val);
			// Re-assigning from float will re-normalize
			mantissa = val;
			exponent = 0;
			normalize();
		}
		// Else: assume integer
	}
}

double BigInt::to_float() const {
	return mantissa * Math::pow(10.0, (double)exponent);
}

String BigInt::_to_string() const {
	return String::num_scientific(mantissa) + "e" + String::num_int64(exponent);
}

Dictionary BigInt::get_options() {
	static Dictionary options;
	if (options.is_empty()) {
		options["default_mantissa"] = 1.0;
		options["default_exponent"] = 0;
		options["dynamic_decimals"] = false;
		options["dynamic_numbers"] = 4;
		options["small_decimals"] = 2;
		options["thousand_decimals"] = 2;
		options["big_decimals"] = 2;
		options["scientific_decimals"] = 2;
		options["logarithmic_decimals"] = 2;
		options["maximum_trailing_zeroes"] = 3;
		options["thousand_separator"] = ",";
		options["decimal_separator"] = ".";
		options["suffix_separator"] = "";
		options["reading_separator"] = "";
		options["thousand_name"] = "thousand";
	}
	return options;
}

String BigInt::to_scientific(bool no_decimals_on_small_values, bool force_decimals) const {
	Dictionary opts = get_options();
	int scientific_decimals = opts["scientific_decimals"];
	bool dynamic_decimals = opts["dynamic_decimals"];
	int dynamic_numbers = opts["dynamic_numbers"];
	String decimal_separator = opts["decimal_separator"];
	
	if (exponent < 3) {
		double decimal_increments = 1.0 / (Math::pow(10.0, scientific_decimals) / 10.0);
		double val = Math::snapped(mantissa * Math::pow(10.0, (double)exponent), decimal_increments);
		String value = String::num(val, scientific_decimals);
		// Note: String::num might use '.' always? We should check if we need to replace it.
		// Usually internal string is dot.
		PackedStringArray split = value.split(".");
		if (no_decimals_on_small_values) return split[0];
		
		if (split.size() > 1) {
			int limit = scientific_decimals;
			if (dynamic_decimals) limit = dynamic_numbers - split[0].length(); // Can be negative? MIN handles it?
			limit = MAX(0, MIN(limit, scientific_decimals)); // Ensure positive
			
			String decimals = split[1].substr(0, limit);
			if (decimals.is_empty()) return split[0];
			return split[0] + decimal_separator + decimals;
		} else {
			return value;
		}
	} else {
		// Mantissa is 1.0 to 10.0
		String m_str = String::num(mantissa, scientific_decimals + 2); // Extra precision
		PackedStringArray split = m_str.split(".");
		if (split.size() == 1) split.append("");
		
		if (force_decimals) {
			while (split[1].length() < scientific_decimals) split[1] += "0";
		}
		
		int limit = scientific_decimals;
		if (dynamic_decimals) limit = dynamic_numbers - 1 - String::num_int64(exponent).length();
		limit = MAX(0, MIN(limit, scientific_decimals));
		
		String decl = split[1].substr(0, limit);
		if (decl.is_empty() && !force_decimals) return split[0] + "e" + String::num_int64(exponent);
		
		return split[0] + decimal_separator + decl + "e" + String::num_int64(exponent);
	}
}

String BigInt::to_prefix(bool no_decimals_on_small_values, bool use_thousand_symbol, bool force_decimals, bool scientific_prefix) const {
	Dictionary opts = get_options();
	int small_decimals = opts["small_decimals"];
	int thousand_decimals = opts["thousand_decimals"];
	int big_decimals = opts["big_decimals"];
	bool dynamic_decimals = opts["dynamic_decimals"];
	int dynamic_numbers = opts["dynamic_numbers"];
	String decimal_separator = opts["decimal_separator"];
	String thousand_separator = opts["thousand_separator"];
	
	double number = mantissa;
	if (!scientific_prefix) {
		int hundreds = 1;
		for (int i = 0; i < (exponent % 3); i++) hundreds *= 10;
		number *= hundreds;
	}
	
	String s_num = String::num(number, 10); 
	PackedStringArray split = s_num.split(".");
	if (split.size() == 1) split.append("");
	
	int max_decimals = MAX(MAX(small_decimals, thousand_decimals), big_decimals);
	if (force_decimals) {
		while (split[1].length() < max_decimals) split[1] += "0";
	}
	
	if (no_decimals_on_small_values && exponent < 3) {
		return split[0];
	} else if (exponent < 3) {
		if (small_decimals == 0 || split[1] == "") return split[0];
		int limit = small_decimals;
		if (dynamic_decimals) limit = dynamic_numbers - split[0].length();
		limit = MAX(0, MIN(limit, small_decimals));
		return split[0] + decimal_separator + split[1].substr(0, limit);
	} else if (exponent < 6) {
		if (thousand_decimals == 0 || (split[1] == "" && use_thousand_symbol)) return split[0];
		
		if (use_thousand_symbol) {
			int limit = 3;
			if (dynamic_decimals) limit = dynamic_numbers - split[0].length();
			limit = MAX(0, MIN(limit, thousand_decimals));
			return split[0] + decimal_separator + split[1].substr(0, limit);
		} else {
			return split[0] + thousand_separator + split[1].substr(0, 3);
		}
	} else {
		if (big_decimals == 0 || split[1] == "") return split[0];
		int limit = big_decimals;
		if (dynamic_decimals) limit = dynamic_numbers - split[0].length();
		limit = MAX(0, MIN(limit, big_decimals));
		return split[0] + decimal_separator + split[1].substr(0, limit);
	}
}

String BigInt::to_aa(bool no_decimals_on_small_values, bool use_thousand_symbol, bool force_decimals) const {
	static Dictionary suffixes_aa;
	if (suffixes_aa.is_empty()) {
		suffixes_aa["0"] = "";
		suffixes_aa["1"] = "k";
		suffixes_aa["2"] = "m";
		suffixes_aa["3"] = "b";
		suffixes_aa["4"] = "t";
	}

	Dictionary opts = get_options();
	String suffix_separator = opts["suffix_separator"];
	
	int64_t target = exponent / 3;
	String aa_index = String::num_int64(target);
	String suffix = "";
	
	if (!suffixes_aa.has(aa_index)) {
		// Generate suffix
		static const char* alphabet[] = {
			"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
			"n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"
		};
		int64_t offset = target + 22; // Offset matching standard AA
		int64_t base = 26;
		
		while (offset > 0) {
			offset -= 1;
			int digit = offset % base;
			suffix = String(alphabet[digit]) + suffix;
			offset /= base;
		}
		suffixes_aa[aa_index] = suffix;
	} else {
		suffix = suffixes_aa[aa_index];
	}
	
	if (!use_thousand_symbol && target == 1) {
		suffix = "";
	}
	
	String prefix = to_prefix(no_decimals_on_small_values, use_thousand_symbol, force_decimals);
	return prefix + suffix_separator + suffix;
}

String BigInt::to_metric_symbol(bool no_decimals_on_small_values) const {
	static Dictionary suffixes_metric_symbol;
	if (suffixes_metric_symbol.is_empty()) {
		suffixes_metric_symbol["0"] = "";
		suffixes_metric_symbol["1"] = "k";
		suffixes_metric_symbol["2"] = "M";
		suffixes_metric_symbol["3"] = "G";
		suffixes_metric_symbol["4"] = "T";
		suffixes_metric_symbol["5"] = "P";
		suffixes_metric_symbol["6"] = "E";
		suffixes_metric_symbol["7"] = "Z";
		suffixes_metric_symbol["8"] = "Y";
		suffixes_metric_symbol["9"] = "R";
		suffixes_metric_symbol["10"] = "Q";
	}
	Dictionary opts = get_options();
	String suffix_separator = opts["suffix_separator"];

	int64_t target = exponent / 3;
	String t_str = String::num_int64(target);
	
	if (!suffixes_metric_symbol.has(t_str)) {
		return to_scientific();
	} else {
		return to_prefix(no_decimals_on_small_values) + suffix_separator + String(suffixes_metric_symbol[t_str]);
	}
}

String BigInt::to_metric_name(bool no_decimals_on_small_values) const {
	static Dictionary suffixes_metric_name;
	if (suffixes_metric_name.is_empty()) {
		suffixes_metric_name["0"] = "";
		suffixes_metric_name["1"] = "kilo";
		suffixes_metric_name["2"] = "mega";
		suffixes_metric_name["3"] = "giga";
		suffixes_metric_name["4"] = "tera";
		suffixes_metric_name["5"] = "peta";
		suffixes_metric_name["6"] = "exa";
		suffixes_metric_name["7"] = "zetta";
		suffixes_metric_name["8"] = "yotta";
		suffixes_metric_name["9"] = "ronna";
		suffixes_metric_name["10"] = "quetta";
	}
	Dictionary opts = get_options();
	String suffix_separator = opts["suffix_separator"];

	int64_t target = exponent / 3;
	String t_str = String::num_int64(target);
	
	if (!suffixes_metric_name.has(t_str)) {
		return to_scientific();
	} else {
		return to_prefix(no_decimals_on_small_values) + suffix_separator + String(suffixes_metric_name[t_str]);
	}
}

void BigInt::_bind_methods() {
	ClassDB::bind_static_method("BigInt", D_METHOD("get_options"), &BigInt::get_options);

	ClassDB::bind_method(D_METHOD("to_scientific", "no_decimals_on_small_values", "force_decimals"), &BigInt::to_scientific, DEFVAL(false), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("to_prefix", "no_decimals_on_small_values", "use_thousand_symbol", "force_decimals", "scientific_prefix"), &BigInt::to_prefix, DEFVAL(false), DEFVAL(true), DEFVAL(true), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("to_aa", "no_decimals_on_small_values", "use_thousand_symbol", "force_decimals"), &BigInt::to_aa, DEFVAL(false), DEFVAL(true), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("to_metric_symbol", "no_decimals_on_small_values"), &BigInt::to_metric_symbol, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("to_metric_name", "no_decimals_on_small_values"), &BigInt::to_metric_name, DEFVAL(false));

	ClassDB::bind_method(D_METHOD("set_mantissa", "mantissa"), &BigInt::set_mantissa);
	ClassDB::bind_method(D_METHOD("get_mantissa"), &BigInt::get_mantissa);
	ClassDB::bind_method(D_METHOD("set_exponent", "exponent"), &BigInt::set_exponent);
	ClassDB::bind_method(D_METHOD("get_exponent"), &BigInt::get_exponent);
	ClassDB::bind_method(D_METHOD("normalize"), &BigInt::normalize);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "mantissa"), "set_mantissa", "get_mantissa");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "exponent"), "set_exponent", "get_exponent");

	ClassDB::bind_method(D_METHOD("is_less_than", "n"), &BigInt::is_less_than);
	ClassDB::bind_method(D_METHOD("is_equal_to", "n"), &BigInt::is_equal_to);
	ClassDB::bind_method(D_METHOD("is_greater_than", "n"), &BigInt::is_greater_than);
	ClassDB::bind_method(D_METHOD("is_less_than_or_equal_to", "n"), &BigInt::is_less_than_or_equal_to);
	ClassDB::bind_method(D_METHOD("is_greater_than_or_equal_to", "n"), &BigInt::is_greater_than_or_equal_to);

	ClassDB::bind_method(D_METHOD("plus", "n"), &BigInt::plus);
	ClassDB::bind_method(D_METHOD("plus_equals", "n"), &BigInt::plus_equals);
	ClassDB::bind_method(D_METHOD("minus", "n"), &BigInt::minus);
	ClassDB::bind_method(D_METHOD("minus_equals", "n"), &BigInt::minus_equals);
	ClassDB::bind_method(D_METHOD("multiply", "n"), &BigInt::multiply);
	ClassDB::bind_method(D_METHOD("multiply_equals", "n"), &BigInt::multiply_equals);
	ClassDB::bind_method(D_METHOD("divide", "n"), &BigInt::divide);
	ClassDB::bind_method(D_METHOD("divide_equals", "n"), &BigInt::divide_equals);

	ClassDB::bind_method(D_METHOD("mod", "n"), &BigInt::mod);
	ClassDB::bind_method(D_METHOD("power", "n"), &BigInt::power);
	ClassDB::bind_method(D_METHOD("power_equals", "n"), &BigInt::power_equals);
	ClassDB::bind_method(D_METHOD("square_root"), &BigInt::square_root);
	ClassDB::bind_method(D_METHOD("absolute"), &BigInt::absolute);
	ClassDB::bind_method(D_METHOD("log10"), &BigInt::log10);
	ClassDB::bind_method(D_METHOD("ln"), &BigInt::ln);
	ClassDB::bind_method(D_METHOD("floor_value"), &BigInt::floor_value);
	ClassDB::bind_method(D_METHOD("to_float"), &BigInt::to_float);
}


