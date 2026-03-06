#pragma once
#include <string>

// IModifier — base interface for all score modifiers.
// Follows the Decorator concept: each modifier wraps and transforms a score.
class IModifier {
public:
    virtual ~IModifier() = default;

    // Apply this modifier to a score and return the new score.
    virtual int apply(int score) const = 0;

    // Human-readable name of this modifier.
    virtual std::string getName() const = 0;

    // Clone this modifier (Prototype pattern — conceptual implementation).
    virtual IModifier* clone() const = 0;
};
