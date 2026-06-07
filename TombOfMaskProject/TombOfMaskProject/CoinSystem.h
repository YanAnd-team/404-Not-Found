#pragma once

class CoinSystem {
private:
    int coins;

public:
    CoinSystem();
    void addCoins(int amount);
    int getCoins() const;
    void reset();
};
