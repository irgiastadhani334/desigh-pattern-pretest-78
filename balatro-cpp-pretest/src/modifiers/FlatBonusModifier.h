#pragma once
#include "IModifier.h"

// FlatBonusModifier — adds a fixed bonus value to the score.
// Concrete modifier B.
class FlatBonusModifier : public IModifier {
private:
    int bonusValue;

public:
    explicit FlatBonusModifier(int bonus) : bonusValue(bonus) {}

    int apply(int score) const override {
        return score + bonusValue;
    }

    std::string getName() const override {
        return "FlatBoost (+" + std::to_string(bonusValue) + ")";
    }

    IModifier* clone() const override {
        return new FlatBonusModifier(*this);
    }
};
