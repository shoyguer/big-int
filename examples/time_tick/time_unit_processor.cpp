// MIT License
// Copyright (c) 2025 Lucas "Shoyguer" Melo

#include "time_unit_processor.hpp"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;


// Increments a unit and cascades to all dependent child units
void TimeUnitProcessor::increment_unit(const String &unit_name) {
	Array all_units = unit_manager->get_all_unit_names();
	
	for (int i = 0; i < all_units.size(); i++) {
		String child_name = all_units[i];
		
		if (unit_manager->is_complex(child_name)) {
			process_complex_unit(child_name, unit_name);
		} else {
			String tracked = unit_manager->get_tracked_unit(child_name);
			if (tracked == unit_name) {
				process_simple_unit_increment(child_name, unit_name);
			}
		}
	}
}

// Decrements a unit and cascades to all dependent child units (reverse time)
void TimeUnitProcessor::decrement_unit(const String &unit_name) {
	Array all_units = unit_manager->get_all_unit_names();
	
	for (int i = 0; i < all_units.size(); i++) {
		String child_name = all_units[i];
		
		// Complex units don't support reverse time yet
		if (unit_manager->is_complex(child_name)) {
			continue;
		}
		
		String tracked = unit_manager->get_tracked_unit(child_name);
		if (tracked == unit_name) {
			process_simple_unit_decrement(child_name, unit_name);
		}
	}
}

// Processes increment for a simple unit, handling counters, overflow, and wrapping
void TimeUnitProcessor::process_simple_unit_increment(const String &child_name, const String &parent_name) {
	int counter = unit_manager->get_counter(child_name);
	int parent_step = unit_manager->get_step(parent_name);
	counter += parent_step;
	
	int trigger_count = unit_manager->get_trigger_count(child_name);
	
	if (counter >= trigger_count) {
		counter -= trigger_count;
		unit_manager->set_counter(child_name, counter);
		
		int old_value = unit_manager->get_value(child_name);
		int step = unit_manager->get_step(child_name);
		int max_value = unit_manager->get_max_value(child_name);
		int min_value = unit_manager->get_min_value(child_name);
		int new_value = old_value + step;
		
		// Check for overflow
		if (step > 0 && old_value > INT_MAX - step) {
			new_value = min_value;
			UtilityFunctions::push_warning(vformat("TimeTick: Time unit '%s' would overflow, resetting to %d", child_name, min_value));
		} else if (step < 0 && old_value < INT_MIN - step) {
			new_value = min_value;
			UtilityFunctions::push_warning(vformat("TimeTick: Time unit '%s' would underflow, resetting to %d", child_name, min_value));
		}
		
		// Apply wrapping
		bool did_wrap = false;
		if (max_value > 0) {
			int wrapped_value = apply_wrapping(new_value, min_value, max_value);
			did_wrap = (wrapped_value != new_value);
			new_value = wrapped_value;
			
			// If wrapped, update value and trigger children before emitting signal
			if (did_wrap) {
				unit_manager->set_value(child_name, new_value);
				increment_unit(child_name);
				emit_change_signal(child_name, new_value, old_value);
				return;
			}
		}
		
		// Normal flow: update value, emit signal, then trigger children
		unit_manager->set_value(child_name, new_value);
		
		if (old_value != new_value) {
			emit_change_signal(child_name, new_value, old_value);
		}
		
		increment_unit(child_name);
	} else {
		unit_manager->set_counter(child_name, counter);
	}
}

// Processes decrement for a simple unit (reverse time support)
void TimeUnitProcessor::process_simple_unit_decrement(const String &child_name, const String &parent_name) {
	int counter = unit_manager->get_counter(child_name);
	int parent_step = unit_manager->get_step(parent_name);
	counter -= parent_step;
	
	int trigger_count = unit_manager->get_trigger_count(child_name);
	
	if (counter < 0) {
		counter += trigger_count;
		unit_manager->set_counter(child_name, counter);
		
		int old_value = unit_manager->get_value(child_name);
		int step = unit_manager->get_step(child_name);
		int max_value = unit_manager->get_max_value(child_name);
		int min_value = unit_manager->get_min_value(child_name);
		int new_value = old_value - step;
		
		// Apply wrapping for reverse
		if (max_value > 0) {
			new_value = apply_wrapping(new_value, min_value, max_value);
		} else if (new_value < 0) {
			new_value = 0;
		}
		
		unit_manager->set_value(child_name, new_value);
		
		if (old_value != new_value) {
			emit_change_signal(child_name, new_value, old_value);
		}
		
		decrement_unit(child_name);
	} else {
		unit_manager->set_counter(child_name, counter);
	}
}

// Processes complex units that depend on multiple tracked unit values
void TimeUnitProcessor::process_complex_unit(const String &child_name, const String &parent_name) {
	Dictionary tracked_units = unit_manager->get_tracked_units(child_name);
	
	// Only check if the parent being incremented is tracked
	if (!tracked_units.has(parent_name)) {
		return;
	}
	
	// Check if all conditions are met
	bool all_met = check_complex_conditions(child_name);
	
	// Get or create trigger state
	Dictionary unit = unit_manager->get_unit(child_name);
	String state_key = String(child_name) + String("_triggered");
	bool was_triggered = unit.has(state_key) && (bool)unit[state_key];
	
	if (all_met && !was_triggered) {
		// All conditions met, trigger!
		int old_value = unit_manager->get_value(child_name);
		int step = unit_manager->get_step(child_name);
		int max_value = unit_manager->get_max_value(child_name);
		int min_value = unit_manager->get_min_value(child_name);
		int new_value = apply_wrapping(old_value + step, min_value, max_value);
		
		unit_manager->set_value(child_name, new_value);
		
		// Mark as triggered
		unit[state_key] = true;
		
		if (old_value != new_value) {
			emit_change_signal(child_name, new_value, old_value);
		}
		
		increment_unit(child_name);
	} else if (!all_met && was_triggered) {
		// Conditions no longer met, reset trigger
		unit[state_key] = false;
	}
}

// Checks if all conditions for a complex unit are met
bool TimeUnitProcessor::check_complex_conditions(const String &unit_name) {
	Dictionary tracked_units = unit_manager->get_tracked_units(unit_name);
	Array keys = tracked_units.keys();
	
	for (int i = 0; i < keys.size(); i++) {
		String tracked_name = keys[i];
		int required_value = tracked_units[tracked_name];
		int current_value = 0;
		
		if (tracked_name == "tick") {
			current_value = current_tick;
		} else {
			current_value = unit_manager->get_value(tracked_name);
		}
		
		if (current_value < required_value) {
			return false;
		}
	}
	
	return true;
}

// Applies min/max wrapping to a value (e.g., 60 seconds wraps to 0)
int TimeUnitProcessor::apply_wrapping(int value, int min_val, int max_val) {
	if (max_val <= 0) {
		return value;
	}
	
	int range = max_val - min_val;
	while (value >= max_val) {
		value -= range;
	}
	while (value < min_val) {
		value += range;
	}
	
	return value;
}

// Emits the time_unit_changed signal through the callback
void TimeUnitProcessor::emit_change_signal(const String &name, int new_val, int old_val) {
	if (signal_callback.is_valid()) {
		Array args;
		args.append(name);
		args.append(new_val);
		args.append(old_val);
		signal_callback.callv(args);
	}
}
