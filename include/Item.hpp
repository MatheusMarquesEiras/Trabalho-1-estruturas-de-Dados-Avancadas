#ifndef ITEM_HPP
#define ITEM_HPP

#include <string>
#include <iostream>

struct Item {
    int id;
    std::string nome;
    int raridade; // 1 to 100, where 100 is rarest

    Item(int id = 0, std::string nome = "", int raridade = 0)
        : id(id), nome(nome), raridade(raridade) {}

    void print() const {
        std::cout << "Item[ID=" << id << ", Nome=" << nome << ", Raridade=" << raridade << "]" << std::endl;
    }

    bool operator>(const Item& other) const {
        return raridade > other.raridade;
    }

    bool operator<(const Item& other) const {
        return raridade < other.raridade;
    }
};

#endif
