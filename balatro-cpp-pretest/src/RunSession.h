#pragma once
#include "ScoringSystem.h"
#include "ShopSystem.h"
#include <vector>
#include <string>

class RunSession {
private :
  int roundNumber;
  int totalScore;
  int maxRounds;

  IScoringStrategy* strategy;
  ScoringSystem* scorer;
  ShopSystem* shop;

  std::vector<IModifier*> ownedModifiers;

  std::vector<std::string> drawHand(int size = 5) const;

  void runShop();

  void displayHand(const std::vector<std::string>& hand) const;

public:
  RunSession(int rounds = 3);
  ~RunSession();

  void staart();
}
