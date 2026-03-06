#include "ModifierFactory.h"
#include "modifiers/DoubleScoreModifier.h"
#include "modifiers/FlatBonusModifier.h"
#include "modifiers/SquareScoreModifier.h"

IModifier* ModifierFactory::create(const std::string& type, int param) {
    if (type == "double")  return new DoubleScoreModifier();
    if (type == "flat")    return new FlatBonusModifier(param > 0 ? param : 50);
    if (type == "square")  return new SquareScoreModifier();
    return nullptr;
}

std::vector<std::string> ModifierFactory::availableTypes() {
    return { "double", "flat", "square" };
}
