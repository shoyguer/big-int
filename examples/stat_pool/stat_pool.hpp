// MIT License
// Copyright (c) 2025 Lucas "Shoyguer" Melo

#pragma once

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/classes/wrapped.hpp"

namespace godot {

class StatPool : public Resource {
    GDCLASS(StatPool, Resource)

public:
    StatPool();

    // Value Management
    void set_min_value(int new_min);
    int get_min_value() const { return min_value; }

    void set_max_value(int new_max);
    int get_max_value() const { return max_value; }
    
    void set_value(int new_value);
    int get_value() const { return value; }
    
    void increase(int amount);
    void decrease(int amount);
    void fill();
    void deplete();

    // Properties
    bool is_depleted() const { return value == min_value; }
    bool is_filled() const { return value == max_value; }
    float get_percentage() const;

protected:
    static void _bind_methods();

private:
    // Member variables
    int min_value;
    int max_value;
    int value;

    int clamp_value(int val, int min_val, int max_val) const;
};

}
