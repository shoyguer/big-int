// MIT License
// Copyright (c) 2025 Lucas "Shoyguer" Melo

#include "stat_pool.hpp"
#include "godot_cpp/core/class_db.hpp"

using namespace godot;


// Initializes StatPool with default values
StatPool::StatPool() : min_value(0), max_value(100), value(100)
{
    // Default initialization with min_value = 0, max_value = 100, value = 100
}

// Sets minimum value, auto-adjusts max if needed, and clamps current value
void StatPool::set_min_value(int new_min)
{
    if (new_min == min_value) return;
    
    int old_min = min_value;
    min_value = new_min;

    // Ensure min_value does not exceed max_value, nor is equal to it
    if (min_value >= max_value) {
        set_max_value(min_value + 1);
    }
    
    // Clamp current value to new minimum
    if (value < min_value) {
        set_value(min_value);
    }
    
    bool increased = min_value > old_min;
    emit_signal("min_value_changed", old_min, min_value, increased);
}

// Sets maximum value, auto-adjusts min if needed, and clamps current value
void StatPool::set_max_value(int new_max) 
{
    if (new_max == max_value) return;
    
    int old_max = max_value;
    max_value = new_max;

    // Ensures max_value does not fall below min_value, nor is equal to it
    if (max_value <= min_value) {
        set_min_value(max_value - 1);
    }

    // Clamp current value to new maximum
    if (value > max_value) {
        set_value(max_value);
    }
    
    bool increased = max_value > old_max;
    emit_signal("max_value_changed", old_max, max_value, increased);
}

// Sets current value with clamping and emits appropriate signals
void StatPool::set_value(int new_value) 
{
    if (new_value == value) return;
    
    int old_value = value;
    value = clamp_value(new_value, min_value, max_value);
    
    bool increased = value > old_value;
    emit_signal("value_changed", old_value, value, increased);
    
    if (value == min_value && old_value != min_value) {
        emit_signal("depleted", this);
    } else if (value > min_value && old_value == min_value) {
        emit_signal("restored", this);
    } else if (value == max_value && old_value != max_value) {
        emit_signal("restored_fully", this);
    }
}

// Increases value by specified amount
void StatPool::increase(int amount) 
{
    set_value(value + amount);
}

// Decreases value by specified amount
void StatPool::decrease(int amount) 
{
    set_value(value - amount);
}

// Sets value to maximum
void StatPool::fill() 
{
    set_value(max_value);
}

// Sets value to minimum
void StatPool::deplete() 
{
    set_value(min_value);
}

// Returns current value as percentage (0.0 to 1.0) between min and max
float StatPool::get_percentage() const 
{
    int range = max_value - min_value;
    return static_cast<float>(value - min_value) / static_cast<float>(range);
}

// Clamps value between min and max bounds
int StatPool::clamp_value(int val, int min_val, int max_val) const 
{
    if (val < min_val) return min_val;
    if (val > max_val) return max_val;
    return val;
}

// Registers methods, properties, and signals for Godot access
void StatPool::_bind_methods() 
{
    // Methods
    ClassDB::bind_method(D_METHOD("set_min_value", "new_min"), &StatPool::set_min_value);
    ClassDB::bind_method(D_METHOD("get_min_value"), &StatPool::get_min_value);
    
    ClassDB::bind_method(D_METHOD("set_max_value", "new_max"), &StatPool::set_max_value);
    ClassDB::bind_method(D_METHOD("get_max_value"), &StatPool::get_max_value);
    
    ClassDB::bind_method(D_METHOD("set_value", "new_value"), &StatPool::set_value);
    ClassDB::bind_method(D_METHOD("get_value"), &StatPool::get_value);
    
    ClassDB::bind_method(D_METHOD("increase", "amount"), &StatPool::increase);
    ClassDB::bind_method(D_METHOD("decrease", "amount"), &StatPool::decrease);
    ClassDB::bind_method(D_METHOD("fill"), &StatPool::fill);
    ClassDB::bind_method(D_METHOD("deplete"), &StatPool::deplete);
    
    // Property getters
    ClassDB::bind_method(D_METHOD("is_depleted"), &StatPool::is_depleted);
    ClassDB::bind_method(D_METHOD("is_filled"), &StatPool::is_filled);
    ClassDB::bind_method(D_METHOD("get_percentage"), &StatPool::get_percentage);

    // Properties
    ADD_PROPERTY(PropertyInfo(Variant::INT, "min_value"), "set_min_value", "get_min_value");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "max_value"), "set_max_value", "get_max_value");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "value"), "set_value", "get_value");

    // Signals
    ADD_SIGNAL(MethodInfo("min_value_changed",
        PropertyInfo(Variant::INT, "old_min"),
        PropertyInfo(Variant::INT, "new_min"),
        PropertyInfo(Variant::BOOL, "increased")));
        
    ADD_SIGNAL(MethodInfo("max_value_changed",
        PropertyInfo(Variant::INT, "old_max"),
        PropertyInfo(Variant::INT, "new_max"), 
        PropertyInfo(Variant::BOOL, "increased")));
        
    ADD_SIGNAL(MethodInfo("value_changed",
        PropertyInfo(Variant::INT, "old_value"),
        PropertyInfo(Variant::INT, "new_value"), 
        PropertyInfo(Variant::BOOL, "increased")));
        
    ADD_SIGNAL(MethodInfo("depleted",
        PropertyInfo(Variant::OBJECT, "stat", PROPERTY_HINT_RESOURCE_TYPE, "StatPool")));
        
    ADD_SIGNAL(MethodInfo("restored",
        PropertyInfo(Variant::OBJECT, "stat", PROPERTY_HINT_RESOURCE_TYPE, "StatPool")));
        
    ADD_SIGNAL(MethodInfo("restored_fully",
        PropertyInfo(Variant::OBJECT, "stat", PROPERTY_HINT_RESOURCE_TYPE, "StatPool")));
}
