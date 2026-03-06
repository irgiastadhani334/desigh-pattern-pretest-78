#include "ScoringSystem.h"
#include <iostream>
#include <iomanip>

ScoringSystem::ScoringSystem(IScoringStrategy* strat) : strategy(strat) {}

ScoringSystem::~ScoringSystem() {
    // strategy and modifiers are owned by RunSession — do not delete here.
}

void ScoringSystem::setStrategy(IScoringStrategy* strat) {
    strategy = strat;
}

int ScoringSystem::calculate(const std::vector<std::string>& hand) const {
    int score = strategy->baseScore(hand);
    for (IModifier* mod : modifiers) {
        score = mod->apply(score);
    }
    return score;
}

void ScoringSystem::printBreakdown(const std::vector<std::string>& hand) const {
    std::cout << "  Scoring rule  : " << strategy->name() << "\n";
    int score = strategy->baseScore(hand);
    std::cout << "  Base score    : " << score << "\n";

    for (IModifier* mod : modifiers) {
        int prev = score;
        score = mod->apply(score);
        std::cout << "  [" << std::setw(20) << std::left << mod->getName() << "] "
                  << prev << " -> " << score << "\n";
    }
    std::cout << "  Final score   : " << score << "\n";
}

void ScoringSystem::addModifier(IModifier* mod) {
    modifiers.push_back(mod);
}
