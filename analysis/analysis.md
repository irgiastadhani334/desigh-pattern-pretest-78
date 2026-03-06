# Design Pattern Analysis
## Card-Run Game — Pretest Submission

---

## PART A — Creational Pattern Analysis

### A1 — Factory

**Where are modifier objects created?**

All modifier objects are created in `ModifierFactory::create()`, located in `src/ModifierFactory.cpp`:

```cpp
IModifier* ModifierFactory::create(const std::string& type, int param) {
    if (type == "double")  return new DoubleScoreModifier();
    if (type == "flat")    return new FlatBonusModifier(param > 0 ? param : 50);
    if (type == "square")  return new SquareScoreModifier();
    return nullptr;
}
```

**Which class creates them:** `ModifierFactory` (static method `create`).

**Why centralising creation is beneficial:**  
Without a factory, every caller that needs a modifier — `ShopSystem`, `RunSession`, tests — would write `new DoubleScoreModifier()` directly. That means the concrete class name is scattered across many files. When a new modifier is added or a class is renamed, every one of those callsites must change. By funnelling creation through `ModifierFactory::create`, only *one file* (`ModifierFactory.cpp`) needs to change. The factory also makes it trivial to add constructor parameters (e.g. the `param` argument for `FlatBonusModifier`) in a single place.

**Reference:** `src/ModifierFactory.h` and `src/ModifierFactory.cpp`, method `ModifierFactory::create`.

---

### A2 — Extensibility

**How `SquareScoreModifier` was added:**

1. Created `src/modifiers/SquareScoreModifier.h` — the new concrete class implementing `IModifier`.
2. Added one `#include` and one `if` branch in `src/ModifierFactory.cpp`:
   ```cpp
   if (type == "square")  return new SquareScoreModifier();
   ```
3. Added one entry to `ShopSystem`'s display menu in `src/ShopSystem.cpp`.

**Files changed:** `src/modifiers/SquareScoreModifier.h` (new), `src/ModifierFactory.cpp` (2 lines), `src/ShopSystem.cpp` (1 line).

**Was the design easy to extend?**  
Yes. The `IModifier` interface meant the new class needed to implement only `apply()`, `getName()`, and `clone()`. The factory's string-dispatch pattern made registration trivial. No existing modifier class was touched. This demonstrates the **Open/Closed Principle** — open for extension, closed for modification.

---

### A3 — Factory Method (Conceptual)

If there were multiple shop types (e.g. `WeaponShop`, `BuffShop`, `DebuffShop`), a **Factory Method** would let each shop subclass decide *which kind* of modifier it produces, without the caller needing to know.

Example interface:

```cpp
// Abstract creator
class ShopBase {
public:
    virtual IModifier* createModifier() = 0;   // Factory Method
    void offerModifier(ScoringSystem& s) {
        IModifier* m = createModifier();        // calls subclass
        s.addModifier(m);
    }
};

// Concrete creator A — only sells double/flat boosts
class BuffShop : public ShopBase {
public:
    IModifier* createModifier() override {
        return ModifierFactory::create("double");
    }
};

// Concrete creator B — only sells square (risky) boosts
class HighRiskShop : public ShopBase {
public:
    IModifier* createModifier() override {
        return ModifierFactory::create("square");
    }
};
```

The `offerModifier` logic in the base class is reused; only `createModifier()` varies per subclass. This separates the *decision of what to create* from the *act of using the created object*.

---

### A4 — Prototype (Conceptual)

**How `clone()` could help:**

The `IModifier` interface already declares `virtual IModifier* clone() const = 0`, and every concrete class implements it:

```cpp
// DoubleScoreModifier.h
IModifier* clone() const override {
    return new DoubleScoreModifier(*this);
}
```

**Where `clone()` would be used:**  
If the shop needs to sell the same modifier type multiple times without reconstructing it from a string key, it could keep a *prototype registry*: a `std::map<std::string, IModifier*>` of pre-built instances. To produce a new modifier it calls `prototype->clone()` rather than `new ConcreteModifier()`. This is especially useful if construction is expensive (e.g. loading configuration from disk).

```cpp
// Prototype registry sketch
IModifier* proto = registry["double"];  // pre-built prototype
IModifier* copy  = proto->clone();      // cheap copy
```

---

### A5 — Singleton (Conceptual)

**Did the implementation use global objects?**  
No. `RunSession` constructs `ScoringSystem`, `ShopSystem`, and `IScoringStrategy` as owned members and passes references explicitly.

**Advantages of Singleton (if applied to e.g. `ModifierFactory`):**  
- Guarantees one shared instance — useful if the factory loads modifier metadata from a file at startup.
- Accessible globally without passing a reference through every function call.

**Risks:**  
- Hidden dependency — callers silently rely on a global state, making code harder to test in isolation.
- Thread-safety — concurrent creation of the instance in a multi-threaded game loop requires locks.
- Lifetime ambiguity — destruction order of global/static objects is undefined across translation units.

For a small educational project, a class with only `static` methods (as implemented here) achieves the uniqueness goal without the lifetime risks.

---

## PART B — Structural Pattern Analysis

### B1 — Decorator Concept

**What is being decorated?**  
The integer score value is "decorated" — each modifier in the chain wraps the output of the previous step and transforms it before passing it on.

**How the modifier chain behaves like Decorator:**

In the classic Decorator pattern, a `ConcreteDecorator` holds a reference to a `Component` and calls `component->operation()` before adding its own behaviour. Here, each `IModifier::apply(score)` takes the previous score (the "component output") and returns a transformed version:

```cpp
// ScoringSystem.cpp — calculate()
int score = strategy->baseScore(hand);  // base component
for (IModifier* mod : modifiers) {
    score = mod->apply(score);           // each modifier decorates the result
}
```

The chain is: `baseScore → mod1.apply → mod2.apply → ... → finalScore`. Adding a new modifier to the chain is identical to wrapping a Decorator around an existing object — no existing code changes.

**Reference:** `src/ScoringSystem.cpp`, method `ScoringSystem::calculate` and `ScoringSystem::printBreakdown`.

---

### B2 — Modifier Order

**Does modifier order affect the score?**  
Yes, significantly. Because modifiers are applied sequentially, the order changes the result when both multiplicative and additive modifiers are present.

**Example using the code:**

Assume `base = 50`, modifiers = `[FlatBonus(+50), DoubleScore(x2)]`:
```
50 + 50 = 100, then 100 * 2 = 200
```

Reverse order `[DoubleScore(x2), FlatBonus(+50)]`:
```
50 * 2 = 100, then 100 + 50 = 150
```

The difference is **50 points**. In `ScoringSystem::addModifier`, modifiers are appended to the back of `std::vector<IModifier*> modifiers`, so **purchase order determines application order**. The `printBreakdown` method in `src/ScoringSystem.cpp` shows each step explicitly so the player can observe this.

---

### B3 — Composite (Conceptual)

A `CompositeModifier` would treat a *group* of modifiers as a single modifier — useful for selling "modifier bundles" in the shop.

```cpp
class CompositeModifier : public IModifier {
    std::vector<IModifier*> children;
public:
    void add(IModifier* m) { children.push_back(m); }

    int apply(int score) const override {
        for (auto* m : children) score = m->apply(score);
        return score;
    }
    std::string getName() const override { return "Bundle"; }
    IModifier* clone() const override { /* deep-clone children */ }
};
```

`ShopSystem` can then sell a `CompositeModifier` as a single item, and `ScoringSystem` treats it like any other `IModifier`. The caller never knows it contains multiple children — that is the essence of the Composite pattern.

---

### B4 — Adapter (Conceptual)

Suppose an external C library `LegacyScorer` is available:
```c
int legacy_compute(int cards[], int n);  // C API, not compatible with IScoringStrategy
```

An Adapter bridges it:

```cpp
class LegacyScorerAdapter : public IScoringStrategy {
public:
    int baseScore(const std::vector<std::string>& hand) const override {
        // Convert hand to int array expected by legacy API
        std::vector<int> arr(hand.size(), 10);
        return legacy_compute(arr.data(), static_cast<int>(arr.size()));
    }
    std::string name() const override { return "LegacyScorer (adapted)"; }
};
```

`RunSession` calls `scorer->setStrategy(new LegacyScorerAdapter())` and the rest of the system is unaware any adaptation is happening.

---

### B5 — Facade (Conceptual)

A `GameFacade` would simplify starting and interacting with the game for an external caller (e.g. a UI layer or a test harness) that does not want to know about `RunSession`, `ScoringSystem`, or `ShopSystem` individually.

```cpp
class GameFacade {
    RunSession* session;
public:
    GameFacade()  { session = new RunSession(3); }
    ~GameFacade() { delete session; }

    void startGame()            { session->start(); }
    int  getCurrentScore()      { return session->getTotalScore(); }
    void purchaseModifier(const std::string& type) { session->buyModifier(type); }
    void advanceRound()         { session->playRound(); }
};
```

The facade exposes only: `startGame`, `getCurrentScore`, `purchaseModifier`, `advanceRound`. All internal wiring remains hidden.

---

## PART C — Behavioral Pattern Analysis

### C1 — Strategy

**Where scoring logic exists:**

The scoring logic is split into two parts. The *base score* computation lives in `IScoringStrategy` subclasses (`StandardScoring`, `HighCardScoring`) in `src/ScoringSystem.h`. The *modifier chain* application lives in `ScoringSystem::calculate()` in `src/ScoringSystem.cpp`.

**How to support multiple scoring rules:**

`ScoringSystem` holds a pointer `IScoringStrategy* strategy`. To swap rules at runtime:

```cpp
scorer->setStrategy(new HighCardScoring());
```

This is the Strategy pattern: the algorithm (scoring rule) is encapsulated in an interchangeable object. Adding a new rule (e.g. `PairBonusScoring` that gives extra points for pairs) only requires a new class that implements `IScoringStrategy::baseScore` — no changes to `ScoringSystem`.

---

### C2 — Observer (Conceptual)

**Events in the system that could benefit from Observer:**

| Event | Potential listeners |
|---|---|
| Modifier purchased | Achievement tracker, UI updater |
| Round completed | Statistics recorder, difficulty scaler |
| Score calculated | High-score logger, analytics |

**How Observer would improve design:**

Currently, `RunSession::start()` is responsible for printing everything — it mixes game logic with output. With Observer, `RunSession` would emit events:

```cpp
// Subject emits events
void RunSession::onRoundComplete(int score) {
    for (auto* obs : observers) obs->onRoundComplete(score);
}

// Listeners implement
class ConsoleLogger : public IGameObserver {
    void onRoundComplete(int score) override {
        std::cout << "Round done. Score: " << score << "\n";
    }
};
```

This separates concerns: `RunSession` handles game state, observers handle reactions. Adding a file logger or analytics observer requires no changes to `RunSession`.

---

### C3 — Command (Conceptual)

**Actions that could be Command objects:**

- **PlayHand** — draw and score a hand
- **BuyModifier(type)** — shop purchase
- **SkipShop** — skip purchase phase
- **UndoLastModifier** — remove the most recently added modifier

Wrapping actions as Command objects enables:
- **Undo/Redo** — store a history stack and call `command->undo()`
- **Replay** — record all commands and replay the run deterministically
- **Macro commands** — batch multiple actions into one

```cpp
class BuyModifierCommand : public ICommand {
    ShopSystem& shop;
    ScoringSystem& scorer;
    std::string type;
    IModifier* purchased = nullptr;
public:
    void execute() override { purchased = shop.tryBuy(type); scorer.addModifier(purchased); }
    void undo()    override { scorer.removeModifier(purchased); delete purchased; }
};
```

---

### C4 — State (Conceptual)

**How State pattern could organize run phases:**

The run currently progresses linearly through an enum-less sequence. A State pattern would make each phase an object:

```cpp
class IRunState {
public:
    virtual void enter(RunSession& s) = 0;
    virtual void update(RunSession& s) = 0;
    virtual IRunState* next() = 0;
};

class PlayHandState  : public IRunState { ... };
class ScoreState     : public IRunState { ... };
class ShopState      : public IRunState { ... };
class EndRunState    : public IRunState { ... };
```

`RunSession` holds `IRunState* currentState` and calls `currentState->update(*this)` each tick. Transitioning is `currentState = currentState->next()`. Benefits: each state's logic is isolated, transitions are explicit and testable, and adding a new phase (e.g. `BossRoundState`) does not require modifying existing state classes.
