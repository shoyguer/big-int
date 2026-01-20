#include "big_number.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/math.hpp>

using namespace godot;

const double BigNumber::MANTISSA_MAX = 1209600.0;
const double BigNumber::MANTISSA_PRECISION = 0.0000001;

namespace {
const char *METRIC_SYMBOLS[] = {
	"", "k", "M", "G", "T", "P", "E", "Z", "Y", "R", "Q",
	"V", "U", "Td", "S", "Ri", "Qx", "Pp", "O", "N", "Mi"
};

const char *METRIC_NAMES[] = {
	"", "kilo", "mega", "giga", "tera", "peta", "exa", "zetta", "yotta", "ronna", "quetta",
	"vunda", "uda", "treda", "sorta", "rinta", "quexa", "pepta", "ocha", "nena", "ming"
};

const char *ALPHABET[] = {
	"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
	"n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"
};

// ln(10)
const double LOG_10 = 2.302585092994046;

struct OptionKeys {
	StringName default_mantissa = "default_mantissa";
	StringName default_exponent = "default_exponent";
	StringName dynamic_decimals = "dynamic_decimals";
	StringName dynamic_numbers = "dynamic_numbers";
	StringName small_decimals = "small_decimals";
	StringName thousand_decimals = "thousand_decimals";
	StringName big_decimals = "big_decimals";
	StringName scientific_decimals = "scientific_decimals";
	StringName logarithmic_decimals = "logarithmic_decimals";
	StringName maximum_trailing_zeroes = "maximum_trailing_zeroes";
	StringName thousand_separator = "thousand_separator";
	StringName decimal_separator = "decimal_separator";
	StringName suffix_separator = "suffix_separator";
	StringName reading_separator = "reading_separator";
	StringName thousand_name = "thousand_name";
};

const OptionKeys &get_option_keys() {
	static OptionKeys k;
	return k;
}
}

BigNumber::BigNumber() {
	mantissa = 1.0;
	exponent = 0;
}

BigNumber::BigNumber(const String &p_string) {
	PackedStringArray scientific = p_string.split("e");
	mantissa = scientific[0].to_float();
	exponent = scientific.size() > 1 ? scientific[1].to_int() : 0;
	normalize();
}

BigNumber::BigNumber(int64_t p_int) {
	mantissa = (double)p_int;
	exponent = 0;
	normalize();
}

BigNumber::BigNumber(double p_float) {
	_size_check(p_float);
	mantissa = p_float;
	exponent = 0;
	normalize();
}

BigNumber::BigNumber(double p_mantissa, int64_t p_exponent) {
	_size_check(p_mantissa);
	mantissa = p_mantissa;
	exponent = p_exponent;
	normalize();
}

BigNumber::BigNumber(const Variant &p_val) {
	if (p_val.get_type() == Variant::OBJECT) {
		Ref<BigNumber> m = p_val;
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

BigNumber::~BigNumber() {
}

void BigNumber::set_mantissa(double p_mantissa) {
	_size_check(p_mantissa);
	mantissa = p_mantissa;
	normalize();
}

double BigNumber::get_mantissa() const {
	return mantissa;
}

void BigNumber::set_exponent(int64_t p_exponent) {
	exponent = p_exponent;
	normalize();
}

int64_t BigNumber::get_exponent() const {
	return exponent;
}

void BigNumber::normalize() {
	if (mantissa == 0.0) {
		exponent = 0;
		return;
	}

	// Handle signs
	bool is_negative = mantissa < 0.0;
	if (is_negative) {
		mantissa = -mantissa;
	}

	if (mantissa >= 10.0 || mantissa < 1.0) {
		double log_val = Math::log(mantissa) / LOG_10;
		int64_t exp_change = (int64_t)Math::floor(log_val);

		exponent += exp_change;
		mantissa /= Math::pow(10.0, (double)exp_change);
	}
}

void BigNumber::_get_values(const Variant &n, double &r_mantissa, int64_t &r_exponent) {
	if (n.get_type() == Variant::INT) {
		r_mantissa = (double)(int64_t)n;
		r_exponent = 0;
	} else if (n.get_type() == Variant::FLOAT) {
		r_mantissa = (double)n;
		r_exponent = 0;
	} else if (n.get_type() == Variant::OBJECT) {
		Ref<BigNumber> b = n;
		if (b.is_valid()) {
			r_mantissa = b->get_mantissa();
			r_exponent = b->get_exponent();
			return;
		}
		// Invalid object, fall through to fallback
	} else {
		// Fallback/String/Other
		Ref<BigNumber> temp = memnew(BigNumber(n));
		r_mantissa = temp->get_mantissa();
		r_exponent = temp->get_exponent();
		return;
	}

	// Normalize r_mantissa/r_exponent for INT/FLOAT types
	if (r_mantissa == 0.0) {
		r_exponent = 0;
		return;
	}
	if (r_mantissa < 0.0) {
		r_mantissa = -r_mantissa;
	}

	if (r_mantissa >= 10.0 || r_mantissa < 1.0) {
		double log_val = Math::log(r_mantissa) / LOG_10;
		int64_t exp_change = (int64_t)Math::floor(log_val);

		r_exponent += exp_change;
		r_mantissa /= Math::pow(10.0, (double)exp_change);
	}
}

Ref<BigNumber> BigNumber::_type_check(const Variant &n) {
	if (n.get_type() == Variant::OBJECT) {
		Ref<BigNumber> b = n;
		if (b.is_valid()) {
			return b;
		}
	}
	return memnew(BigNumber(n));
}

void BigNumber::_size_check(double p_mantissa) {
	if (p_mantissa > MANTISSA_MAX) {
		ERR_PRINT("BigNumber Error: Mantissa \"" + String::num(p_mantissa) + "\" exceeds MANTISSA_MAX.");
	}
}

bool BigNumber::is_less_than(const Variant &n) const {
	double other_mantissa;
	int64_t other_exponent;
	_get_values(n, other_mantissa, other_exponent);
	
	if (mantissa == 0.0) {
		return other_mantissa > 0.0; // 0 < 0 is false
	}
	
	if (exponent < other_exponent) {
		if (exponent == other_exponent - 1 && mantissa > 10.0 * other_mantissa) {
			return false;
		}
		return true;
	} else if (exponent == other_exponent) {
		return mantissa < other_mantissa;
	} else {
		if (exponent == other_exponent + 1 && mantissa * 10.0 < other_mantissa) {
			return true;
		}
		return false;
	}
}

bool BigNumber::is_equal_to(const Variant &n) const {
	double other_mantissa;
	int64_t other_exponent;
	_get_values(n, other_mantissa, other_exponent);
	return other_exponent == exponent && Math::is_equal_approx(other_mantissa, mantissa);
}

bool BigNumber::is_greater_than(const Variant &n) const {
	return !is_less_than_or_equal_to(n);
}

bool BigNumber::is_less_than_or_equal_to(const Variant &n) const {
	Ref<BigNumber> other = _type_check(n);
	if (is_less_than(other)) {
		return true;
	}
	if (other->exponent == exponent && Math::is_equal_approx(other->mantissa, mantissa)) {
		return true;
	}
	return false;
}

bool BigNumber::is_greater_than_or_equal_to(const Variant &n) const {
	return !is_less_than(n);
}

Ref<BigNumber> BigNumber::plus(const Variant &n) const {
	Ref<BigNumber> res = memnew(BigNumber(mantissa, exponent));
	res->plus_equals(n);
	return res;
}

Ref<BigNumber> BigNumber::plus_equals(const Variant &n) {
	double other_mantissa;
	int64_t other_exponent;
	_get_values(n, other_mantissa, other_exponent);
	
	int64_t exp_diff = other_exponent - exponent;
	
	if (exp_diff == 0) {
		mantissa += other_mantissa;
	} else if (exp_diff > 0) {
		if (exp_diff >= 248) {
			mantissa = other_mantissa;
			exponent = other_exponent;
		} else {
			double scaled_mantissa = other_mantissa * Math::pow(10.0, (double)exp_diff);
			mantissa += scaled_mantissa;
		}
	} else {
		if (-exp_diff >= 248) {
			// Other too small
		} else {
			double scaled_mantissa = other_mantissa / Math::pow(10.0, (double)(-exp_diff));
			mantissa += scaled_mantissa;
		}
	}
	
	normalize();
	return Ref<BigNumber>(this);
}

Ref<BigNumber> BigNumber::minus(const Variant &n) const {
	Ref<BigNumber> res = memnew(BigNumber(mantissa, exponent));
	res->minus_equals(n);
	return res;
}

Ref<BigNumber> BigNumber::minus_equals(const Variant &n) {
	double other_mantissa;
	int64_t other_exponent;
	_get_values(n, other_mantissa, other_exponent);
	
	double other_mantissa_neg = -other_mantissa;
	
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
	return Ref<BigNumber>(this);
}

Ref<BigNumber> BigNumber::multiply(const Variant &n) const {
	Ref<BigNumber> res = memnew(BigNumber(mantissa, exponent));
	res->multiply_equals(n);
	return res;
}

Ref<BigNumber> BigNumber::multiply_equals(const Variant &n) {
	double other_mantissa;
	int64_t other_exponent;
	_get_values(n, other_mantissa, other_exponent);
	
	exponent += other_exponent;
	mantissa *= other_mantissa;
	
	normalize();
	return Ref<BigNumber>(this);
}

Ref<BigNumber> BigNumber::divide(const Variant &n) const {
	Ref<BigNumber> res = memnew(BigNumber(mantissa, exponent));
	res->divide_equals(n);
	return res;
}

Ref<BigNumber> BigNumber::divide_equals(const Variant &n) {
	double other_mantissa;
	int64_t other_exponent;
	_get_values(n, other_mantissa, other_exponent);
	
	if (other_mantissa == 0.0) {
		ERR_PRINT("BigNumber Error: Divide by zero");
		return Ref<BigNumber>(this);
	}
	
	exponent -= other_exponent;
	mantissa /= other_mantissa;
	
	normalize();
	return Ref<BigNumber>(this);
}

Ref<BigNumber> BigNumber::mod(const Variant &n) const {
	Ref<BigNumber> other = _type_check(n);
	
	Ref<BigNumber> quot = divide(other);
	quot->floor_value();
	
	Ref<BigNumber> product = quot->multiply(other);
	Ref<BigNumber> res = minus(product);
	return res;
}

Ref<BigNumber> BigNumber::power(const Variant &n) const {
	Ref<BigNumber> res = memnew(BigNumber(mantissa, exponent));
	res->power_equals(n);
	return res;
}

Ref<BigNumber> BigNumber::power_equals(const Variant &n) {
	if (n.get_type() == Variant::INT) {
		int64_t p = (int64_t)n;
		if (p == 0) {
			mantissa = 1.0;
			exponent = 0;
			return Ref<BigNumber>(this);
		}
		// Handling negative int power? Original GDScript says "not fully supported in simple power", 
		// but simple logic: x^-p = 1 / x^p.
		// For now using GDScript logic:
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
		return Ref<BigNumber>(this);

	} else if (n.get_type() == Variant::FLOAT) {
		double p = (double)n;
		if (mantissa == 0.0) return Ref<BigNumber>(this);
		
		double log_val = log10();
		double new_log = log_val * p;
		
		int64_t new_exponent = (int64_t)Math::floor(new_log);
		double remainder = new_log - (double)new_exponent;
		double new_mantissa = Math::pow(10.0, remainder);
		
		exponent = new_exponent;
		mantissa = new_mantissa;
		normalize();
		return Ref<BigNumber>(this);

	} else if (n.get_type() == Variant::OBJECT) {
		Ref<BigNumber> other = n;
		if (other.is_valid()) {
			return power_equals(other->to_float());
		}
	}
	// Fallback?
	return Ref<BigNumber>(this);
}

Ref<BigNumber> BigNumber::square_root() const {
	Ref<BigNumber> res = memnew(BigNumber(mantissa, exponent));
	
	if (res->exponent % 2 == 0) {
		res->mantissa = Math::sqrt(res->mantissa);
		res->exponent = res->exponent / 2;
	} else {
		res->mantissa = Math::sqrt(res->mantissa * 10.0);
		res->exponent = (res->exponent - 1) / 2;
	}
	
	res->normalize();
	return res;
}

Ref<BigNumber> BigNumber::absolute() const {
	Ref<BigNumber> res = memnew(BigNumber(mantissa, exponent));
	res->mantissa = Math::abs(res->mantissa);
	return res;
}

double BigNumber::log10() const {
	return (double)exponent + (Math::log(mantissa) / LOG_10);
}

double BigNumber::ln() const {
	return log10() * LOG_10;
}

void BigNumber::floor_value() {
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

double BigNumber::to_float() const {
	return mantissa * Math::pow(10.0, (double)exponent);
}

String BigNumber::to_plain_scientific() const {
	return String::num(mantissa) + "e" + String::num_int64(exponent);
}

String BigNumber::_to_string() const {
	String m_str = String::num(mantissa);
	int mantissa_decimals = 0;
	if (m_str.find(".") >= 0) {
		mantissa_decimals = m_str.split(".")[1].length();
	}

	if (mantissa_decimals > exponent) {
		if (exponent < 248) {
			return String::num(mantissa * Math::pow(10.0, (double)exponent));
		} else {
			return to_plain_scientific();
		}
	} else {
		String mantissa_string = m_str.replace(".", "");
		for (int i = 0; i < (exponent - mantissa_decimals); i++) {
			mantissa_string += "0";
		}
		return mantissa_string;
	}
}

Dictionary BigNumber::get_options() {
	static Dictionary options;
	if (options.is_empty()) {
		const OptionKeys &k = get_option_keys();
		options[k.default_mantissa] = 1.0;
		options[k.default_exponent] = 0;
		options[k.dynamic_decimals] = false;
		options[k.dynamic_numbers] = 4;
		options[k.small_decimals] = 2;
		options[k.thousand_decimals] = 2;
		options[k.big_decimals] = 2;
		options[k.scientific_decimals] = 2;
		options[k.logarithmic_decimals] = 2;
		options[k.maximum_trailing_zeroes] = 3;
		options[k.thousand_separator] = ",";
		options[k.decimal_separator] = ".";
		options[k.suffix_separator] = "";
		options[k.reading_separator] = "";
		options[k.thousand_name] = "thousand";
	}
	return options;
}

String BigNumber::to_scientific(bool no_decimals_on_small_values, bool force_decimals) const {
	const OptionKeys &k = get_option_keys();

	Dictionary opts = get_options();
	int scientific_decimals = opts[k.scientific_decimals];
	bool dynamic_decimals = opts[k.dynamic_decimals];
	int dynamic_numbers = opts[k.dynamic_numbers];
	String decimal_separator = opts[k.decimal_separator];
	
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

String BigNumber::to_prefix(bool no_decimals_on_small_values, bool use_thousand_symbol, bool force_decimals, bool scientific_prefix) const {
	const OptionKeys &k = get_option_keys();

	Dictionary opts = get_options();
	int small_decimals = opts[k.small_decimals];
	int thousand_decimals = opts[k.thousand_decimals];
	int big_decimals = opts[k.big_decimals];
	bool dynamic_decimals = opts[k.dynamic_decimals];
	int dynamic_numbers = opts[k.dynamic_numbers];
	String decimal_separator = opts[k.decimal_separator];
	String thousand_separator = opts[k.thousand_separator];
	
	double number = mantissa;
	if (!scientific_prefix) {
		int hundreds = 1;
		for (int i = 0; i < (exponent % 3); i++) hundreds *= 10;
		number *= hundreds;
	}
	
	String s_num = String::num(number); 
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

String BigNumber::to_aa(bool no_decimals_on_small_values, bool use_thousand_symbol, bool force_decimals) const {
	static Dictionary suffixes_aa;
	if (suffixes_aa.is_empty()) {
		suffixes_aa["0"] = "";
		suffixes_aa["1"] = "k";
		suffixes_aa["2"] = "m";
		suffixes_aa["3"] = "b";
		suffixes_aa["4"] = "t";
	}

	const OptionKeys &k = get_option_keys();
	Dictionary opts = get_options();
	String suffix_separator = opts[k.suffix_separator];
	
	int64_t target = exponent / 3;
	String aa_index = String::num_int64(target);
	String suffix = "";
	
	if (!suffixes_aa.has(aa_index)) {
		// Generate suffix
		int64_t offset = target + 22; // Offset matching standard AA
		int64_t base = 26;
		
		while (offset > 0) {
			offset -= 1;
			int digit = offset % base;
			suffix = String(ALPHABET[digit]) + suffix;
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

String BigNumber::to_metric_symbol(bool no_decimals_on_small_values) const {
	Dictionary opts = get_options();
	const OptionKeys &k = get_option_keys();
	String suffix_separator = opts[k.suffix_separator];

	int64_t target = exponent / 3;

	if (target >= 0 && target < (int64_t)(sizeof(METRIC_SYMBOLS) / sizeof(METRIC_SYMBOLS[0]))) {
		return to_prefix(no_decimals_on_small_values) + suffix_separator + METRIC_SYMBOLS[target];
	} else {
		return to_scientific();
	}
}

String BigNumber::to_metric_name(bool no_decimals_on_small_values) const {
	Dictionary opts = get_options();
	const OptionKeys &k = get_option_keys();
	String suffix_separator = opts[k.suffix_separator];

	int64_t target = exponent / 3;

	if (target >= 0 && target < (int64_t)(sizeof(METRIC_NAMES) / sizeof(METRIC_NAMES[0]))) {
		return to_prefix(no_decimals_on_small_values) + suffix_separator + METRIC_NAMES[target];
	} else {
		return to_scientific();
	}
}

void BigNumber::_bind_methods() {
	ClassDB::bind_static_method("BigNumber", D_METHOD("get_options"), &BigNumber::get_options);

	ClassDB::bind_method(D_METHOD("to_scientific", "no_decimals_on_small_values", "force_decimals"), &BigNumber::to_scientific, DEFVAL(false), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("to_prefix", "no_decimals_on_small_values", "use_thousand_symbol", "force_decimals", "scientific_prefix"), &BigNumber::to_prefix, DEFVAL(false), DEFVAL(true), DEFVAL(true), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("to_aa", "no_decimals_on_small_values", "use_thousand_symbol", "force_decimals"), &BigNumber::to_aa, DEFVAL(false), DEFVAL(true), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("to_metric_symbol", "no_decimals_on_small_values"), &BigNumber::to_metric_symbol, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("to_metric_name", "no_decimals_on_small_values"), &BigNumber::to_metric_name, DEFVAL(false));

	ClassDB::bind_method(D_METHOD("set_mantissa", "mantissa"), &BigNumber::set_mantissa);
	ClassDB::bind_method(D_METHOD("get_mantissa"), &BigNumber::get_mantissa);
	ClassDB::bind_method(D_METHOD("set_exponent", "exponent"), &BigNumber::set_exponent);
	ClassDB::bind_method(D_METHOD("get_exponent"), &BigNumber::get_exponent);
	ClassDB::bind_method(D_METHOD("normalize"), &BigNumber::normalize);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "mantissa"), "set_mantissa", "get_mantissa");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "exponent"), "set_exponent", "get_exponent");

	ClassDB::bind_method(D_METHOD("is_less_than", "n"), &BigNumber::is_less_than);
	ClassDB::bind_method(D_METHOD("is_equal_to", "n"), &BigNumber::is_equal_to);
	ClassDB::bind_method(D_METHOD("is_greater_than", "n"), &BigNumber::is_greater_than);
	ClassDB::bind_method(D_METHOD("is_less_than_or_equal_to", "n"), &BigNumber::is_less_than_or_equal_to);
	ClassDB::bind_method(D_METHOD("is_greater_than_or_equal_to", "n"), &BigNumber::is_greater_than_or_equal_to);

	ClassDB::bind_method(D_METHOD("plus", "n"), &BigNumber::plus);
	ClassDB::bind_method(D_METHOD("plus_equals", "n"), &BigNumber::plus_equals);
	ClassDB::bind_method(D_METHOD("minus", "n"), &BigNumber::minus);
	ClassDB::bind_method(D_METHOD("minus_equals", "n"), &BigNumber::minus_equals);
	ClassDB::bind_method(D_METHOD("multiply", "n"), &BigNumber::multiply);
	ClassDB::bind_method(D_METHOD("multiply_equals", "n"), &BigNumber::multiply_equals);
	ClassDB::bind_method(D_METHOD("divide", "n"), &BigNumber::divide);
	ClassDB::bind_method(D_METHOD("divide_equals", "n"), &BigNumber::divide_equals);

	ClassDB::bind_method(D_METHOD("mod", "n"), &BigNumber::mod);
	ClassDB::bind_method(D_METHOD("power", "n"), &BigNumber::power);
	ClassDB::bind_method(D_METHOD("power_equals", "n"), &BigNumber::power_equals);
	ClassDB::bind_method(D_METHOD("square_root"), &BigNumber::square_root);
	ClassDB::bind_method(D_METHOD("absolute"), &BigNumber::absolute);
	ClassDB::bind_method(D_METHOD("log10"), &BigNumber::log10);
	ClassDB::bind_method(D_METHOD("ln"), &BigNumber::ln);
	ClassDB::bind_method(D_METHOD("floor_value"), &BigNumber::floor_value);
	ClassDB::bind_method(D_METHOD("to_float"), &BigNumber::to_float);
	ClassDB::bind_method(D_METHOD("to_plain_scientific"), &BigNumber::to_plain_scientific);
}


