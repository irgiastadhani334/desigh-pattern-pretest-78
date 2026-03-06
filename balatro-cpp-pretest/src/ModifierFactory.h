#pragma once
#include "modifiers/IModifier.h"
#include <string>
#include <vector>

// ModifierFactory — Creational (Factory) Pattern.
// Centralises modifier object creation so the rest of the system
// never needs to know concrete class names.
class ModifierFactory {
public:
    // Create a modifier by name. Returns nullptr for unknown names.
    static IModifier* create(const std::string& type, int param = 0);

    // Return the list of modifier type names available in the shop.
    static std::vector<std::string> availableTypes();
};
