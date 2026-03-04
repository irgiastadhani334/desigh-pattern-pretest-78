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

  
}
