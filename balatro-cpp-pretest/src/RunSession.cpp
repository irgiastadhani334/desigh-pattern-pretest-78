#include "RunSession.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <algorithm>

static const std::vector<std::string> DECK = {
    "2♥","3♥","4♥","5♥","6♥","7♥","8♥","9♥","10♥","J♥","Q♥","K♥","A♥",
    "2♦","3♦","4♦","5♦","6♦","7♦","8♦","9♦","10♦","J♦","Q♦","K♦","A♦",
    "2♣","3♣","4♣","5♣","6♣","7♣","8♣","9♣","10♣","J♣","Q♣","K♣","A♣",
    "2♠","3♠","4♠","5♠","6♠","7♠","8♠","9♠","10♠","J♠","Q♠","K♠","A♠"
};

// ---- helpers ----

std::vector<std::string> RunSession::drawHand(int size) const {
    std::vector<std::string> deck = DECK;
    // Fisher-Yates shuffle
    for (int i = static_cast<int>(deck.size()) - 1; i > 0; --i) {
        int j = std::rand() % (i + 1);
        std::swap(deck[i], deck[j]);
    }
    return { deck.begin(), deck.begin() + size };
}

void RunSession::displayHand(const std::vector<std::string>& hand) const {
    std::cout << "  Hand: ";
    for (size_t i = 0; i < hand.size(); ++i) {
        std::cout << hand[i];
        if (i + 1 < hand.size()) std::cout << "  ";
    }
    std::cout << "\n";
}

// ---- shop phase ----

void RunSession::runShop() {
    shop->displayShop();
    std::cout << "  Your choice: ";
    int choice;
    std::cin >> choice;

    struct Item { std::string type; int cost; };
    std::vector<Item> items = { {"double",30}, {"flat",20}, {"square",50} };

    if (choice >= 1 && choice <= 3) {
        auto& item = items[choice - 1];
        IModifier* mod = shop->tryBuy(item.type, item.cost);
        if (mod) {
            scorer->addModifier(mod);
            ownedModifiers.push_back(mod);   // take ownership
        }
    } else {
        std::cout << "  Skipped shop.\n";
    }
}

// ---- constructor / destructor ----

RunSession::RunSession(int rounds)
    : roundNumber(0), totalScore(0), maxRounds(rounds)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    strategy = new StandardScoring();
    scorer   = new ScoringSystem(strategy);
    shop     = new ShopSystem(100);
}

RunSession::~RunSession() {
    for (IModifier* m : ownedModifiers) delete m;
    delete scorer;
    delete shop;
    delete strategy;
}

// ---- main game loop ----

void RunSession::start() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════╗\n";
    std::cout << "║        CARD-RUN GAME  v1.0           ║\n";
    std::cout << "╚══════════════════════════════════════╝\n";
    std::cout << "  Rounds to play: " << maxRounds << "\n\n";

    for (roundNumber = 1; roundNumber <= maxRounds; ++roundNumber) {
        std::cout << "┌──────────────────────────────────────┐\n";
        std::cout << "│  ROUND " << roundNumber
                  << " / " << maxRounds
                  << "                             │\n";
        std::cout << "└──────────────────────────────────────┘\n";

        // --- Play Hand phase ---
        std::vector<std::string> hand = drawHand(5);
        displayHand(hand);
        std::cout << "  [Press Enter to score hand]";
        std::cin.ignore();
        std::cin.get();

        // --- Score phase ---
        std::cout << "\n  --- Score Breakdown ---\n";
        scorer->printBreakdown(hand);
        int roundScore = scorer->calculate(hand);
        totalScore += roundScore;

        std::cout << "\n  Round score   : " << roundScore << "\n";
        std::cout << "  Total score   : " << totalScore << "\n";

        // --- Shop phase (skip after last round) ---
        if (roundNumber < maxRounds) {
            runShop();
        }

        std::cout << "\n";
    }

    // --- End Run ---
    std::cout << "╔══════════════════════════════════════╗\n";
    std::cout << "║              RUN COMPLETE            ║\n";
    std::cout << "╠══════════════════════════════════════╣\n";
    std::cout << "║  Final Score : " << std::setw(6) << totalScore
              << "                    ║\n";
    std::cout << "╚══════════════════════════════════════╝\n";

    // Print modifiers collected
    const auto& mods = scorer->getModifiers();
    if (!mods.empty()) {
        std::cout << "\n  Modifiers collected:\n";
        for (IModifier* m : mods) {
            std::cout << "    • " << m->getName() << "\n";
        }
    }
    std::cout << "\n";
}
