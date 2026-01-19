// MIT License
// Copyright (c) 2025 Lucas "Shoyguer" Melo

#include "seed.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/classes/time.hpp"

using namespace godot;


// Generates a random seed string using the specified type and length
String Seed::generate_seed_string(SeedType seed_type, int max_seed_length)
{
    String new_seed = "";

    Ref<RandomNumberGenerator> aux_rng;
    aux_rng.instantiate();

    // Prefix Random number to be hashed to the aux_rng seed
    int prefix_random = aux_rng->randi_range(1000, 9999);

    // This section I decided to get the most random as possible for the aux_rng seed.
    // It uses a combination of random numbers and the current date to create a unique seed.
    // This way, even if the function is called multiple times in quick succession,
    // the likelihood of generating the same seed is basically impossible.
    
    // Get current date in YYYYMMDDHHMMSSms format
    Dictionary datetime = Time::get_singleton()->get_datetime_dict_from_system();
    int year = datetime["year"];
    int month = datetime["month"];
    int day = datetime["day"];
    int hour = datetime["hour"];
    int minute = datetime["minute"];
    int second = datetime["second"];
    
    // Get milliseconds from current time
    uint64_t current_time_ms = Time::get_singleton()->get_ticks_msec();
    int millisecond = current_time_ms % 1000;
    
    String date_string = 
        String::num_int64(year) + 
        String::num_int64(month).pad_zeros(2) + 
        String::num_int64(day).pad_zeros(2) +
        String::num_int64(hour).pad_zeros(2) +
        String::num_int64(minute).pad_zeros(2) +
        String::num_int64(second).pad_zeros(2) +
        String::num_int64(millisecond).pad_zeros(3);
    
    // Suffix Random number to be hashed to the aux_rng seed
    int suffix_random = aux_rng->randi_range(1000, 9999);
    
    // Combine into final hash string
    String hash_string = String::num_int64(prefix_random) + date_string + String::num_int64(suffix_random);
    
    // Hashes the string to set the aux_rng seed
    aux_rng->set_seed(hash_string.hash());
    
    for (int i = 0; i < max_seed_length; i++) {
        Array char_types = get_char_types(seed_type);
        int type = char_types[aux_rng->randi_range(0, char_types.size() - 1)];
        new_seed += get_char(static_cast<CharType>(type), aux_rng);
    }

    return new_seed;
}

// Creates an RNG instance with the provided seed (or generates one if empty)
Ref<RandomNumberGenerator> Seed::generate_rng(const String &new_seed, SeedType seed_type, int max_seed_length)
{
    String final_seed = "";
    
    // If no seed is provided, generate a new one.
    if (new_seed == "") {
        final_seed = generate_seed_string(seed_type, max_seed_length);
    }
    // If seed provided, set it to final_seed.
    else {
        final_seed = new_seed;
    }
    
    // Create new RNG and hash the seed
    Ref<RandomNumberGenerator> new_rng;
    new_rng.instantiate();
    
    return rng_hash_seed(new_rng, final_seed);
}

// Hashes the seed string and applies it to the RNG instance
Ref<RandomNumberGenerator> Seed::rng_hash_seed(Ref<RandomNumberGenerator> new_rng, const String &new_seed)
{
    if (new_rng.is_valid()) {
        new_rng->set_seed(new_seed.hash());
    }
    return new_rng;
}

// Returns available character types based on the seed type
Array Seed::get_char_types(SeedType seed_type)
{
    Array types;
    
    switch (seed_type) {
        case NUMBERS:
            types.push_back(NUMBER);
            break;
        case CAPITAL_ONLY:
            types.push_back(CAPITAL);
            break;
        case LOWER_ONLY:
            types.push_back(LOWER);
            break;
        case ALL_LETTERS:
            types.push_back(CAPITAL);
            types.push_back(LOWER);
            break;
        case ALL_MIXED:
            types.push_back(NUMBER);
            types.push_back(CAPITAL);
            types.push_back(LOWER);
            break;
    }
    
    return types;
}

// Generates a random character based on the specified character type
String Seed::get_char(CharType type, Ref<RandomNumberGenerator> aux_rng)
{
    String string_char = "";
    
    switch (type) {
        case NUMBER:
            // UNICODE 0-9 (48-57)
            string_char = String::chr(aux_rng->randi_range(48, 57));
            break;
        case CAPITAL:
            // UNICODE A-Z (65-90)
            string_char = String::chr(aux_rng->randi_range(65, 90));
            break;
        case LOWER:
            // UNICODE a-z (97-122)
            string_char = String::chr(aux_rng->randi_range(97, 122));
            break;
    }
    
    return string_char;
}

// Binds methods and enums to Godot's ClassDB for GDScript/C# access
void Seed::_bind_methods()
{
    ClassDB::bind_static_method("Seed", D_METHOD("generate_seed_string", "seed_type", "max_seed_length"), &Seed::generate_seed_string, DEFVAL(ALL_MIXED), DEFVAL(12));
    ClassDB::bind_static_method("Seed", D_METHOD("generate_rng", "new_seed", "seed_type", "max_seed_length"), &Seed::generate_rng, DEFVAL(""), DEFVAL(ALL_MIXED), DEFVAL(12));

    BIND_ENUM_CONSTANT(NUMBERS);
    BIND_ENUM_CONSTANT(CAPITAL_ONLY);
    BIND_ENUM_CONSTANT(LOWER_ONLY);
    BIND_ENUM_CONSTANT(ALL_LETTERS);
    BIND_ENUM_CONSTANT(ALL_MIXED);
}

VARIANT_ENUM_CAST(Seed::SeedType);