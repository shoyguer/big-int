// MIT License
// Copyright (c) 2025 Lucas "Shoyguer" Melo

#include "time_tick.hpp"
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/time.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;


TimeTick::TimeTick() {
	// Constructor
}

TimeTick::~TimeTick() {
	shutdown();
	if (processor) {
		delete processor;
		processor = nullptr;
	}
}

// Initializes the time system with specified tick duration and connects to physics frame
void TimeTick::initialize(double tick_duration) {
	if (tick_duration <= 0.0) {
		UtilityFunctions::push_warning("TimeTick: Tick duration must be greater than 0.0, clamping to 0.001");
		tick_duration = 0.001;
	}
	tick_time = CLAMP(tick_duration, 0.001, 600.0);
	current_tick = 0;
	accumulated_time = 0.0;
	paused = false;
	time_scale = 1.0;
	initialized = true;
	
	// Clear helper classes
	unit_manager.clear();
	
	// Initialize processor with signal callback
	if (!processor) {
		processor = new TimeUnitProcessor(&unit_manager);
	}
	Callable callback = callable_mp(this, &TimeTick::_emit_unit_changed);
	processor->set_signal_callback(callback);
	
	// Connect to SceneTree's physics_frame signal
	SceneTree *tree = Object::cast_to<SceneTree>(Engine::get_singleton()->get_main_loop());
	if (tree && !connected_to_physics) {
		Callable physics_callback = callable_mp(this, &TimeTick::_on_physics_frame);
		if (!tree->is_connected("physics_frame", physics_callback)) {
			tree->connect("physics_frame", physics_callback);
			connected_to_physics = true;
		}
		last_physics_time = Time::get_singleton()->get_ticks_msec() / 1000.0;
	}
}

// Registers a simple time unit that increments when a tracked unit reaches trigger count
void TimeTick::register_time_unit(const String &unit_name, const String &tracked_unit, int trigger_count, int max_value, int min_value) {
	if (unit_name.is_empty()) {
		UtilityFunctions::push_error("TimeTick: Unit name cannot be empty");
		return;
	}
	
	if (trigger_count <= 0) {
		UtilityFunctions::push_error("TimeTick: Trigger count must be positive");
		return;
	}
	
	// Delegate to manager
	unit_manager.register_simple_unit(unit_name, tracked_unit, trigger_count, max_value, min_value);
}

// Registers a complex time unit that increments when all tracked units meet specific conditions
void TimeTick::register_complex_time_unit(const String &unit_name, const Dictionary &tracked_units, int max_value, int min_value) {
	if (unit_name.is_empty()) {
		UtilityFunctions::push_error("TimeTick: Unit name cannot be empty");
		return;
	}
	
	if (tracked_units.is_empty()) {
		UtilityFunctions::push_error("TimeTick: Complex time unit must track at least one unit");
		return;
	}
	
	// Validate that all tracked units exist
	Array keys = tracked_units.keys();
	for (int i = 0; i < keys.size(); i++) {
		String tracked_unit = keys[i];
		if (!unit_manager.has_unit(tracked_unit) && tracked_unit != "tick") {
			UtilityFunctions::push_warning(vformat("TimeTick: Tracked unit '%s' not yet registered, make sure to register it first", tracked_unit));
		}
	}
	
	// Delegate to manager
	unit_manager.register_complex_unit(unit_name, tracked_units, max_value, min_value);
}

// Removes a time unit from the system
void TimeTick::unregister_time_unit(const String &unit_name) {
	unit_manager.unregister_unit(unit_name);
}

// Sets how much a time unit increments per parent unit tick
void TimeTick::set_time_unit_step(const String &unit_name, int step_amount) {
	if (!unit_manager.has_unit(unit_name)) {
		UtilityFunctions::push_error(vformat("TimeTick: Time unit '%s' not found", unit_name));
		return;
	}
	unit_manager.set_step(unit_name, step_amount);
}

// Returns the step amount for a time unit
int TimeTick::get_time_unit_step(const String &unit_name) const {
	return unit_manager.get_step(unit_name);
}

// Sets how many tracked units are needed before this unit increments
void TimeTick::set_time_unit_trigger_count(const String &unit_name, int trigger_count) {
	if (trigger_count <= 0) {
		UtilityFunctions::push_error("TimeTick: Trigger count must be positive");
		return;
	}
	
	if (!unit_manager.has_unit(unit_name)) {
		UtilityFunctions::push_error(vformat("TimeTick: Time unit '%s' not found", unit_name));
		return;
	}
	
	if (unit_manager.is_complex(unit_name)) {
		UtilityFunctions::push_error(vformat("TimeTick: Cannot set trigger_count for complex time unit '%s'. Use tracked_units dictionary instead.", unit_name));
		return;
	}
	
	unit_manager.set_trigger_count(unit_name, trigger_count);
}

// Returns the trigger count for a time unit (returns -1 for complex units)
int TimeTick::get_time_unit_trigger_count(const String &unit_name) const {
	if (unit_manager.is_complex(unit_name)) {
		UtilityFunctions::push_warning(vformat("TimeTick: Complex time unit '%s' doesn't have a single trigger_count. Use get_time_unit_tracked_units() instead.", unit_name));
		return -1;
	}
	return unit_manager.get_trigger_count(unit_name);
}

// Sets the minimum value a time unit wraps back to when exceeding max
void TimeTick::set_time_unit_starting_value(const String &unit_name, int starting_value) {
	if (!unit_manager.has_unit(unit_name)) {
		UtilityFunctions::push_error(vformat("TimeTick: Time unit '%s' not found", unit_name));
		return;
	}
	unit_manager.set_min_value(unit_name, starting_value);
}

// Returns the starting value (minimum) for a time unit
int TimeTick::get_time_unit_starting_value(const String &unit_name) const {
	return unit_manager.get_min_value(unit_name);
}

// Returns a dictionary containing all data for a time unit
Dictionary TimeTick::get_time_unit_data(const String &unit_name) const {
	return unit_manager.get_unit(unit_name);
}

// Returns the current value of a time unit
int TimeTick::get_time_unit(const String &unit_name) const {
	return unit_manager.get_value(unit_name);
}

// Sets the current value of a time unit directly and emits signal if changed
void TimeTick::set_time_unit(const String &unit_name, int value) {
	if (!unit_manager.has_unit(unit_name)) {
		UtilityFunctions::push_error(vformat("TimeTick: Time unit '%s' not found", unit_name));
		return;
	}
	
	int old_value = unit_manager.get_value(unit_name);
	unit_manager.set_value(unit_name, value);
	unit_manager.set_counter(unit_name, 0);
	
	if (old_value != value) {
		emit_signal("time_unit_changed", unit_name, value, old_value);
	}
}

// Sets multiple time unit values at once from a dictionary and recalculates counters
void TimeTick::set_time_units(const Dictionary &values) {
	// First, set all the values
	Array keys = values.keys();
	for (int i = 0; i < keys.size(); i++) {
		String unit_name = keys[i];
		int value = values[unit_name];
		if (unit_manager.has_unit(unit_name)) {
			unit_manager.set_value(unit_name, value);
		}
	}
	
	// Then recalculate all counters based on what each unit tracks
	Array all_keys = unit_manager.get_all_unit_names();
	for (int i = 0; i < all_keys.size(); i++) {
		String unit_name = all_keys[i];
		
		// Only update counters for simple (non-complex) units
		if (!unit_manager.is_complex(unit_name)) {
			String tracked = unit_manager.get_tracked_unit(unit_name);
			
			// Set counter based on the tracked unit's current value
			if (tracked == "tick") {
				unit_manager.set_counter(unit_name, current_tick);
			} else if (unit_manager.has_unit(tracked)) {
				int tracked_value = unit_manager.get_value(tracked);
				int tracked_step = unit_manager.get_step(tracked);
				unit_manager.set_counter(unit_name, tracked_value * tracked_step);
			} else {
				unit_manager.set_counter(unit_name, 0);
			}
		} else {
			unit_manager.set_counter(unit_name, 0);
		}
	}
	
	// Finally, emit signals for changed values
	for (int i = 0; i < keys.size(); i++) {
		String unit_name = keys[i];
		int value = values[unit_name];
		emit_signal("time_unit_changed", unit_name, value, value);
	}
}

// Returns an array of all registered time unit names
TypedArray<String> TimeTick::get_time_unit_names() const {
	return unit_manager.get_all_names();
}

// Returns a formatted string with time unit values replacing {unit_name} placeholders
String TimeTick::get_formatted_time(const String &format_string) const {
	String result = format_string;
	Array keys = unit_manager.get_all_unit_names();
	
	for (int i = 0; i < keys.size(); i++) {
		String unit_name = keys[i];
		int value = unit_manager.get_value(unit_name);
		String placeholder = String("{") + unit_name + String("}");
		result = result.replace(placeholder, String::num_int64(value));
	}
	
	return result;
}

// Returns a formatted string with zero-padded time unit values separated by a delimiter
String TimeTick::get_formatted_time_padded(const TypedArray<String> &units, const String &separator, int padding) const {
	PackedStringArray parts;
	
	for (int i = 0; i < units.size(); i++) {
		String unit_name = units[i];
		if (unit_manager.has_unit(unit_name)) {
			int value = unit_manager.get_value(unit_name);
			parts.append(String::num_int64(value).pad_zeros(padding));
		} else {
			parts.append("00");
		}
	}
	
	return separator.join(parts);
}

// Cleans up the time system and disconnects from physics frame
void TimeTick::shutdown() {
	// Disconnect from SceneTree's physics_frame signal
	SceneTree *tree = Object::cast_to<SceneTree>(Engine::get_singleton()->get_main_loop());
	if (tree && connected_to_physics) {
		Callable callback = callable_mp(this, &TimeTick::_on_physics_frame);
		if (tree->is_connected("physics_frame", callback)) {
			tree->disconnect("physics_frame", callback);
		}
		connected_to_physics = false;
	}
	
	initialized = false;
	unit_manager.clear();
}

// Pauses time progression
void TimeTick::pause() {
	paused = true;
}

// Resumes time progression after being paused
void TimeTick::resume() {
	paused = false;
}

// Toggles between paused and unpaused states
void TimeTick::toggle_pause() {
	paused = !paused;
}

// Returns true if time is currently paused
bool TimeTick::is_paused() const {
	return paused;
}

// Resets tick count and all time units to their starting values
void TimeTick::reset() {
	current_tick = 0;
	accumulated_time = 0.0;
	unit_manager.reset_all_to_min();
}

// Sets the time scale multiplier (negative values reverse time)
void TimeTick::set_time_scale(double scale) {
	time_scale = CLAMP(scale, -1000.0, 1000.0);
}

// Returns the current time scale multiplier
double TimeTick::get_time_scale() const {
	return time_scale;
}

// Sets the tick duration in real-time seconds
void TimeTick::set_tick_duration(double duration) {
	if (duration <= 0.0) {
		UtilityFunctions::push_warning("TimeTick: Tick duration must be greater than 0.0, clamping to 0.001");
		duration = 0.001;
	}
	tick_time = CLAMP(duration, 0.001, 600.0);
}

// Returns the tick duration in real-time seconds
double TimeTick::get_tick_duration() const {
	return tick_time;
}

// Returns the current tick count
int TimeTick::get_current_tick() const {
	return current_tick;
}

// Returns progress to next tick as a value between 0.0 and 1.0
double TimeTick::get_tick_progress() const {
	if (tick_time <= 0.0) {
		return 0.0;
	}
	return CLAMP(accumulated_time / tick_time, 0.0, 1.0);
}

// Returns true if the system has been initialized
bool TimeTick::is_initialized() const {
	return initialized;
}


// Private methods
// Called every physics frame to process time progression
void TimeTick::_on_physics_frame() {
	if (!initialized) {
		return;
	}
	
	double current_time = Time::get_singleton()->get_ticks_msec() / 1000.0;
	double delta = current_time - last_physics_time;
	last_physics_time = current_time;
	_process_tick(delta);
}

// Processes time accumulation and triggers ticks (supports forward and backward time)
void TimeTick::_process_tick(double delta) {
	if (paused) {
		return;
	}
	
	// Apply time scale
	double scaled_delta = delta * time_scale;
	accumulated_time += scaled_delta;
	
	// Handle forward time (positive time_scale)
	if (time_scale >= 0.0) {
		// Process all ticks that should have occurred
		while (accumulated_time >= tick_time) {
			accumulated_time -= tick_time;
			
			// Check for overflow - reset to 0 if we exceed INT_MAX
			if (current_tick >= INT_MAX) {
				current_tick = 0;
				UtilityFunctions::push_warning("TimeTick: Tick count reached maximum value, resetting to 0");
			} else {
				current_tick += 1;
			}
			
			// Increment the "tick" unit
			_increment_unit("tick");
			
			// Emit signal
			emit_signal("tick_updated", current_tick);
		}
	} else {
		// Handle backward time (negative time_scale)
		// accumulated_time will be negative, so we check if it's <= -tick_time
		while (accumulated_time <= -tick_time) {
			accumulated_time += tick_time;
			
			// Check for underflow - reset to 0 if we go below 0
			if (current_tick <= 0) {
				current_tick = 0;
				UtilityFunctions::push_warning("TimeTick: Tick count reached minimum value (0), cannot decrement further");
				// Stop decrementing to prevent going negative
				accumulated_time = 0.0;
				break;
			} else {
				current_tick -= 1;
			}
			
			// Decrement the "tick" unit
			_decrement_unit("tick");
			
			// Emit signal
			emit_signal("tick_updated", current_tick);
		}
	}
}

// Increments a time unit and processes cascading effects
void TimeTick::_increment_unit(const String &unit_name) {
	if (processor) {
		processor->set_current_tick(current_tick);
		processor->increment_unit(unit_name);
	}
}

// Decrements a time unit and processes cascading effects
void TimeTick::_decrement_unit(const String &unit_name) {
	if (processor) {
		processor->set_current_tick(current_tick);
		processor->decrement_unit(unit_name);
	}
}

// Emits the time_unit_changed signal when a unit value changes
// Signal emission helper (called by processor via callback)
void TimeTick::_emit_unit_changed(const String &name, int new_val, int old_val) {
	emit_signal("time_unit_changed", name, new_val, old_val);
}

// Registers all methods, properties, and signals with Godot's ClassDB
// Bind methods
void TimeTick::_bind_methods() {
	// Signals
	ADD_SIGNAL(MethodInfo("tick_updated", PropertyInfo(Variant::INT, "current_tick")));
	ADD_SIGNAL(MethodInfo("time_unit_changed", 
		PropertyInfo(Variant::STRING, "unit_name"), 
		PropertyInfo(Variant::INT, "new_value"), 
		PropertyInfo(Variant::INT, "old_value")));
	
	// Methods
	ClassDB::bind_method(D_METHOD("initialize", "tick_duration"), &TimeTick::initialize, DEFVAL(1.0));
	ClassDB::bind_method(D_METHOD("register_time_unit", "unit_name", "tracked_unit", "trigger_count", "max_value", "min_value"), 
		&TimeTick::register_time_unit, DEFVAL(1), DEFVAL(-1), DEFVAL(0));
	ClassDB::bind_method(D_METHOD("register_complex_time_unit", "unit_name", "tracked_units", "max_value", "min_value"),
		&TimeTick::register_complex_time_unit, DEFVAL(-1), DEFVAL(0));
	ClassDB::bind_method(D_METHOD("unregister_time_unit", "unit_name"), &TimeTick::unregister_time_unit);
	ClassDB::bind_method(D_METHOD("set_time_unit_step", "unit_name", "step_amount"), &TimeTick::set_time_unit_step);
	ClassDB::bind_method(D_METHOD("get_time_unit_step", "unit_name"), &TimeTick::get_time_unit_step);
	ClassDB::bind_method(D_METHOD("set_time_unit_trigger_count", "unit_name", "trigger_count"), &TimeTick::set_time_unit_trigger_count);
	ClassDB::bind_method(D_METHOD("get_time_unit_trigger_count", "unit_name"), &TimeTick::get_time_unit_trigger_count);
	ClassDB::bind_method(D_METHOD("set_time_unit_starting_value", "unit_name", "starting_value"), &TimeTick::set_time_unit_starting_value);
	ClassDB::bind_method(D_METHOD("get_time_unit_starting_value", "unit_name"), &TimeTick::get_time_unit_starting_value);
	ClassDB::bind_method(D_METHOD("get_time_unit_data", "unit_name"), &TimeTick::get_time_unit_data);
	ClassDB::bind_method(D_METHOD("get_time_unit", "unit_name"), &TimeTick::get_time_unit);
	ClassDB::bind_method(D_METHOD("set_time_unit", "unit_name", "value"), &TimeTick::set_time_unit);
	ClassDB::bind_method(D_METHOD("set_time_units", "values"), &TimeTick::set_time_units);
	ClassDB::bind_method(D_METHOD("get_time_unit_names"), &TimeTick::get_time_unit_names);
	ClassDB::bind_method(D_METHOD("get_formatted_time", "format_string"), &TimeTick::get_formatted_time);
	ClassDB::bind_method(D_METHOD("get_formatted_time_padded", "units", "separator", "padding"), 
		&TimeTick::get_formatted_time_padded, DEFVAL(":"), DEFVAL(2));
	ClassDB::bind_method(D_METHOD("shutdown"), &TimeTick::shutdown);
	ClassDB::bind_method(D_METHOD("pause"), &TimeTick::pause);
	ClassDB::bind_method(D_METHOD("resume"), &TimeTick::resume);
	ClassDB::bind_method(D_METHOD("toggle_pause"), &TimeTick::toggle_pause);
	ClassDB::bind_method(D_METHOD("is_paused"), &TimeTick::is_paused);
	ClassDB::bind_method(D_METHOD("reset"), &TimeTick::reset);
	ClassDB::bind_method(D_METHOD("set_time_scale", "scale"), &TimeTick::set_time_scale);
	ClassDB::bind_method(D_METHOD("get_time_scale"), &TimeTick::get_time_scale);
	ClassDB::bind_method(D_METHOD("set_tick_duration", "duration"), &TimeTick::set_tick_duration);
	ClassDB::bind_method(D_METHOD("get_tick_duration"), &TimeTick::get_tick_duration);
	ClassDB::bind_method(D_METHOD("get_current_tick"), &TimeTick::get_current_tick);
	ClassDB::bind_method(D_METHOD("get_tick_progress"), &TimeTick::get_tick_progress);
	ClassDB::bind_method(D_METHOD("is_initialized"), &TimeTick::is_initialized);
}
