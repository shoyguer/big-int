extends Control

const Big = preload("res://big.gd")
const GDBigInt = preload("res://gd_big_int.gd")
const ITERATIONS = 100000

func _ready() -> void:
	# Add a small delay to let the UI draw first so we don't freeze immediately on startup
	await get_tree().create_timer(0.1).timeout
	
	benchmark_instantiation()
	await get_tree().process_frame
	benchmark_addition()
	await get_tree().process_frame
	benchmark_subtraction()
	await get_tree().process_frame
	benchmark_multiplication()
	await get_tree().process_frame
	benchmark_division()
	await get_tree().process_frame
	benchmark_modulo()
	await get_tree().process_frame
	benchmark_power()
	await get_tree().process_frame
	benchmark_sqrt()
	await get_tree().process_frame
	benchmark_comparison()
	await get_tree().process_frame
	
	benchmark_formatting_scientific()
	await get_tree().process_frame
	benchmark_formatting_prefix()
	await get_tree().process_frame
	benchmark_formatting_aa()
	await get_tree().process_frame
	benchmark_formatting_metric_symbol()
	await get_tree().process_frame
	benchmark_formatting_metric_name()

func update_ui_row(prefix: String, time_cpp: int, time_old: int, time_new: int) -> void:
	var lbl_cpp = get_node("%" + "Res_" + prefix + "_CPP") as Label
	var lbl_old = get_node("%" + "Res_" + prefix + "_Old") as Label
	var lbl_new = get_node("%" + "Res_" + prefix + "_New") as Label
	var lbl_ratio_old = get_node("%" + "Res_" + prefix + "_RatioOld") as Label
	var lbl_ratio_new = get_node("%" + "Res_" + prefix + "_RatioNew") as Label
	
	if lbl_cpp:
		lbl_cpp.text = "%d us (%.2f ms)" % [time_cpp, time_cpp / 1000.0]
	
	if lbl_old:
		lbl_old.text = "%d us (%.2f ms)" % [time_old, time_old / 1000.0]
		
	if lbl_new:
		lbl_new.text = "%d us (%.2f ms)" % [time_new, time_new / 1000.0]
	
	if lbl_ratio_old:
		if time_cpp > 0:
			var ratio = float(time_old) / float(time_cpp)
			lbl_ratio_old.text = "%.2fx" % ratio
			if ratio > 1.0:
				lbl_ratio_old.add_theme_color_override("font_color", Color.GREEN)
			else:
				lbl_ratio_old.add_theme_color_override("font_color", Color.RED)
		else:
			lbl_ratio_old.text = "Inf"

	if lbl_ratio_new:
		if time_cpp > 0:
			var ratio = float(time_new) / float(time_cpp)
			lbl_ratio_new.text = "%.2fx" % ratio
			if ratio > 1.0:
				lbl_ratio_new.add_theme_color_override("font_color", Color.GREEN)
			else:
				lbl_ratio_new.add_theme_color_override("font_color", Color.RED)
		else:
			lbl_ratio_new.text = "Inf"

func benchmark_instantiation() -> void:
	var time_cpp = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _b = BigInt.new()
	time_cpp = Time.get_ticks_usec() - time_cpp
	
	var time_old = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _b = GDBigInt.new()
	time_old = Time.get_ticks_usec() - time_old

	var time_new = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _b = Big.new()
	time_new = Time.get_ticks_usec() - time_new
	
	update_ui_row("Inst", time_cpp, time_old, time_new)

func benchmark_addition() -> void:
	var a_cpp = BigInt.new()
	a_cpp.mantissa = 1.0
	a_cpp.exponent = 50
	var b_cpp = BigInt.new()
	b_cpp.mantissa = 5.0
	b_cpp.exponent = 40
	
	var a_old = GDBigInt.new(1.0, 50)
	var b_old = GDBigInt.new(5.0, 40)
	
	var a_new = Big.new(1.0, 50)
	var b_new = Big.new(5.0, 40)
	
	var time_cpp = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _c = a_cpp.plus(b_cpp)
	time_cpp = Time.get_ticks_usec() - time_cpp
	
	var time_old = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _c = a_old.plus(b_old)
	time_old = Time.get_ticks_usec() - time_old

	var time_new = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _c = Big.add(a_new, b_new)
	time_new = Time.get_ticks_usec() - time_new
	
	update_ui_row("Add", time_cpp, time_old, time_new)

func benchmark_subtraction() -> void:
	var a_cpp = BigInt.new()
	a_cpp.mantissa = 1.0
	a_cpp.exponent = 50
	var b_cpp = BigInt.new()
	b_cpp.mantissa = 5.0
	b_cpp.exponent = 40
	
	var a_old = GDBigInt.new(1.0, 50)
	var b_old = GDBigInt.new(5.0, 40)
	
	var a_new = Big.new(1.0, 50)
	var b_new = Big.new(5.0, 40)
	
	var time_cpp = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _c = a_cpp.minus(b_cpp)
	time_cpp = Time.get_ticks_usec() - time_cpp
	
	var time_old = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _c = a_old.minus(b_old)
	time_old = Time.get_ticks_usec() - time_old

	var time_new = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _c = Big.subtract(a_new, b_new)
	time_new = Time.get_ticks_usec() - time_new
	
	update_ui_row("Sub", time_cpp, time_old, time_new)

func benchmark_multiplication() -> void:
	var a_cpp = BigInt.new()
	a_cpp.mantissa = 1.5
	a_cpp.exponent = 10
	var b_cpp = BigInt.new()
	b_cpp.mantissa = 2.0
	b_cpp.exponent = 5
	
	var a_old = GDBigInt.new(1.5, 10)
	var b_old = GDBigInt.new(2.0, 5)
	
	var a_new = Big.new(1.5, 10)
	var b_new = Big.new(2.0, 5)
	
	var time_cpp = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _c = a_cpp.multiply(b_cpp)
	time_cpp = Time.get_ticks_usec() - time_cpp
	
	var time_old = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _c = a_old.multiply(b_old)
	time_old = Time.get_ticks_usec() - time_old

	var time_new = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _c = Big.times(a_new, b_new)
	time_new = Time.get_ticks_usec() - time_new
	
	update_ui_row("Mul", time_cpp, time_old, time_new)

func benchmark_division() -> void:
	var a_cpp = BigInt.new()
	a_cpp.mantissa = 8.0
	a_cpp.exponent = 20
	var b_cpp = BigInt.new()
	b_cpp.mantissa = 2.0
	b_cpp.exponent = 5

	var a_old = GDBigInt.new(8.0, 20)
	var b_old = GDBigInt.new(2.0, 5)
	
	var a_new = Big.new(8.0, 20)
	var b_new = Big.new(2.0, 5)
	
	var time_cpp = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _c = a_cpp.divide(b_cpp)
	time_cpp = Time.get_ticks_usec() - time_cpp
	
	var time_old = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _c = a_old.divide(b_old)
	time_old = Time.get_ticks_usec() - time_old

	var time_new = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _c = Big.division(a_new, b_new)
	time_new = Time.get_ticks_usec() - time_new
	
	update_ui_row("Div", time_cpp, time_old, time_new)

func benchmark_modulo() -> void:
	var a_cpp = BigInt.new()
	a_cpp.mantissa = 5.0
	a_cpp.exponent = 1
	var b_cpp = 4.0
	
	var a_old = GDBigInt.new(5.0, 1)
	var b_old = 4.0

	var a_new = Big.new(5.0, 1)
	var b_new = 4.0
	
	var time_cpp = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _c = a_cpp.mod(b_cpp)
	time_cpp = Time.get_ticks_usec() - time_cpp
	
	var time_old = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _c = a_old.mod(b_old)
	time_old = Time.get_ticks_usec() - time_old

	var time_new = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _c = Big.modulo(a_new, b_new)
	time_new = Time.get_ticks_usec() - time_new
	
	update_ui_row("Mod", time_cpp, time_old, time_new)

func benchmark_power() -> void:
	var a_cpp = BigInt.new()
	a_cpp.mantissa = 2.0
	a_cpp.exponent = 3
	var p = 5
	
	var a_old = GDBigInt.new(2.0, 3)
	
	var a_new = Big.new(2.0, 3)
	
	var time_cpp = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _c = a_cpp.power(p)
	time_cpp = Time.get_ticks_usec() - time_cpp
	
	var time_old = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _c = a_old.power(p)
	time_old = Time.get_ticks_usec() - time_old

	var time_new = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _c = Big.powers(a_new, p)
	time_new = Time.get_ticks_usec() - time_new
	
	update_ui_row("Pow", time_cpp, time_old, time_new)

func benchmark_sqrt() -> void:
	var a_cpp = BigInt.new()
	a_cpp.mantissa = 9.0
	a_cpp.exponent = 10
	
	var a_old = GDBigInt.new(9.0, 10)

	var a_new = Big.new(9.0, 10)
	
	var time_cpp = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _c = a_cpp.square_root()
	time_cpp = Time.get_ticks_usec() - time_cpp
	
	var time_old = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _c = a_old.square_root()
	time_old = Time.get_ticks_usec() - time_old

	var time_new = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _c = Big.root(a_new)
	time_new = Time.get_ticks_usec() - time_new
	
	update_ui_row("Sqrt", time_cpp, time_old, time_new)

func benchmark_comparison() -> void:
	var a_cpp = BigInt.new()
	a_cpp.mantissa = 1.0
	a_cpp.exponent = 50
	var b_cpp = BigInt.new()
	b_cpp.mantissa = 5.0
	b_cpp.exponent = 50
	
	var a_old = GDBigInt.new(1.0, 50)
	var b_old = GDBigInt.new(5.0, 50)

	var a_new = Big.new(1.0, 50)
	var b_new = Big.new(5.0, 50)
	
	var time_cpp = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _c = a_cpp.is_less_than(b_cpp)
	time_cpp = Time.get_ticks_usec() - time_cpp
	
	var time_old = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _c = a_old.is_less_than(b_old)
	time_old = Time.get_ticks_usec() - time_old

	var time_new = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _c = a_new.isLessThan(b_new)
	time_new = Time.get_ticks_usec() - time_new
	
	update_ui_row("Comp", time_cpp, time_old, time_new)

func benchmark_formatting_scientific() -> void:
	var a_cpp = BigInt.new()
	a_cpp.mantissa = 1.23456
	a_cpp.exponent = 50
	
	var a_old = GDBigInt.new(1.23456, 50)
	var a_new = Big.new(1.23456, 50)
	
	var time_cpp = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _s = a_cpp.to_scientific()
	time_cpp = Time.get_ticks_usec() - time_cpp
	
	var time_old = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _s = a_old.to_scientific()
	time_old = Time.get_ticks_usec() - time_old

	var time_new = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _s = a_new.toScientific()
	time_new = Time.get_ticks_usec() - time_new
	
	update_ui_row("FmtSci", time_cpp, time_old, time_new)

func benchmark_formatting_prefix() -> void:
	var a_cpp = BigInt.new()
	a_cpp.mantissa = 1.23456
	a_cpp.exponent = 50
	
	var a_old = GDBigInt.new(1.23456, 50)
	var a_new = Big.new(1.23456, 50)
	
	var time_cpp = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _s = a_cpp.to_prefix()
	time_cpp = Time.get_ticks_usec() - time_cpp
	
	var time_old = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _s = a_old.to_prefix()
	time_old = Time.get_ticks_usec() - time_old

	var time_new = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _s = a_new.toPrefix()
	time_new = Time.get_ticks_usec() - time_new
	
	update_ui_row("FmtPre", time_cpp, time_old, time_new)

func benchmark_formatting_aa() -> void:
	var a_cpp = BigInt.new()
	a_cpp.mantissa = 1.23456
	a_cpp.exponent = 50
	
	var a_old = GDBigInt.new(1.23456, 50)
	var a_new = Big.new(1.23456, 50)
	
	var time_cpp = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _s = a_cpp.to_aa()
	time_cpp = Time.get_ticks_usec() - time_cpp
	
	var time_old = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _s = a_old.to_aa()
	time_old = Time.get_ticks_usec() - time_old

	var time_new = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _s = a_new.toAA()
	time_new = Time.get_ticks_usec() - time_new
	
	update_ui_row("FmtAA", time_cpp, time_old, time_new)

func benchmark_formatting_metric_symbol() -> void:
	var a_cpp = BigInt.new()
	a_cpp.mantissa = 1.23456
	a_cpp.exponent = 12
	
	var a_old = GDBigInt.new(1.23456, 12)
	var a_new = Big.new(1.23456, 12)
	
	var time_cpp = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _s = a_cpp.to_metric_symbol()
	time_cpp = Time.get_ticks_usec() - time_cpp
	
	var time_old = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _s = a_old.to_metric_symbol()
	time_old = Time.get_ticks_usec() - time_old

	var time_new = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _s = a_new.toMetricSymbol()
	time_new = Time.get_ticks_usec() - time_new
	
	update_ui_row("FmtSym", time_cpp, time_old, time_new)

func benchmark_formatting_metric_name() -> void:
	var a_cpp = BigInt.new()
	a_cpp.mantissa = 1.23456
	a_cpp.exponent = 12
	
	var a_old = GDBigInt.new(1.23456, 12)
	var a_new = Big.new(1.23456, 12)
	
	var time_cpp = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _s = a_cpp.to_metric_name()
	time_cpp = Time.get_ticks_usec() - time_cpp
	
	var time_old = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _s = a_old.to_metric_name()
	time_old = Time.get_ticks_usec() - time_old

	var time_new = Time.get_ticks_usec()
	for i in range(ITERATIONS):
		var _s = a_new.toMetricName()
	time_new = Time.get_ticks_usec() - time_new
	
	update_ui_row("FmtNam", time_cpp, time_old, time_new)
