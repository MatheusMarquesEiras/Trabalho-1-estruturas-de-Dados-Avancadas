#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include "Item.hpp"
#include <vector>
#include <list>
#include <cmath>

enum HashMethod {
    DIVISION,
    MULTIPLICATION
};

class HashTable {
private:
    int size;
    std::vector<std::list<Item>> table;
    HashMethod method;
    int collisions;

    int hashDivision(int key) {
        return key % size;
    }

    int hashMultiplication(int key) {
        const double A = (std::sqrt(5) - 1.0) / 2.0;
        double fractionalPart = (key * A) - std::floor(key * A);
        return static_cast<int>(size * fractionalPart);
    }

    int getIndex(int key) {
        if (method == DIVISION) {
            return hashDivision(key);
        } else {
            return hashMultiplication(key);
        }
    }

public:
    HashTable(int size = 101, HashMethod method = DIVISION)
        : size(size), table(size), method(method), collisions(0) {}

    void setMethod(HashMethod m) {
        method = m;
        // Optionally rehash here if the table is not empty, 
        // but for this assignment, we'll just switch the method for new insertions/lookups.
    }

    void insert(const Item& item) {
        int index = getIndex(item.id);
        if (!table[index].empty()) {
            collisions++;
        }
        table[index].push_back(item);
    }

    Item* search(int id) {
        int index = getIndex(id);
        for (auto& item : table[index]) {
            if (item.id == id) {
                return &item;
            }
        }
        return nullptr;
    }

    bool remove(int id) {
        int index = getIndex(id);
        auto& list = table[index];
        for (auto it = list.begin(); it != list.end(); ++it) {
            if (it->id == id) {
                list.erase(it);
                return true;
            }
        }
        return false;
    }

    int getCollisions() const {
        return collisions;
    }

    void resetCollisions() {
        collisions = 0;
    }
};

#endif
