#pragma once
#include "IModifier.h"
#include <cmath>

// SquareScoreModifier — squares the current score.
// Added to demonstrate extensibility of the Factory + IModifier design.
// Only this file and ModifierFactory.cpp needed to change.
class SquareScoreModifier : public IModifier {
public:
    int apply(int score) const override {
        return score * score;
    }

    std::string getName() const override {
        return "SquareBoost (^2)";
    }

    IModifier* clone() const override {
        return new SquareScoreModifier(*this);
    }
};
