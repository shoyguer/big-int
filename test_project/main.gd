extends Control
## Benchmark script for comparing BigNumber implementations.
##
## Compares the performance of the new C++ GDExtension BigNumber,
## and another pure GDScript Big implementation.


## Number of iterations for each benchmark test.
const ITERATIONS: int = 100000


func _ready() -> void:
	setup_table_style()

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
	await get_tree().process_frame
	benchmark_is_equal()
	await get_tree().process_frame
	benchmark_absolute()
	await get_tree().process_frame
	benchmark_log10()
	await get_tree().process_frame
	benchmark_ln()
	await get_tree().process_frame
	benchmark_floor()
	await get_tree().process_frame
	benchmark_to_float()
	await get_tree().process_frame
	benchmark_formatting_plain()


## Updates the UI with the benchmark results.
func update_ui_row(prefix: String, time_cpp: int, time_new: int) -> void:
	var lbl_cpp: Label = get_node("%" + "Res_" + prefix + "_CPP") as Label
	var lbl_new: Label = get_node("%" + "Res_" + prefix + "_New") as Label
	var lbl_ratio_new: Label = get_node("%" + "Res_" + prefix + "_RatioNew") as Label

	if lbl_cpp:
		lbl_cpp.text = "%.2f ms" % [time_cpp / 1000.0]

	if lbl_new:
		lbl_new.text = "%.2f ms" % [time_new / 1000.0]

	if lbl_ratio_new:
		if time_cpp > 0:
			var ratio: float = float(time_new) / float(time_cpp)
			lbl_ratio_new.text = "%.2f x" % ratio
			if ratio > 1.0:
				lbl_ratio_new.add_theme_color_override("font_color", Color.GREEN)
				lbl_ratio_new.text = lbl_ratio_new.text + " Faster"
			else:
				lbl_ratio_new.add_theme_color_override("font_color", Color.RED)
				lbl_ratio_new.text = lbl_ratio_new.text + " Slower"
		else:
			lbl_ratio_new.text = "Inf"


## Benchmarks instantiation performance.
func benchmark_instantiation() -> void:
	var time_cpp: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _b: BigNumber = BigNumber.new()
	time_cpp = Time.get_ticks_usec() - time_cpp

	var time_new: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _b: Big = Big.new()
	time_new = Time.get_ticks_usec() - time_new

	update_ui_row("Inst", time_cpp, time_new)


## Benchmarks addition performance.
func benchmark_addition() -> void:
	var a_cpp: BigNumber = BigNumber.new()
	a_cpp.mantissa = 1.0
	a_cpp.exponent = 50
	var b_cpp: BigNumber = BigNumber.new()
	b_cpp.mantissa = 5.0
	b_cpp.exponent = 40

	var a_new: Big = Big.new(1.0, 50)
	var b_new: Big = Big.new(5.0, 40)

	var time_cpp: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _c: BigNumber = a_cpp.plus(b_cpp)
	time_cpp = Time.get_ticks_usec() - time_cpp

	var time_new: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _c: Big = Big.add(a_new, b_new)
	time_new = Time.get_ticks_usec() - time_new

	update_ui_row("Add", time_cpp, time_new)


## Benchmarks subtraction performance.
func benchmark_subtraction() -> void:
	var a_cpp: BigNumber = BigNumber.new()
	a_cpp.mantissa = 1.0
	a_cpp.exponent = 50
	var b_cpp: BigNumber = BigNumber.new()
	b_cpp.mantissa = 5.0
	b_cpp.exponent = 40

	var a_new: Big = Big.new(1.0, 50)
	var b_new: Big = Big.new(5.0, 40)

	var time_cpp: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _c: BigNumber = a_cpp.minus(b_cpp)
	time_cpp = Time.get_ticks_usec() - time_cpp

	var time_new: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _c: Big = Big.subtract(a_new, b_new)
	time_new = Time.get_ticks_usec() - time_new

	update_ui_row("Sub", time_cpp, time_new)


## Benchmarks multiplication performance.
func benchmark_multiplication() -> void:
	var a_cpp: BigNumber = BigNumber.new()
	a_cpp.mantissa = 1.5
	a_cpp.exponent = 10
	var b_cpp: BigNumber = BigNumber.new()
	b_cpp.mantissa = 2.0
	b_cpp.exponent = 5

	var a_new: Big = Big.new(1.5, 10)
	var b_new: Big = Big.new(2.0, 5)

	var time_cpp: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _c: BigNumber = a_cpp.multiply(b_cpp)
	time_cpp = Time.get_ticks_usec() - time_cpp

	var time_new: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _c: Big = Big.times(a_new, b_new)
	time_new = Time.get_ticks_usec() - time_new

	update_ui_row("Mul", time_cpp, time_new)


## Benchmarks division performance.
func benchmark_division() -> void:
	var a_cpp: BigNumber = BigNumber.new()
	a_cpp.mantissa = 8.0
	a_cpp.exponent = 20
	var b_cpp: BigNumber = BigNumber.new()
	b_cpp.mantissa = 2.0
	b_cpp.exponent = 5

	var a_new: Big = Big.new(8.0, 20)
	var b_new: Big = Big.new(2.0, 5)

	var time_cpp: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _c: BigNumber = a_cpp.divide(b_cpp)
	time_cpp = Time.get_ticks_usec() - time_cpp

	var time_new: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _c: Big = Big.division(a_new, b_new)
	time_new = Time.get_ticks_usec() - time_new

	update_ui_row("Div", time_cpp, time_new)


## Benchmarks modulo performance.
func benchmark_modulo() -> void:
	var a_cpp: BigNumber = BigNumber.new()
	a_cpp.mantissa = 5.0
	a_cpp.exponent = 1
	var b_cpp: float = 4.0

	var a_new: Big = Big.new(5.0, 1)
	var b_new: float = 4.0

	var time_cpp: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _c: BigNumber = a_cpp.mod(b_cpp)
	time_cpp = Time.get_ticks_usec() - time_cpp

	var time_new: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _c: Big = Big.modulo(a_new, b_new)
	time_new = Time.get_ticks_usec() - time_new

	update_ui_row("Mod", time_cpp, time_new)


## Benchmarks power performance.
func benchmark_power() -> void:
	var a_cpp: BigNumber = BigNumber.new()
	a_cpp.mantissa = 2.0
	a_cpp.exponent = 3
	var p: int = 5

	var a_new: Big = Big.new(2.0, 3)

	var time_cpp: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _c: BigNumber = a_cpp.power(p)
	time_cpp = Time.get_ticks_usec() - time_cpp

	var time_new: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _c: Big = Big.powers(a_new, p)
	time_new = Time.get_ticks_usec() - time_new

	update_ui_row("Pow", time_cpp, time_new)


## Benchmarks square root performance.
func benchmark_sqrt() -> void:
	var a_cpp: BigNumber = BigNumber.new()
	a_cpp.mantissa = 9.0
	a_cpp.exponent = 10

	var a_new: Big = Big.new(9.0, 10)

	var time_cpp: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _c: BigNumber = a_cpp.square_root()
	time_cpp = Time.get_ticks_usec() - time_cpp

	var time_new: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _c: Big = Big.root(a_new)
	time_new = Time.get_ticks_usec() - time_new

	update_ui_row("Sqrt", time_cpp, time_new)


## Benchmarks comparison performance.
func benchmark_comparison() -> void:
	var a_cpp: BigNumber = BigNumber.new()
	a_cpp.mantissa = 1.0
	a_cpp.exponent = 50
	var b_cpp: BigNumber = BigNumber.new()
	b_cpp.mantissa = 5.0
	b_cpp.exponent = 50

	var a_new: Big = Big.new(1.0, 50)
	var b_new: Big = Big.new(5.0, 50)

	var time_cpp: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _c: bool = a_cpp.is_less_than(b_cpp)
	time_cpp = Time.get_ticks_usec() - time_cpp

	var time_new: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _c: bool = a_new.isLessThan(b_new)
	time_new = Time.get_ticks_usec() - time_new

	update_ui_row("Comp", time_cpp, time_new)


## Benchmarks scientific formatting performance.
func benchmark_formatting_scientific() -> void:
	var a_cpp: BigNumber = BigNumber.new()
	a_cpp.mantissa = 1.23456
	a_cpp.exponent = 50

	var a_new: Big = Big.new(1.23456, 50)

	var time_cpp: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _s: String = a_cpp.to_scientific()
	time_cpp = Time.get_ticks_usec() - time_cpp

	var time_new: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _s: String = a_new.toScientific()
	time_new = Time.get_ticks_usec() - time_new

	update_ui_row("FmtSci", time_cpp, time_new)


## Benchmarks prefix formatting performance.
func benchmark_formatting_prefix() -> void:
	var a_cpp: BigNumber = BigNumber.new()
	a_cpp.mantissa = 1.23456
	a_cpp.exponent = 50

	var a_new: Big = Big.new(1.23456, 50)

	var time_cpp: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _s: String = a_cpp.to_prefix()
	time_cpp = Time.get_ticks_usec() - time_cpp

	var time_new: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _s: String = a_new.toPrefix()
	time_new = Time.get_ticks_usec() - time_new

	update_ui_row("FmtPre", time_cpp, time_new)


## Benchmarks AA formatting performance.
func benchmark_formatting_aa() -> void:
	var a_cpp: BigNumber = BigNumber.new()
	a_cpp.mantissa = 1.23456
	a_cpp.exponent = 50

	var a_new: Big = Big.new(1.23456, 50)

	var time_cpp: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _s: String = a_cpp.to_aa()
	time_cpp = Time.get_ticks_usec() - time_cpp

	var time_new: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _s: String = a_new.toAA()
	time_new = Time.get_ticks_usec() - time_new

	update_ui_row("FmtAA", time_cpp, time_new)


## Benchmarks metric symbol formatting performance.
func benchmark_formatting_metric_symbol() -> void:
	var a_cpp: BigNumber = BigNumber.new()
	a_cpp.mantissa = 1.23456
	a_cpp.exponent = 12

	var a_new: Big = Big.new(1.23456, 12)

	var time_cpp: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _s: String = a_cpp.to_metric_symbol()
	time_cpp = Time.get_ticks_usec() - time_cpp

	var time_new: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _s: String = a_new.toMetricSymbol()
	time_new = Time.get_ticks_usec() - time_new

	update_ui_row("FmtSym", time_cpp, time_new)


## Benchmarks metric name formatting performance.
func benchmark_formatting_metric_name() -> void:
	var a_cpp: BigNumber = BigNumber.new()
	a_cpp.mantissa = 1.23456
	a_cpp.exponent = 12

	var a_new: Big = Big.new(1.23456, 12)

	var time_cpp: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _s: String = a_cpp.to_metric_name()
	time_cpp = Time.get_ticks_usec() - time_cpp

	var time_new: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _s: String = a_new.toMetricName()
	time_new = Time.get_ticks_usec() - time_new

	update_ui_row("FmtNam", time_cpp, time_new)


## Benchmarks equality check performance.
func benchmark_is_equal() -> void:
	var a_cpp: BigNumber = BigNumber.new()
	a_cpp.mantissa = 1.0
	a_cpp.exponent = 50
	var b_cpp: BigNumber = BigNumber.new()
	b_cpp.mantissa = 1.0
	b_cpp.exponent = 50

	var a_new: Big = Big.new(1.0, 50)
	var b_new: Big = Big.new(1.0, 50)

	var time_cpp: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _c: bool = a_cpp.is_equal_to(b_cpp)
	time_cpp = Time.get_ticks_usec() - time_cpp

	var time_new: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _c: bool = a_new.isEqualTo(b_new)
	time_new = Time.get_ticks_usec() - time_new

	update_ui_row("IsEq", time_cpp, time_new)


## Benchmarks absolute value performance.
func benchmark_absolute() -> void:
	var a_cpp: BigNumber = BigNumber.new()
	a_cpp.mantissa = -1.5
	a_cpp.exponent = 10

	var a_new: Big = Big.new(-1.5, 10)

	var time_cpp: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _c: BigNumber = a_cpp.absolute()
	time_cpp = Time.get_ticks_usec() - time_cpp

	var time_new: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _c: Big = Big.absolute(a_new)
	time_new = Time.get_ticks_usec() - time_new

	update_ui_row("Abs", time_cpp, time_new)


## Benchmarks log10 calculation performance.
func benchmark_log10() -> void:
	var a_cpp: BigNumber = BigNumber.new()
	a_cpp.mantissa = 1.5
	a_cpp.exponent = 10

	var a_new: Big = Big.new(1.5, 10)

	var time_cpp: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _c: float = a_cpp.log10()
	time_cpp = Time.get_ticks_usec() - time_cpp

	var time_new: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _c: float = a_new.absLog10()
	time_new = Time.get_ticks_usec() - time_new

	update_ui_row("Log10", time_cpp, time_new)


## Benchmarks natural logarithm performance.
func benchmark_ln() -> void:
	var a_cpp: BigNumber = BigNumber.new()
	a_cpp.mantissa = 1.5
	a_cpp.exponent = 10

	var a_new: Big = Big.new(1.5, 10)

	var time_cpp: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _c: float = a_cpp.ln()
	time_cpp = Time.get_ticks_usec() - time_cpp

	var time_new: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _c: float = a_new.ln()
	time_new = Time.get_ticks_usec() - time_new

	update_ui_row("Ln", time_cpp, time_new)


## Benchmarks floor value performance.
func benchmark_floor() -> void:
	var a_cpp: BigNumber = BigNumber.new()
	a_cpp.mantissa = 1.5
	a_cpp.exponent = 0

	var a_new: Big = Big.new(1.5, 0)

	var time_cpp: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		a_cpp.floor_value()
	time_cpp = Time.get_ticks_usec() - time_cpp

	var time_new: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		Big.roundDown(a_new)
	time_new = Time.get_ticks_usec() - time_new

	update_ui_row("Floor", time_cpp, time_new)


## Benchmarks conversion to float performance.
func benchmark_to_float() -> void:
	var a_cpp: BigNumber = BigNumber.new()
	a_cpp.mantissa = 1.5
	a_cpp.exponent = 10

	var a_new: Big = Big.new(1.5, 10)

	var time_cpp: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _c: float = a_cpp.to_float()
	time_cpp = Time.get_ticks_usec() - time_cpp

	var time_new: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _c: float = a_new.toFloat()
	time_new = Time.get_ticks_usec() - time_new

	update_ui_row("ToFloat", time_cpp, time_new)


## Benchmarks plain scientific formatting performance.
func benchmark_formatting_plain() -> void:
	var a_cpp: BigNumber = BigNumber.new()
	a_cpp.mantissa = 1.23456
	a_cpp.exponent = 50

	var a_new: Big = Big.new(1.23456, 50)

	var time_cpp: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _s: String = a_cpp.to_plain_scientific()
	time_cpp = Time.get_ticks_usec() - time_cpp

	var time_new: int = Time.get_ticks_usec()
	for i: int in range(ITERATIONS):
		var _s: String = a_new.toPlainScientific()
	time_new = Time.get_ticks_usec() - time_new

	update_ui_row("FmtPlain", time_cpp, time_new)


## Sets up alternating row colors for the results table.
func setup_table_style() -> void:
	var grid: GridContainer = $Panel/MarginContainer/VBoxContainer/ScrollContainer/GridContainer
	var children: Array[Node] = grid.get_children()
	var row_size: int = 4
	
	var odd_style: StyleBoxFlat = StyleBoxFlat.new()
	odd_style.bg_color = Color(0, 0, 0, 0.2)
	odd_style.content_margin_left = 5
	odd_style.content_margin_right = 5
	odd_style.set_corner_radius_all(4)

	var even_style: StyleBoxFlat = StyleBoxFlat.new()
	even_style.bg_color = Color(0, 0, 0, 0) # Transparent
	even_style.content_margin_left = 5
	even_style.content_margin_right = 5
	even_style.set_corner_radius_all(4)

	var data_index: int = 0
	for child: Node in children:
		var control: Control = child as Control
		if not control:
			continue
			
		# Skip headers (starting with H_) and Separators
		if child.name.begins_with("H_") or child is Separator:
			continue
		
		# Ensure label fills the cell for background consistency
		control.size_flags_vertical = Control.SIZE_EXPAND_FILL
		control.size_flags_horizontal = Control.SIZE_EXPAND_FILL
		
		var row_idx: int = data_index / row_size
		if row_idx % 2 == 1:
			control.add_theme_stylebox_override("normal", odd_style)
		else:
			control.add_theme_stylebox_override("normal", even_style)
		
		data_index += 1
