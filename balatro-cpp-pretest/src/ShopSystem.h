#pragma once
#include "ModifierFactory.h"
#include "modifiers/IModifier.h"
#include <vector>
#include <string>

// ShopSystem — lets the player browse and buy modifiers between rounds.
// Uses ModifierFactory internally (Factory pattern).
class ShopSystem {
private:
    int gold;

public:
    explicit ShopSystem(int startingGold = 100);

    void displayShop() const;

    // Attempt to purchase a modifier. Returns new modifier or nullptr.
    IModifier* tryBuy(const std::string& type, int cost = 30);

    int getGold() const { return gold; }
};
