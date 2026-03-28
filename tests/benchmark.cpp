#include <iostream>
#include <vector>
#include <chrono>
#include "Item.hpp"
#include "PRNG.hpp"
#include "HashTable.hpp"
#include "MaxHeap.hpp"
#include "DropSystem.hpp"

void runBenchmark(int numItems) {
    std::cout << "\n--- Benchmark: " << numItems << " itens ---" << std::endl;
    
    XORShift xorshift(42);
    DropSystem ds(&xorshift, 0.1);
    
    HashTable htDiv(numItems/2, DIVISION);
    HashTable htMult(numItems/2, MULTIPLICATION);
    MaxHeap heap;

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < numItems; ++i) {
        Item item = ds.generateDrop(i + 1);
        htDiv.insert(item);
        htMult.insert(item);
        heap.insert(item);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    
    std::cout << "Tempo total: " << duration.count() << " ms" << std::endl;
    std::cout << "Colisoes (Divisao): " << htDiv.getCollisions() << std::endl;
    std::cout << "Colisoes (Multiplicacao): " << htMult.getCollisions() << std::endl;
    std::cout << "Itens Raros: " << ds.getRareItemsGenerated() << " (Proporcao: " << ds.getRareRatio() << ")" << std::endl;
    
    // Benchmark Search
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; ++i) {
        htDiv.search(xorshift.next() % numItems);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "Tempo 1000 buscas (Divisao): " << duration.count() << " ms" << std::endl;

    // Benchmark Heap extraction
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000 && !heap.empty(); ++i) {
        heap.extractMax();
    }
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "Tempo 1000 extrações Max-Heap: " << duration.count() << " ms" << std::endl;
}

int main() {
    runBenchmark(1000);
    runBenchmark(10000);
    runBenchmark(100000);
    return 0;
}
