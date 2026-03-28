#ifndef MAXHEAP_HPP
#define MAXHEAP_HPP

#include "Item.hpp"
#include <vector>
#include <stdexcept>

class MaxHeap {
private:
    std::vector<Item> heap;

    int parent(int i) { return (i - 1) / 2; }
    int leftChild(int i) { return (2 * i) + 1; }
    int rightChild(int i) { return (2 * i) + 2; }

    void siftUp(int i) {
        while (i > 0 && heap[i].raridade > heap[parent(i)].raridade) {
            std::swap(heap[i], heap[parent(i)]);
            i = parent(i);
        }
    }

    void siftDown(int i) {
        int maxIndex = i;
        int l = leftChild(i);
        if (l < heap.size() && heap[l].raridade > heap[maxIndex].raridade) {
            maxIndex = l;
        }
        int r = rightChild(i);
        if (r < heap.size() && heap[r].raridade > heap[maxIndex].raridade) {
            maxIndex = r;
        }
        if (i != maxIndex) {
            std::swap(heap[i], heap[maxIndex]);
            siftDown(maxIndex);
        }
    }

public:
    void insert(const Item& item) {
        heap.push_back(item);
        siftUp(heap.size() - 1);
    }

    Item extractMax() {
        if (heap.empty()) {
            throw std::runtime_error("Heap empty");
        }
        Item result = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty()) {
            siftDown(0);
        }
        return result;
    }

    Item peekMax() const {
        if (heap.empty()) {
            throw std::runtime_error("Heap empty");
        }
        return heap[0];
    }

    bool empty() const {
        return heap.empty();
    }

    size_t size() const {
        return heap.size();
    }
};

#endif
