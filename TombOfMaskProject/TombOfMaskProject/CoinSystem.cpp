#include "CoinSystem.h"

CoinSystem::CoinSystem() : coins(0) {}

void CoinSystem::addCoins(int amount) {
    if (amount > 0) coins += amount;
}

int CoinSystem::getCoins() const {
    return coins;
}

void CoinSystem::reset() {
    coins = 0;
}