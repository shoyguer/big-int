// MIT License
// Copyright (c) 2025 Lucas "Shoyguer" Melo

#include "time_unit_manager.hpp"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;


// Registers a simple time unit that tracks another unit
void TimeUnitManager::register_simple_unit(const String &name, const String &tracked_unit, int trigger_count, int max_value, int min_value) {
	Dictionary unit;
	unit["name"] = name;
	unit["current_value"] = min_value;
	unit["tracked_unit"] = tracked_unit;
	unit["trigger_count"] = trigger_count;
	unit["step_amount"] = 1;
	unit["max_value"] = max_value;
	unit["min_value"] = min_value;
	units[name] = unit;
	
	init_counter(name);
}

// Registers a complex time unit that tracks multiple units with specific values
void TimeUnitManager::register_complex_unit(const String &name, const Dictionary &tracked_units, int max_value, int min_value) {
	Dictionary unit;
	unit["name"] = name;
	unit["current_value"] = min_value;
	unit["is_complex"] = true;
	unit["tracked_units"] = tracked_units;
	unit["step_amount"] = 1;
	unit["max_value"] = max_value;
	unit["min_value"] = min_value;
	units[name] = unit;
}

// Removes a time unit from the system
void TimeUnitManager::unregister_unit(const String &name) {
	units.erase(name);
	counters.erase(name);
}

// Returns true if the unit exists in the system
bool TimeUnitManager::has_unit(const String &name) const {
	return units.has(name);
}

// Returns the complete data dictionary for a unit
Dictionary TimeUnitManager::get_unit(const String &name) const {
	if (units.has(name)) {
		return units[name];
	}
	return Dictionary();
}

// Returns the current value of a time unit
int TimeUnitManager::get_value(const String &name) const {
	if (units.has(name)) {
		Dictionary unit = units[name];
		return unit["current_value"];
	}
	return 0;
}

// Returns an array of all registered time unit names
TypedArray<String> TimeUnitManager::get_all_names() const {
	TypedArray<String> names;
	Array keys = units.keys();
	for (int i = 0; i < keys.size(); i++) {
		names.append(keys[i]);
	}
	return names;
}

// Sets the current value of a time unit
void TimeUnitManager::set_value(const String &name, int value) {
	if (units.has(name)) {
		Dictionary unit = units[name];
		unit["current_value"] = value;
		units[name] = unit;
	}
}

// Sets the step amount for a time unit (how much it increments)
void TimeUnitManager::set_step(const String &name, int step) {
	if (units.has(name)) {
		Dictionary unit = units[name];
		unit["step_amount"] = step;
		units[name] = unit;
	}
}

// Sets how many times the tracked unit must increment to trigger this unit
void TimeUnitManager::set_trigger_count(const String &name, int count) {
	if (units.has(name)) {
		Dictionary unit = units[name];
		unit["trigger_count"] = count;
		units[name] = unit;
	}
}

// Sets the minimum value for a time unit
void TimeUnitManager::set_min_value(const String &name, int min_val) {
	if (units.has(name)) {
		Dictionary unit = units[name];
		unit["min_value"] = min_val;
		units[name] = unit;
	}
}

// Returns true if the unit is a complex unit (tracks multiple units)
bool TimeUnitManager::is_complex(const String &name) const {
	if (units.has(name)) {
		Dictionary unit = units[name];
		return unit.has("is_complex") && (bool)unit["is_complex"];
	}
	return false;
}

// Returns the step amount for a time unit
int TimeUnitManager::get_step(const String &name) const {
	if (units.has(name)) {
		Dictionary unit = units[name];
		return unit.has("step_amount") ? (int)unit["step_amount"] : 1;
	}
	return 1;
}

// Returns the trigger count for a simple time unit
int TimeUnitManager::get_trigger_count(const String &name) const {
	if (units.has(name)) {
		Dictionary unit = units[name];
		return unit.has("trigger_count") ? (int)unit["trigger_count"] : 1;
	}
	return 1;
}

// Returns the minimum value for a time unit
int TimeUnitManager::get_min_value(const String &name) const {
	if (units.has(name)) {
		Dictionary unit = units[name];
		return unit.has("min_value") ? (int)unit["min_value"] : 0;
	}
	return 0;
}

// Returns the maximum value for a time unit (-1 means no max)
int TimeUnitManager::get_max_value(const String &name) const {
	if (units.has(name)) {
		Dictionary unit = units[name];
		return unit.has("max_value") ? (int)unit["max_value"] : -1;
	}
	return -1;
}

// Returns the name of the unit being tracked by a simple unit
String TimeUnitManager::get_tracked_unit(const String &name) const {
	if (units.has(name)) {
		Dictionary unit = units[name];
		return unit.has("tracked_unit") ? (String)unit["tracked_unit"] : "";
	}
	return "";
}

// Returns the dictionary of tracked units for a complex unit
Dictionary TimeUnitManager::get_tracked_units(const String &name) const {
	if (units.has(name)) {
		Dictionary unit = units[name];
		return unit.has("tracked_units") ? (Dictionary)unit["tracked_units"] : Dictionary();
	}
	return Dictionary();
}

// Resets all time units to their minimum values
void TimeUnitManager::reset_all_to_min() {
	Array keys = units.keys();
	for (int i = 0; i < keys.size(); i++) {
		String name = keys[i];
		int min_val = get_min_value(name);
		set_value(name, min_val);
		set_counter(name, 0);
	}
}

// Clears all registered units and counters
void TimeUnitManager::clear() {
	units.clear();
	counters.clear();
}

// Initializes the counter for a unit to zero if it doesn't exist
void TimeUnitManager::init_counter(const String &name) {
	if (!counters.has(name)) {
		counters[name] = 0;
	}
}

// Returns the current counter value for a unit
int TimeUnitManager::get_counter(const String &name) const {
	if (counters.has(name)) {
		return counters[name];
	}
	return 0;
}

// Sets the counter value for a unit
void TimeUnitManager::set_counter(const String &name, int value) {
	counters[name] = value;
}

// Increments the counter for a unit by the specified amount
void TimeUnitManager::increment_counter(const String &name, int amount) {
	int current = get_counter(name);
	counters[name] = current + amount;
}

// Decrements the counter for a unit by the specified amount
void TimeUnitManager::decrement_counter(const String &name, int amount) {
	int current = get_counter(name);
	counters[name] = current - amount;
}
