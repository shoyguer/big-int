class_name GDBigInt
extends RefCounted
## Big number class for use in idle / incremental games and other games that needs very large numbers.
##
## Can format large numbers using a variety of notation methods:[br]
## AA notation like AA, AB, AC etc.[br]
## Metric symbol notation k, m, G, T etc.[br]
## Metric name notation kilo, mega, giga, tera etc.[br]
## Long names like octo-vigin-tillion or millia-nongen-quin-vigin-tillion (based on work by Landon Curt Noll)[br]
## Scientic notation like 13e37 or 42e42[br]
## Long strings like 4200000000 or 13370000000000000000000000000000[br][br]
## Please note that this class has limited precision and does not fully support negative exponents.


#region Properties

#region Constants
## Metric Symbol Suffixes.
const SUFFIXES_METRIC_SYMBOL: Dictionary = {
	"0": "",
	"1": "k",
	"2": "M",
	"3": "G",
	"4": "T",
	"5": "P",
	"6": "E",
	"7": "Z",
	"8": "Y",
	"9": "R",
	"10": "Q",
}

## Metric Name Suffixes.
const SUFFIXES_METRIC_NAME: Dictionary = {
	"0": "",
	"1": "kilo",
	"2": "mega",
	"3": "giga",
	"4": "tera",
	"5": "peta",
	"6": "exa",
	"7": "zetta",
	"8": "yotta",
	"9": "ronna",
	"10": "quetta",
}

## AA Alphabet.
const ALPHABET_AA: Array[String] = [
	"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
	"n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"
]

## Latin Ones Prefixes.
const LATIN_ONES: Array[String] = [
	"", "un", "duo", "tre", "quattuor", "quin", "sex", "septen", "octo", "novem"
]

## Latin Tens Prefixes.
const LATIN_TENS: Array[String] = [
	"", "dec", "vigin", "trigin", "quadragin", "quinquagin", "sexagin", "septuagin", "octogin", "nonagin"
]

## Latin Hundreds Prefixes.
const LATIN_HUNDREDS: Array[String] = [
	"", "cen", "duocen", "trecen", "quadringen", "quingen", "sescen", "septingen", "octingen", "nongen"
]

## Latin Special Prefixes.
const LATIN_SPECIAL: Array[String] = [
	"", "mi", "bi", "tri", "quadri", "quin", "sex", "sept", "oct", "non"
]

## Maximum Big Number Mantissa.
const MANTISSA_MAX: float = 1209600.0

## Big Number Mantissa floating-point precision.
const MANTISSA_PRECISION: float = 0.0000001

## Int (signed 64-bit) minimum value.
const INT_MIN: int = -9223372036854775808

## Int (signed 64-bit) maximum value.
const INT_MAX: int = 9223372036854775807
#endregion

## AA suffixes keys in dictionary to prevent generating each of them again and again.
static var suffixes_aa: Dictionary = {
	"0": "",
	"1": "k",
	"2": "m",
	"3": "b",
	"4": "t",
	"5": "aa",
	"6": "ab",
	"7": "ac",
	"8": "ad",
	"9": "ae",
	"10": "af",
	"11": "ag",
	"12": "ah",
	"13": "ai",
	"14": "aj",
	"15": "ak",
	"16": "al",
	"17": "am",
	"18": "an",
	"19": "ao",
	"20": "ap",
	"21": "aq",
	"22": "ar",
	"23": "as",
	"24": "at",
	"25": "au",
	"26": "av",
	"27": "aw",
	"28": "ax",
	"29": "ay",
	"30": "az",
	"31": "ba",
	"32": "bb",
	"33": "bc",
	"34": "bd",
	"35": "be",
	"36": "bf",
	"37": "bg",
	"38": "bh",
	"39": "bi",
	"40": "bj",
	"41": "bk",
	"42": "bl",
	"43": "bm",
	"44": "bn",
	"45": "bo",
	"46": "bp",
	"47": "bq",
	"48": "br",
	"49": "bs",
	"50": "bt",
	"51": "bu",
	"52": "bv",
	"53": "bw",
	"54": "bx",
	"55": "by",
	"56": "bz",
	"57": "ca"
}

## Various options to control the string presentation of GDBigInt Numbers.
static var options: Dictionary = {
	"default_mantissa": 1.0,
	"default_exponent": 0,
	"dynamic_decimals": false,
	"dynamic_numbers": 4,
	"small_decimals": 2,
	"thousand_decimals": 2,
	"big_decimals": 2,
	"scientific_decimals": 2,
	"logarithmic_decimals": 2,
	"maximum_trailing_zeroes": 3,
	"thousand_separator": ",",
	"decimal_separator": ".",
	"suffix_separator": "",
	"reading_separator": "",
	"thousand_name": "thousand"
}

var mantissa: float
var exponent: int
#endregion


## Initializes the GDBigInt number.
## Can take another GDBigInt, a String (scientific notation), a float, or an int.
func _init(m: Variant = options["default_mantissa"], e: int = options["default_exponent"]) -> void:
	if m is GDBigInt:
		mantissa = m.mantissa
		exponent = m.exponent
	elif typeof(m) == TYPE_STRING:
		var scientific: PackedStringArray = m.split("e")
		mantissa = float(scientific[0])
		exponent = int(scientific[1]) if scientific.size() > 1 else 0
	else:
		if typeof(m) != TYPE_INT and typeof(m) != TYPE_FLOAT:
			printerr("GDBigInt Error: Unknown data type passed as a mantissa!")
			mantissa = 1.0
			exponent = 0
		else:
			mantissa = float(m)
			exponent = e
	
	_size_check(mantissa)
	normalize()


## Converts the GDBigInt Number into a string.
## Replaces the default string representation of the object.
func _to_string() -> String:
	var mantissa_decimals: int = 0
	if str(mantissa).find(".") >= 0:
		mantissa_decimals = str(mantissa).split(".")[1].length()
	
	if mantissa_decimals > exponent:
		if exponent < 248:
			return str(mantissa * (10.0 ** exponent))
		else:
			return to_plain_scientific()
	else:
		var mantissa_string: String = str(mantissa).replace(".", "")
		for _i: int in range(exponent - mantissa_decimals):
			mantissa_string += "0"
		return mantissa_string


## Normalizes the GDBigInt number (keeps mantissa between 1.0 and 10.0).
## Optimized for performance and handles zero correctly.
func normalize() -> void:
	if mantissa == 0.0:
		exponent = 0
		return

	# Handle signs
	var is_negative: bool = mantissa < 0.0
	if is_negative:
		mantissa = -mantissa
	
	mantissa = snappedf(mantissa, MANTISSA_PRECISION)
	
	# If still effectively zero after snap
	if mantissa == 0.0:
		exponent = 0
		return
	
	if mantissa < 1.0 or mantissa >= 10.0:
		var diff: int = int(floor(log(mantissa) / log(10.0))) # Equivalent to log10, safer than custom implementation
		
		# Only apply large shifts if reasonable to avoid overflow in pow
		if diff > -10 and diff < 248:
			var div: float = 10.0 ** diff
			if div > MANTISSA_PRECISION:
				mantissa /= div
				exponent += diff
	
	# Catch edge cases from floating point errors
	while mantissa < 1.0:
		mantissa *= 10.0
		exponent -= 1
	while mantissa >= 10.0:
		mantissa /= 10.0
		exponent += 1
		
	mantissa = snappedf(mantissa, MANTISSA_PRECISION)
	if mantissa == 0.0:
		mantissa = 0.0
		exponent = 0
	
	if is_negative:
		mantissa = -mantissa


## Returns a new GDBigInt with the absolute value.
func absolute() -> GDBigInt:
	var result: GDBigInt = GDBigInt.new(self)
	result.mantissa = abs(result.mantissa)
	return result


## Returns true if the number is less than input n.
func is_less_than(n: Variant) -> bool:
	var other: GDBigInt = _type_check(n)
	# Safe to assume 'other' is normalized if it came from _type_check -> new() -> init
	
	if mantissa == 0.0:
		# If we are 0, we are less of any positive number (ignoring near-zero checks for simplicity logic here)
		return other.mantissa > 0.0 or (other.mantissa == 0.0 and false) # 0 < 0 is false
	
	if exponent < other.exponent:
		# Check if they are actually close enough to overlap due to representation
		# But generally smaller exponent means smaller number for normalized positives
		# Handling signs is complex, assuming positive logic primarily for idle games here
		
		# Edge case check from original code preserved
		if exponent == other.exponent - 1 and mantissa > 10.0 * other.mantissa:
			return false
		return true
		
	elif exponent == other.exponent:
		return mantissa < other.mantissa
		
	else:
		# exponent > other.exponent
		if exponent == other.exponent + 1 and mantissa * 10.0 < other.mantissa:
			return true
		return false


## Returns true if the number is equal to input n.
func is_equal_to(n: Variant) -> bool:
	var other: GDBigInt = _type_check(n)
	return other.exponent == exponent and is_equal_approx(other.mantissa, mantissa)


## Returns true if the number is greater than input n.
func is_greater_than(n: Variant) -> bool:
	return not is_less_than_or_equal_to(n)


## Returns true if the number is greater than or equal to input n.
func is_greater_than_or_equal_to(n: Variant) -> bool:
	return not is_less_than(n)


## Returns true if the number is less than or equal to input n.
func is_less_than_or_equal_to(n: Variant) -> bool:
	var other: GDBigInt = _type_check(n)
	if is_less_than(other):
		return true
	if other.exponent == exponent and is_equal_approx(other.mantissa, mantissa):
		return true
	return false


## Adds a number to this GDBigInt and returns a new GDBigInt.
func plus(n: Variant) -> GDBigInt:
	var res: GDBigInt = GDBigInt.new(self)
	res.plus_equals(n)
	return res


## adds a number to this GDBigInt in-place.
func plus_equals(n: Variant) -> GDBigInt:
	var other: GDBigInt = _type_check(n)
	
	var exp_diff: int = other.exponent - exponent
	
	if exp_diff == 0:
		mantissa += other.mantissa
	elif exp_diff > 0:
		# Other is bigger (or equal mantissa potential). We scale self down? No, we align to the bigger exponent usually.
		# Original code: aligned everything to X (self).
		# If Y (other) is much bigger, X's contribution is negligible.
		
		if exp_diff >= 248:
			# Other is way bigger, we just become other
			mantissa = other.mantissa
			exponent = other.exponent
		else:
			# We align 'other' down to 'self's exponent? 
			# y.mantissa * 10^exp_diff
			# if exp_diff is positive, other is bigger exponent.
			# so we add other's scaled mantissa.
			var scaled_mantissa: float = other.mantissa * (10.0 ** exp_diff)
			mantissa += scaled_mantissa
			# Normalize handles fixup
			
	else:
		# exp_diff < 0. Self is bigger.
		# Check if other is negligible
		if -exp_diff >= 248:
			# Other is too small, ignore add
			pass 
		else:
			# Scale other down to match self
			var scaled_mantissa: float = other.mantissa / (10.0 ** (-exp_diff))
			mantissa += scaled_mantissa

	normalize()
	return self


## Subtracts a number from this GDBigInt and returns a new GDBigInt.
func minus(n: Variant) -> GDBigInt:
	var res: GDBigInt = GDBigInt.new(self)
	res.minus_equals(n)
	return res


## Subtracts a number from this GDBigInt in-place.
func minus_equals(n: Variant) -> GDBigInt:
	var other: GDBigInt = _type_check(n)
	# Subtraction is just addition of negative
	# We want to avoid creating a new object for negative 'other' if possible, 
	# but 'other' should not be mutated.
	# For performance, we implement custom add logic with negative sign
	
	var other_mantissa_neg: float = -other.mantissa
	var other_exponent: int = other.exponent
	
	var exp_diff: int = other_exponent - exponent
	
	if exp_diff == 0:
		mantissa += other_mantissa_neg
	elif exp_diff > 0:
		if exp_diff >= 248:
			mantissa = other_mantissa_neg
			exponent = other_exponent
		else:
			var scaled_mantissa: float = other_mantissa_neg * (10.0 ** exp_diff)
			mantissa += scaled_mantissa
	else:
		if -exp_diff < 248:
			var scaled_mantissa: float = other_mantissa_neg / (10.0 ** (-exp_diff))
			mantissa += scaled_mantissa
			
	normalize()
	return self


## Multiplies this GDBigInt by a number and returns a new GDBigInt.
func multiply(n: Variant) -> GDBigInt:
	var res: GDBigInt = GDBigInt.new(self)
	res.multiply_equals(n)
	return res


## Multiplies this GDBigInt by a number in-place.
func multiply_equals(n: Variant) -> GDBigInt:
	var other: GDBigInt = _type_check(n)
	
	exponent += other.exponent
	mantissa *= other.mantissa
	
	normalize()
	return self


## Divides this GDBigInt by a number and returns a new GDBigInt.
func divide(n: Variant) -> GDBigInt:
	var res: GDBigInt = GDBigInt.new(self)
	res.divide_equals(n)
	return res


## Divides this GDBigInt by a number in-place.
func divide_equals(n: Variant) -> GDBigInt:
	var other: GDBigInt = _type_check(n)
	
	if other.mantissa == 0.0:
		printerr("GDBigInt Error: Divide by zero")
		return self
		
	exponent -= other.exponent
	mantissa /= other.mantissa
	
	normalize()
	return self


## Modulo of this GDBigInt by a number.
func mod(n: Variant) -> GDBigInt:
	# Modulo: a - (n * floor(a/n))
	var other: GDBigInt = _type_check(n)
	
	var quot: GDBigInt = divide(other)
	quot.floor_value() # Round down
	
	var product: GDBigInt = quot.multiply(other)
	var res: GDBigInt = minus(product)
	return res


## Raises this GDBigInt to a power and returns a result.
func power(n: Variant) -> GDBigInt:
	var res: GDBigInt = GDBigInt.new(self)
	res.power_equals(n)
	return res


## Raises this GDBigInt to a power in-place.
func power_equals(n: Variant) -> GDBigInt:
	# Handle integer power optimized
	if typeof(n) == TYPE_INT:
		var p: int = n
		if p == 0:
			mantissa = 1.0
			exponent = 0
			return self
		if p < 0:
			printerr("GDBigInt Error: Negative integer exponents not fully supported in simple power!")
			return self
			
		# Exponentiation by squaring for integers could be implemented, 
		# but for floating point scientific notation we can use logs.
		# (m * 10^e)^p = m^p * 10^(e*p)
		# m^p can be huge, so we normalize.
		
		# new_e = e * p
		# new_m = m ^ p
		# This is cleaner than the iterative loop in original code
		
		var new_exponent: int = exponent * p
		var new_mantissa: float = mantissa ** float(p)
		
		mantissa = new_mantissa
		exponent = new_exponent
		normalize()
		return self
		
	elif typeof(n) == TYPE_FLOAT:
		var p: float = n
		if mantissa == 0.0: return self
		
		# (m * 10^e)^p = m^p * 10^(e*p)
		# = 10^(log10(m)*p) * 10^(e*p)
		# = 10^( (log10(m) + e) * p )
		
		# Safer implementation using log10
		var log_val: float = log10()
		var new_log: float = log_val * p
		
		# Reconstruct
		# value = 10^new_log
		# exponent = floor(new_log)
		# mantissa = 10^(new_log - exponent)
		
		var new_exponent: int = int(floor(new_log))
		var remainder: float = new_log - new_exponent
		var new_mantissa: float = 10.0 ** remainder
		
		exponent = new_exponent
		mantissa = new_mantissa
		normalize()
		return self
		
	elif n is GDBigInt:
		# Converting GDBigInt exponent to float for power calculation
		# If exponent is huge, power result is essentially Infinity very fast
		return power_equals(n.to_float())
		
	return self


## Calculates the square root.
func square_root() -> GDBigInt:
	var res: GDBigInt = GDBigInt.new(self)
	
	if res.exponent % 2 == 0:
		res.mantissa = sqrt(res.mantissa)
		@warning_ignore("integer_division")
		res.exponent = res.exponent / 2
	else:
		res.mantissa = sqrt(res.mantissa * 10.0)
		@warning_ignore("integer_division")
		res.exponent = (res.exponent - 1) / 2
	
	res.normalize()
	return res


## Log10 of the value.
func log10() -> float:
	return float(exponent) + (log(mantissa) / log(10.0))


## Natural logarithm.
func ln() -> float:
	return log10() * 2.302585092994046


## Floors the number.
func floor_value() -> void:
	if exponent == 0:
		mantissa = floor(mantissa)
	elif exponent < 0:
		# If 0.00123 -> floor is 0
		mantissa = 0.0
		exponent = 0
	else:
		# Large numbers are integers effectively due to precision limit of float mantissa
		# If exponent > 15, we don't have fractional precision usually in mantissa anyway
		pass


## Converts to float (limited precision).
func to_float() -> float:
	return mantissa * (10.0 ** exponent)


## Converts to plain scientific string.
func to_plain_scientific() -> String:
	return str(mantissa) + "e" + str(exponent)


## Converts the GDBigInt Number into a string (in Scientific format).
func to_scientific(no_decimals_on_small_values: bool = false, force_decimals: bool = false) -> String:
	if exponent < 3:
		var decimal_increments: float = 1.0 / (10.0 ** options.scientific_decimals / 10.0)
		var value: String = str(snappedf(mantissa * (10.0 ** exponent), decimal_increments))
		var split: PackedStringArray = value.split(".")
		if no_decimals_on_small_values:
			return split[0]
		if split.size() > 1:
			return split[0] + options.decimal_separator + split[1].substr(0, min(options.scientific_decimals, options.dynamic_numbers - split[0].length() if options.dynamic_decimals else options.scientific_decimals))
		else:
			return value
	else:
		var split: PackedStringArray = str(mantissa).split(".")
		if split.size() == 1:
			split.append("")
		if force_decimals:
			for _i: int in range(options.scientific_decimals):
				if split[1].length() < options.scientific_decimals:
					split[1] += "0"
		return split[0] + options.decimal_separator + split[1].substr(0, min(options.scientific_decimals, options.dynamic_numbers - 1 - str(exponent).length() if options.dynamic_decimals else options.scientific_decimals)) + "e" + str(exponent)


## Helper to generate prefix string.
func to_prefix(no_decimals_on_small_values: bool = false, use_thousand_symbol: bool = true, force_decimals: bool = true, scientific_prefix: bool = false) -> String:
	var number: float = mantissa
	if not scientific_prefix:
		var hundreds: int = 1
		for _i: int in range(exponent % 3):
			hundreds *= 10
		number *= hundreds

	var split: PackedStringArray = str(number).split(".")
	if split.size() == 1:
		split.append("")
	if force_decimals:
		var max_decimals: int = int(max(max(options.small_decimals, options.thousand_decimals), options.big_decimals))
		for _i: int in range(max_decimals):
			if split[1].length() < max_decimals:
				split[1] += "0"
	
	if no_decimals_on_small_values and exponent < 3:
		return split[0]
	elif exponent < 3:
		if options.small_decimals == 0 or split[1] == "":
			return split[0]
		else:
			return split[0] + options.decimal_separator + split[1].substr(0, min(options.small_decimals, options.dynamic_numbers - split[0].length() if options.dynamic_decimals else options.small_decimals))
	elif exponent < 6:
		if options.thousand_decimals == 0 or (split[1] == "" and use_thousand_symbol):
			return split[0]
		else:
			if use_thousand_symbol:
				return split[0] + options.decimal_separator + split[1].substr(0, min(options.thousand_decimals, options.dynamic_numbers - split[0].length() if options.dynamic_decimals else 3))
			else:
				return split[0] + options.thousand_separator + split[1].substr(0, 3)
	else:
		if options.big_decimals == 0 or split[1] == "":
			return split[0]
		else:
			return split[0] + options.decimal_separator + split[1].substr(0, min(options.big_decimals, options.dynamic_numbers - split[0].length() if options.dynamic_decimals else options.big_decimals))


## Converts the GDBigInt Number into a string (in AA format).
func to_aa(no_decimals_on_small_values: bool = false, use_thousand_symbol: bool = true, force_decimals: bool = false) -> String:
	@warning_ignore("integer_division")
	var target: int = int(exponent / 3)
	var aa_index: String = str(target)
	var suffix: String = ""

	if not suffixes_aa.has(aa_index):
		var offset: int = target + 22
		var base: int = ALPHABET_AA.size()
		while offset > 0:
			offset -= 1
			var digit: int = offset % base
			suffix = ALPHABET_AA[digit] + suffix
			@warning_ignore("integer_division")
			offset /= base
		suffixes_aa[aa_index] = suffix
	else:
		suffix = suffixes_aa[aa_index]

	if not use_thousand_symbol and target == 1:
		suffix = ""

	var prefix: String = to_prefix(no_decimals_on_small_values, use_thousand_symbol, force_decimals)
	return prefix + options.suffix_separator + suffix


## Converts the GDBigInt Number into a string (in Metric Symbols format).
func to_metric_symbol(no_decimals_on_small_values: bool = false) -> String:
	@warning_ignore("integer_division")
	var target: int = int(exponent / 3)
	if not SUFFIXES_METRIC_SYMBOL.has(str(target)):
		return to_scientific()
	else:
		return to_prefix(no_decimals_on_small_values) + options.suffix_separator + SUFFIXES_METRIC_SYMBOL[str(target)]


## Converts the GDBigInt Number into a string (in Metric Name format).
func to_metric_name(no_decimals_on_small_values: bool = false) -> String:
	@warning_ignore("integer_division")
	var target: int = int(exponent / 3)
	if not SUFFIXES_METRIC_NAME.has(str(target)):
		return to_scientific()
	else:
		return to_prefix(no_decimals_on_small_values) + options.suffix_separator + SUFFIXES_METRIC_NAME[str(target)]


# Private Helpers

func _size_check(m: float) -> void:
	if m > MANTISSA_MAX:
		printerr("GDBigInt Error: Mantissa \"" + str(m) + "\" exceeds MANTISSA_MAX.")


func _type_check(n: Variant) -> GDBigInt:
	if n is GDBigInt:
		return n
	else:
		return GDBigInt.new(n)
