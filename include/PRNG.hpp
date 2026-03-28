#ifndef PRNG_HPP
#define PRNG_HPP

#include <cstdint>

class PRNG {
public:
    virtual ~PRNG() {}
    virtual uint32_t next() = 0;
    virtual void setSeed(uint32_t seed) = 0;
    
    // Returns a double between 0 and 1
    double nextDouble() {
        return static_cast<double>(next()) / static_cast<double>(UINT32_MAX);
    }
};

class LCG : public PRNG {
private:
    uint32_t state;
    const uint32_t a = 1664525;
    const uint32_t c = 1013904223;
public:
    LCG(uint32_t seed = 42) : state(seed) {}
    void setSeed(uint32_t seed) override { state = seed; }
    uint32_t next() override {
        state = a * state + c;
        return state;
    }
};

class XORShift : public PRNG {
private:
    uint32_t state;
public:
    XORShift(uint32_t seed = 42) : state(seed == 0 ? 42 : seed) {}
    void setSeed(uint32_t seed) override { state = seed == 0 ? 42 : seed; }
    uint32_t next() override {
        state ^= state << 13;
        state ^= state >> 17;
        state ^= state << 5;
        return state;
    }
};

#endif
