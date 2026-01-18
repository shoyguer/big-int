extends Node2D

func _ready() -> void:
	test_plugin_functionality()

func test_plugin_functionality()->void:
	var my_res:ItemData = ItemData.new()
	
	print(my_res.name)
	my_res.name = "Sword"
	print(my_res.name)
	
	print(my_res.price)
	
	var nums:Array[int] = my_res.create_10k_numbers(5, 10)
	print(nums)
	
	print("Spawning stuff..")
	my_res.spawn_stuff()
	
	print("Spawning custom scene..")
	var custom_scn:PackedScene = preload("res://main.tscn")
	my_res.spawn_custom_scene(custom_scn)
	
	var nodes:Array[Node] = [Node.new(), Node.new()]
	
	nodes[0].name = "TestNodeName1"
	nodes[1].name = "TestNodeName2"
	
	print("Print node name")
	my_res.print_node_name(nodes[0])
	
	print("Print resource name")
	
	var res:Resource = Resource.new()
	res.resource_name = "TestResource"
	
	my_res.print_resource_name(res)
	
	print("Print ALL node names")
	my_res.print_all_node_names(nodes)
	
	my_res.modify_all_node_names_wrong_way(nodes, "RenamedNode")
	
	print("Print ALL node names after modification")
	my_res.print_all_node_names(nodes)
	
	
	
	
	
	
