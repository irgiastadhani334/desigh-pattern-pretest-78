# Card-Run Game — Design Pattern Pretest
**Course:** Design Pattern untuk Game  
**Language:** C++ (C++17)  
**Build:** `g++ -std=c++17 src/main.cpp src/RunSession.cpp src/ScoringSystem.cpp src/ShopSystem.cpp src/ModifierFactory.cpp -I src -o card_run_game`

---

## Architecture Overview

The project is split into five core components, each with a single clear responsibility:

**`RunSession`** is the top-level orchestrator. It owns all dynamically allocated objects (`ScoringSystem`, `ShopSystem`, modifier instances) and is responsible for their cleanup. It runs the main game loop: draw hand → score → shop → repeat.

**`ScoringSystem`** applies a scoring strategy to a hand of cards and then runs the result through a chain of modifiers. It holds a pointer to an `IScoringStrategy` (which can be swapped at runtime) and a list of `IModifier*` objects that it applies in order.

**`ShopSystem`** manages the player's gold and presents available modifier options between rounds. It delegates all modifier creation to `ModifierFactory`.

**`ModifierFactory`** is the single point of modifier object creation. It maps string type names (`"double"`, `"flat"`, `"square"`) to concrete `IModifier` instances.

**`IModifier` / concrete modifiers** form the modifier hierarchy. Each concrete class (`DoubleScoreModifier`, `FlatBonusModifier`, `SquareScoreModifier`) implements `apply(score)`, `getName()`, and `clone()`.

---

## Pattern Usage

### Creational — Factory
`ModifierFactory::create(type)` in `src/ModifierFactory.cpp` is the sole constructor of all `IModifier` objects. Callers (`ShopSystem`) never use `new DoubleScoreModifier()` directly. This centralises creation so adding a new modifier only touches `ModifierFactory.cpp` and the new class file.

### Creational — Prototype
Every `IModifier` subclass implements `clone()` which returns a deep copy via the copy constructor. This enables future registry-based creation without rebuilding objects from string keys.

### Structural — Decorator (modifier chain)
`ScoringSystem::calculate()` pipes the base score through `mod->apply(score)` for each modifier in sequence. Each modifier "wraps" the previous score exactly as a Decorator wraps a Component. The chain is: `baseScore → modifier1 → modifier2 → ... → finalScore`.

### Behavioral — Strategy
`IScoringStrategy` defines the base-score algorithm. `StandardScoring` and `HighCardScoring` are two concrete strategies. `ScoringSystem::setStrategy()` allows the rule to be swapped at runtime without changing any other code.

---

## Modification Log — Adding `SquareScoreModifier`

1. **Created** `src/modifiers/SquareScoreModifier.h`  
   Implemented `apply(score)` as `score * score`, `getName()` returns `"SquareBoost (^2)"`, and `clone()` returns a copy-constructed instance.

2. **Modified** `src/ModifierFactory.cpp`  
   Added `#include "modifiers/SquareScoreModifier.h"` and one `if` branch:
   ```cpp
   if (type == "square") return new SquareScoreModifier();
   ```

3. **Modified** `src/ShopSystem.cpp`  
   Added `square` to the display menu with a cost of 50g.

No other files were changed. The `IModifier` interface, `ScoringSystem`, `RunSession`, and all existing modifier classes were untouched — demonstrating that the architecture is open for extension and closed for modification.

---

## AI Usage Disclosure

AI (Claude) was used as a coding assistant in this project. Specifically:

- AI helped draft the initial class structure and file layout based on the assignment requirements.
- AI suggested the use of `IScoringStrategy` as a Strategy interface and the `printBreakdown` method for score transparency.
- AI provided the `clone()` boilerplate pattern for the Prototype concept.

All code was reviewed, understood, and is explainable by the student. The student can describe the role of every class, every method, and every design pattern decision present in the codebase.
