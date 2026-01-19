extends Node2D

func _ready() -> void:
	test_big_int()

func test_big_int() -> void:
	print("--- Testing BigInt ---")
	var num = BigInt.new()
	print("Initial value: ", num) # Expected: 1e0 or similar
	
	num.mantissa = 123.0
	print("After setting mantissa to 123.0: ", num) # Expected: 1.23e2
	print("Mantissa: ", num.mantissa)
	print("Exponent: ", num.exponent)
	
	num.exponent = 5
	print("After setting exponent to 5: ", num) # Expected: 1.23e5 (if normalization doesn't change it further)
	
	num.mantissa = 0.05
	print("After setting mantissa to 0.05: ", num) # Expected: 5e(5-2) -> 5e3
	
	print("--- End Test ---")
