#include "ShopSystem.h"
#include <iostream>
#include <iomanip>

ShopSystem::ShopSystem(int startingGold) : gold(startingGold) {}

void ShopSystem::displayShop() const {
    std::cout << "\n  +--------------------------+\n";
    std::cout << "  |        SHOP              |\n";
    std::cout << "  | Gold: " << std::setw(4) << gold << "                 |\n";
    std::cout << "  +--------------------------+\n";
    std::cout << "  | [1] double  — x2 score   | 30g\n";
    std::cout << "  | [2] flat    — +50 score  | 20g\n";
    std::cout << "  | [3] square  — ^2 score   | 50g\n";
    std::cout << "  | [0] skip shop            |\n";
    std::cout << "  +--------------------------+\n";
}

IModifier* ShopSystem::tryBuy(const std::string& type, int cost) {
    if (gold < cost) {
        std::cout << "  Not enough gold!\n";
        return nullptr;
    }
    IModifier* mod = ModifierFactory::create(type);
    if (!mod) {
        std::cout << "  Unknown modifier type.\n";
        return nullptr;
    }
    gold -= cost;
    std::cout << "  Purchased: " << mod->getName()
              << " (cost " << cost << "g, remaining: " << gold << "g)\n";
    return mod;
}
