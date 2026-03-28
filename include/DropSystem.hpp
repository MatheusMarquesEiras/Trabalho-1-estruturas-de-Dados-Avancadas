#ifndef DROPSYSTEM_HPP
#define DROPSYSTEM_HPP

#include "PRNG.hpp"
#include "Item.hpp"
#include <vector>
#include <string>

class DropSystem {
private:
    PRNG* prng;
    double baseRareChance; // Base chance for a "rare" roll (e.g., 0.1 for 10%)
    double currentRareChance;
    int itemsGenerated;
    int rareItemsGenerated;
    
    // Target ratio of rare items (e.g., 0.1 means we want 10% rare items)
    const double targetRatio = 0.1;
    const int rareThreshold = 80;

public:
    DropSystem(PRNG* prng, double baseRareChance = 0.1)
        : prng(prng), baseRareChance(baseRareChance), currentRareChance(baseRareChance),
          itemsGenerated(0), rareItemsGenerated(0) {}

    void setPRNG(PRNG* newPrng) {
        prng = newPrng;
    }

    Item generateDrop(int id) {
        itemsGenerated++;
        double roll = prng->nextDouble();
        
        int rarity;
        if (roll < currentRareChance) {
            // Rare item: rarity between 81 and 100
            rarity = 81 + (prng->next() % 20);
            rareItemsGenerated++;
        } else {
            // Common item: rarity between 1 and 80
            rarity = 1 + (prng->next() % 80);
        }

        // Adjust balanced logic
        double actualRatio = static_cast<double>(rareItemsGenerated) / itemsGenerated;
        if (actualRatio > targetRatio) {
            // Too many rare items, decrease chance
            currentRareChance *= 0.95;
        } else if (actualRatio < targetRatio) {
            // Too few rare items, increase chance
            currentRareChance *= 1.05;
            if (currentRareChance > 0.5) currentRareChance = 0.5; // Cap at 50%
        }

        std::string name = (rarity > rareThreshold) ? "Item Raro" : "Item Comum";
        return Item(id, name + " " + std::to_string(id), rarity);
    }

    double getRareRatio() const {
        if (itemsGenerated == 0) return 0;
        return static_cast<double>(rareItemsGenerated) / itemsGenerated;
    }

    int getItemsGenerated() const { return itemsGenerated; }
    int getRareItemsGenerated() const { return rareItemsGenerated; }
};

#endif
