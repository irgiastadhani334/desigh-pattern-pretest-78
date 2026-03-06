#pragma once
#include "IModifier.h"

// DoubleScoreModifier — multiplies the current score by 2.
// Concrete modifier A.
class DoubleScoreModifier : public IModifier {
public:
    int apply(int score) const override {
        return score * 2;
    }

    std::string getName() const override {
        return "DoubleBoost (x2)";
    }

    IModifier* clone() const override {
        return new DoubleScoreModifier(*this);
    }
};
