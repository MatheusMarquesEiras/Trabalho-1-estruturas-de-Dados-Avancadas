#include <iostream>
#include <string>
#include <vector>
#include "Item.hpp"
#include "PRNG.hpp"
#include "HashTable.hpp"
#include "MaxHeap.hpp"
#include "DropSystem.hpp"

int main(int argc, char* argv[]) {
    uint32_t seed = static_cast<uint32_t>(argc);
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-s" || arg == "--seed") {
            if (i + 1 >= argc) {
                std::cerr << "Erro: a opcao -s requer um valor de seed." << std::endl;
                return 1;
            }
            try {
                seed = static_cast<uint32_t>(std::stoul(argv[++i]));
            } catch (const std::exception&) {
                std::cerr << "Erro: seed invalida: " << argv[i] << std::endl;
                return 1;
            }
        } else if (arg.rfind("-s", 0) == 0 && arg.size() > 2) {
            try {
                seed = static_cast<uint32_t>(std::stoul(arg.substr(2)));
            } catch (const std::exception&) {
                std::cerr << "Erro: seed invalida: " << arg.substr(2) << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Opcao desconhecida: " << arg << std::endl;
            std::cerr << "Uso: " << argv[0] << " [-s <seed>]" << std::endl;
            return 1;
        }
    }

    LCG lcg(seed);
    XORShift xorshift(seed);
    
    std::cout << "--- Sistema de Inventario e Drop Balanceado ---" << std::endl;
    std::cout << "Seed usada: " << seed << std::endl;
    
    // Demonstrate PRNG
    std::cout << "LCG (primeiros 5): ";
    for(int i=0; i<5; ++i) std::cout << lcg.next() << " ";
    std::cout << "\nXORShift (primeiros 5): ";
    for(int i=0; i<5; ++i) std::cout << xorshift.next() << " ";
    std::cout << "\n" << std::endl;

    // Set up structures
    HashTable inventory(101, DIVISION);
    MaxHeap heap;
    DropSystem ds(&lcg, 0.1); // Use LCG by default, 10% rare chance

    // Generate some drops
    std::cout << "Gerando 20 drops..." << std::endl;
    for (int i = 0; i < 20; ++i) {
        Item item = ds.generateDrop(i + 1);
        inventory.insert(item);
        heap.insert(item);
        item.print();
    }

    std::cout << "\nTotal de itens: " << ds.getItemsGenerated() << std::endl;
    std::cout << "Itens raros: " << ds.getRareItemsGenerated() << " (Proporcao: " << ds.getRareRatio() << ")" << std::endl;

    // Show top items
    std::cout << "\nItens mais valiosos (pelo Max-Heap):" << std::endl;
    for (int i = 0; i < 3 && !heap.empty(); ++i) {
        Item top = heap.extractMax();
        std::cout << i + 1 << ". ";
        top.print();
    }

    // Demonstrate lookup
    std::cout << "\nBuscando Item ID=10 no inventario..." << std::endl;
    Item* found = inventory.search(10);
    if (found) {
        found->print();
    } else {
        std::cout << "Item nao encontrado." << std::endl;
    }

    return 0;
}
