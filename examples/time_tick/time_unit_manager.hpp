// MIT License
// Copyright (c) 2025 Lucas "Shoyguer" Melo

#pragma once

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/typed_array.hpp>

using namespace godot;


// Internal helper class to manage time unit storage and operations
// This is NOT exposed to Godot. This is just for internal organization.
class TimeUnitManager {
public:
	TimeUnitManager() = default;
	~TimeUnitManager() = default;

	// Registration
	void register_simple_unit(const String &name, const String &tracked_unit, int trigger_count, int max_value, int min_value);
	void register_complex_unit(const String &name, const Dictionary &tracked_units, int max_value, int min_value);
	void unregister_unit(const String &name);
	
	// Getters
	bool has_unit(const String &name) const;
	Dictionary get_unit(const String &name) const;
	int get_value(const String &name) const;
	TypedArray<String> get_all_names() const;
	
	// Setters
	void set_value(const String &name, int value);
	void set_step(const String &name, int step);
	void set_trigger_count(const String &name, int count);
	void set_min_value(const String &name, int min_val);
	
	// Queries
	bool is_complex(const String &name) const;
	int get_step(const String &name) const;
	int get_trigger_count(const String &name) const;
	int get_min_value(const String &name) const;
	int get_max_value(const String &name) const;
	String get_tracked_unit(const String &name) const;
	Dictionary get_tracked_units(const String &name) const;
	
	// Bulk operations
	void reset_all_to_min();
	void clear();
	
	// Counter management
	void init_counter(const String &name);
	int get_counter(const String &name) const;
	void set_counter(const String &name, int value);
	void increment_counter(const String &name, int amount);
	void decrement_counter(const String &name, int amount);
	
	Array get_all_unit_names() const { return units.keys(); }
	
private:
	// Stores time unit data
	Dictionary units;
	// Stores accumulation counters for
	Dictionary counters;
};
