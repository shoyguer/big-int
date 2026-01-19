extends Node2D

func _ready() -> void:
	test_big_int()

func test_big_int() -> void:
	print("--- Testing BigInt ---")
	
	var a = BigInt.new()
	a.mantissa = 1.5
	a.exponent = 5
	print("Created A (1.5e5 manually): ", a)

	var b = BigInt.new()
	b.mantissa = 1.0
	b.exponent = 5
	print("Created B (1.0e5 manually): ", b)
	
	print("A > B: ", a.is_greater_than(b))
	print("A < B: ", a.is_less_than(b))  
	
	# Test Arithmetic with Variant (String)
	# This exercises _type_check -> BigInt(Variant) -> BigInt(String)
	print("Testing plus('1.0e5')...")
	var sum = a.plus("1.0e5") 
	print("A + '1.0e5' = ", sum)
	
	# Test Arithmetic with Variant (float)
	print("Testing multiply(2.0)...")
	var prod = a.multiply(2.0)
	print("A * 2.0 = ", prod)
	
	print("--- End Test ---")
