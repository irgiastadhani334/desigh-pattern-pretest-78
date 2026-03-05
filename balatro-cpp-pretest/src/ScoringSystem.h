#pragma once
#include "modifiers/IModifier.h"
#include <vector>
#include <string>

// IScoringStrategy — Strategy Pattern interface.
// Swapping this allows different scoring rules without touching ScoringSystem.
class IScoringStrategy {
public:
    virtual ~IScoringStrategy() = default;
    virtual int baseScore(const std::vector<std::string>& hand) const = 0;
    virtual std::string name() const = 0;
};

// StandardScoring — default strategy: 10 points per card.
class StandardScoring : public IScoringStrategy {
public:
    int baseScore(const std::vector<std::string>& hand) const override {
        return static_cast<int>(hand.size()) * 10;
    }
    std::string name() const override { return "Standard (10 pts/card)"; }
};

// HighCardScoring — alternative strategy: 25 points flat regardless of count.
class HighCardScoring : public IScoringStrategy {
public:
    int baseScore(const std::vector<std::string>& hand) const override {
        return 25;
    }
    std::string name() const override { return "HighCard (25 pts flat)"; }
};

// ScoringSystem — applies a strategy then runs the modifier chain.
// The modifier chain behaves like a Decorator pattern:
// each modifier wraps the previous score and transforms it.
class ScoringSystem {
private:
    IScoringStrategy* strategy;
    std::vector<IModifier*> modifiers;  // owned externally (RunSession)

public:
    explicit ScoringSystem(IScoringStrategy* strat);
    ~ScoringSystem();

    void setStrategy(IScoringStrategy* strat);

    // Calculate final score for a hand with the current modifier chain.
    int calculate(const std::vector<std::string>& hand) const;

    // Print a step-by-step breakdown of score calculation.
    void printBreakdown(const std::vector<std::string>& hand) const;

    // Attach a modifier (does NOT take ownership).
    void addModifier(IModifier* mod);

    const std::vector<IModifier*>& getModifiers() const { return modifiers; }
};
