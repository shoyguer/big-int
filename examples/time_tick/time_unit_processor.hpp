// MIT License
// Copyright (c) 2025 Lucas "Shoyguer" Melo

#pragma once

#include "time_unit_manager.hpp"
#include <godot_cpp/variant/callable.hpp>

using namespace godot;

// Internal helper class to handle time unit increment/decrement logic
// This is NOT exposed to Godot. This is just for internal organization.
class TimeUnitProcessor {
public:
	TimeUnitProcessor(TimeUnitManager *manager) : unit_manager(manager) {}
	~TimeUnitProcessor() = default;
	
	// Set signal emission callback
	void set_signal_callback(Callable callback) { signal_callback = callback; }
	void set_current_tick(int tick) { current_tick = tick; }
	
	// Core processing
	void increment_unit(const String &unit_name);
	void decrement_unit(const String &unit_name);
	
private:
	TimeUnitManager *unit_manager = nullptr;
	Callable signal_callback;
	int current_tick = 0;
	
	// Helper methods
	void process_simple_unit_increment(const String &child_name, const String &parent_name);
	void process_simple_unit_decrement(const String &child_name, const String &parent_name);
	void process_complex_unit(const String &child_name, const String &parent_name);
	
	bool check_complex_conditions(const String &unit_name);
	int apply_wrapping(int value, int min_val, int max_val);
	void emit_change_signal(const String &name, int new_val, int old_val);
};
