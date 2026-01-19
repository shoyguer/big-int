// MIT License
// Copyright (c) 2025 Lucas "Shoyguer" Melo

#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/typed_array.hpp>

// Helper classes
#include "time_unit_manager.hpp"
#include "time_unit_processor.hpp"

using namespace godot;

class TimeTick : public RefCounted {
	GDCLASS(TimeTick, RefCounted)

public:
	struct TimeUnit {
		String name = "";
		int current_value = 0;
		String tracked_unit = "";
		int trigger_count = 1;
		int step_amount = 1;
		int max_value = -1;
		
		TimeUnit() = default;
		TimeUnit(const String &p_name, const String &p_tracked, int p_trigger_count, int p_step = 1, int p_max = -1)
			: name(p_name), tracked_unit(p_tracked), trigger_count(p_trigger_count), step_amount(p_step), max_value(p_max) {}
	};

	TimeTick();
	~TimeTick();

	// Core setup
	void initialize(double tick_duration = 1.0);
	void shutdown();
	
	// Time unit registration
	void register_time_unit(const String &unit_name, const String &tracked_unit, int trigger_count = 1, int max_value = -1, int min_value = 0);
	void register_complex_time_unit(const String &unit_name, const Dictionary &tracked_units, int max_value = -1, int min_value = 0);
	void unregister_time_unit(const String &unit_name);
	
	// Time unit property setters
	void set_time_unit_step(const String &unit_name, int step_amount);
	void set_time_unit_trigger_count(const String &unit_name, int trigger_count);
	void set_time_unit_starting_value(const String &unit_name, int starting_value);
	void set_time_unit(const String &unit_name, int value);
	void set_time_units(const Dictionary &values);
	
	// Time unit property getters
	int get_time_unit_step(const String &unit_name) const;
	int get_time_unit_trigger_count(const String &unit_name) const;
	int get_time_unit_starting_value(const String &unit_name) const;
	int get_time_unit(const String &unit_name) const;
	Dictionary get_time_unit_data(const String &unit_name) const;
	TypedArray<String> get_time_unit_names() const;
	
	// Time formatting
	String get_formatted_time(const String &format_string) const;
	String get_formatted_time_padded(const TypedArray<String> &units, const String &separator = ":", int padding = 2) const;
	
	// Playback control
	void pause();
	void resume();
	void toggle_pause();
	void reset();
	bool is_paused() const;
	
	// Time scale and tick control
	void set_time_scale(double scale);
	double get_time_scale() const;
	void set_tick_duration(double duration);
	double get_tick_duration() const;
	
	// Status queries
	int get_current_tick() const;
	double get_tick_progress() const;
	bool is_initialized() const;

protected:
	static void _bind_methods();

private:
	// Time system state
	double tick_time = 1.0;
	int current_tick = 0;
	double time_scale = 1.0;
	double accumulated_time = 0.0;
	double last_physics_time = 0.0;
	
	// Helper classes for internal organization
	TimeUnitManager unit_manager;
	TimeUnitProcessor *processor = nullptr;
	
	// Status flags
	bool paused = false;
	bool initialized = false;
	bool connected_to_physics = false;
	
	// Internal processing
	void _on_physics_frame();
	void _process_tick(double delta);
	void _increment_unit(const String &unit_name);
	void _decrement_unit(const String &unit_name);
	
	// Signal emission helper (called by processor)
	void _emit_unit_changed(const String &name, int new_val, int old_val);
};

