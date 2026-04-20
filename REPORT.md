# Relatório Técnico — Sistema de Inventário e Drop Balanceado

**Disciplina:** Estruturas de Dados Avançadas — IFPR  
**Autor:** Matheus Marques Eiras  
**Data:** 2026-04-20

---

## Sumário

1. [Visão Geral do Projeto](#1-visão-geral-do-projeto)
2. [Estrutura de Arquivos](#2-estrutura-de-arquivos)
3. [Item — Entidade Central](#3-item--entidade-central)
4. [PRNG — Geradores de Números Pseudo-Aleatórios](#4-prng--geradores-de-números-pseudo-aleatórios)
5. [HashTable — Tabela Hash com Encadeamento Separado](#5-hashtable--tabela-hash-com-encadeamento-separado)
6. [MaxHeap — Fila de Prioridade Máxima](#6-maxheap--fila-de-prioridade-máxima)
7. [DropSystem — Sistema de Drop Balanceado](#7-dropsystem--sistema-de-drop-balanceado)
8. [main.cpp — Programa Principal](#8-maincpp--programa-principal)
9. [benchmark.cpp — Análise de Desempenho](#9-benchmarkcpp--análise-de-desempenho)
10. [Makefile — Compilação](#10-makefile--compilação)
11. [Complexidade Algorítmica](#11-complexidade-algorítmica)
12. [Conclusão](#12-conclusão)

---

## 1. Visão Geral do Projeto

O projeto implementa o núcleo de um **sistema de inventário para jogos**, simulando o mecanismo de "drop" (geração aleatória de itens). O problema central é: como gerar itens aleatórios de forma eficiente, armazená-los rapidamente e garantir que a proporção de itens raros se mantenha equilibrada ao longo do tempo?

Para resolver isso, o projeto combina três estruturas de dados clássicas:

| Estrutura | Finalidade |
|---|---|
| **Tabela Hash** | Armazenamento e busca rápida de itens por ID |
| **Max-Heap** | Recuperação eficiente dos itens mais valiosos |
| **PRNG** | Geração de números aleatórios para os drops |

Todas as estruturas são implementadas manualmente (sem uso de `std::unordered_map`, `std::priority_queue` etc.), o que é o objetivo didático da disciplina.

---

## 2. Estrutura de Arquivos

```
Trabalho-1-estruturas-de-Dados-Avancadas/
├── include/
│   ├── Item.hpp          # Estrutura de dados do item
│   ├── PRNG.hpp          # Geradores de números aleatórios (LCG e XORShift)
│   ├── HashTable.hpp     # Tabela hash com encadeamento separado
│   ├── MaxHeap.hpp       # Heap máximo baseado em raridade
│   └── DropSystem.hpp    # Sistema de geração e balanceamento de drops
├── src/
│   └── main.cpp          # Demonstração do sistema completo
├── tests/
│   └── benchmark.cpp     # Testes de desempenho com grandes volumes
├── Makefile              # Compilação automatizada
└── README.md             # Documentação de uso
```

Todo o código das estruturas está nos arquivos `.hpp` dentro de `include/`, seguindo o padrão de bibliotecas de cabeçalho C++. Isso permite incluir qualquer estrutura em qualquer arquivo fonte com uma única linha `#include`.

---

## 3. Item — Entidade Central

**Arquivo:** `include/Item.hpp`

### O que é

`Item` é a estrutura de dados fundamental do projeto. Representa um item de jogo com três atributos:

```cpp
struct Item {
    int id;          // Identificador único
    std::string nome; // Nome descritivo
    int raridade;    // Valor de 1 a 100 (100 = mais raro)
};
```

### Por que `struct` e não `class`

Em C++, `struct` é equivalente a `class` com tudo público por padrão. Aqui é uma escolha semântica: `Item` é um **agregado de dados simples** (um "value object"), não um objeto com estado interno complexo.

### Operadores sobrecarregados

```cpp
bool operator>(const Item& other) const {
    return raridade > other.raridade;
}
bool operator<(const Item& other) const {
    return raridade < other.raridade;
}
```

Os operadores `>` e `<` foram sobrecarregados para comparar itens pela **raridade**. Isso permite que o MaxHeap determine qual item tem maior prioridade de forma direta, sem precisar saber detalhes internos do `Item`.

### Método `print()`

Exibe os dados do item no console. Útil para depuração e demonstração.

---

## 4. PRNG — Geradores de Números Pseudo-Aleatórios

**Arquivo:** `include/PRNG.hpp`

### O problema

O sistema de drops precisa de aleatoriedade. A função `rand()` da biblioteca padrão C não é ideal para simulações: tem qualidade estatística baixa e não é reproduzível facilmente. Por isso, dois algoritmos clássicos foram implementados do zero.

### Classe base abstrata `PRNG`

```cpp
class PRNG {
public:
    virtual uint32_t next() = 0;
    virtual void setSeed(uint32_t seed) = 0;

    double nextDouble() {
        return static_cast<double>(next()) / static_cast<double>(UINT32_MAX);
    }
};
```

`PRNG` é uma **classe abstrata** (interface) com dois métodos puramente virtuais: `next()` (gera o próximo número) e `setSeed()` (reinicia o gerador). O método concreto `nextDouble()` converte qualquer `uint32_t` em um `double` no intervalo [0, 1] dividindo pelo valor máximo possível (`UINT32_MAX = 4.294.967.295`). Isso permite que o `DropSystem` faça comparações de probabilidade como `if (roll < 0.1)`.

O uso de polimorfismo aqui é essencial: o `DropSystem` recebe um ponteiro `PRNG*` e não precisa saber se está usando LCG ou XORShift, podendo ser trocado em tempo de execução.

---

### LCG — Linear Congruential Generator

```cpp
class LCG : public PRNG {
private:
    uint32_t state;
    const uint32_t a = 1664525;
    const uint32_t c = 1013904223;
public:
    uint32_t next() override {
        state = a * state + c;
        return state;
    }
};
```

#### Como funciona

A fórmula é: **Xₙ₊₁ = (a × Xₙ + c) mod 2³²**

O módulo 2³² é aplicado automaticamente pelo estouro natural de `uint32_t` (overflow intencional). Os valores das constantes `a = 1664525` e `c = 1013904223` são os mesmos utilizados pelo compilador Borland C++ e foram escolhidos por produzirem boa distribuição estatística (estudados por Knuth em *The Art of Computer Programming*).

#### Características

- **Vantagem:** Extremamente rápido (apenas uma multiplicação e uma adição por chamada).
- **Desvantagem:** Qualidade estatística limitada; sequências longas podem apresentar padrões visíveis.
- **Período:** 2³² (4 bilhões de valores antes de repetir).

---

### XORShift

```cpp
class XORShift : public PRNG {
private:
    uint32_t state;
public:
    uint32_t next() override {
        state ^= state << 13;
        state ^= state >> 17;
        state ^= state << 5;
        return state;
    }
};
```

#### Como funciona

Opera exclusivamente com operações de bits (XOR e shift). Cada linha modifica o estado aplicando um deslocamento e um XOR, criando uma avalanche de bits que é difícil de prever.

- `state ^= state << 13`: desloca os bits 13 posições à esquerda e aplica XOR consigo mesmo.
- `state ^= state >> 17`: desloca 17 posições à direita e aplica XOR.
- `state ^= state << 5`: desloca 5 posições à esquerda e aplica XOR.

#### Proteção contra seed zero

```cpp
XORShift(uint32_t seed = 42) : state(seed == 0 ? 42 : seed) {}
```

Se o estado for 0, `0 XOR 0 = 0` — o gerador ficaria preso em zero para sempre. Por isso, seed 0 é substituído por 42.

#### Características

- **Vantagem:** Mais alta qualidade estatística que LCG, ainda extremamente rápido.
- **Período:** 2³² − 1 (todos os valores exceto zero).

---

## 5. HashTable — Tabela Hash com Encadeamento Separado

**Arquivo:** `include/HashTable.hpp`

### O problema

Para buscar um item por ID em um array simples, seria necessário percorrer todos os elementos: O(n). Uma tabela hash resolve isso calculando matematicamente a posição do item, atingindo O(1) na média.

### Estrutura interna

```cpp
std::vector<std::list<Item>> table;
```

A tabela é um **vetor de listas encadeadas**. Cada posição do vetor é um "balde" (bucket). Quando dois itens mapeiam para o mesmo índice (colisão), eles são adicionados à mesma lista — técnica chamada de **encadeamento separado (chaining)**.

### Dois métodos de hash

O enum `HashMethod` permite escolher o algoritmo de hash:

```cpp
enum HashMethod { DIVISION, MULTIPLICATION };
```

#### Método da Divisão

```cpp
int hashDivision(int key) {
    return key % size;
}
```

Fórmula: **h(k) = k mod m**

Simples e rápido. O ideal é que `m` (o tamanho da tabela) seja um número primo para minimizar colisões quando as chaves têm padrões regulares. No `main.cpp`, o tamanho 101 foi escolhido por ser primo.

#### Método da Multiplicação

```cpp
int hashMultiplication(int key) {
    const double A = (std::sqrt(5) - 1.0) / 2.0; // ≈ 0.6180339887 (proporção áurea)
    double fractionalPart = (key * A) - std::floor(key * A);
    return static_cast<int>(size * fractionalPart);
}
```

Fórmula: **h(k) = ⌊m × (kA mod 1)⌋**

Onde `A = (√5 − 1) / 2 ≈ 0,618` é a parte fracionária da proporção áurea. Este valor foi sugerido por Knuth e produz distribuição uniforme independentemente do tamanho da tabela. A expressão `(key * A) - floor(key * A)` extrai apenas a parte fracionária do produto.

### Operações

#### Inserção — `insert()`

```cpp
void insert(const Item& item) {
    int index = getIndex(item.id);
    if (!table[index].empty()) collisions++;
    table[index].push_back(item);
}
```

Calcula o índice e adiciona o item ao final da lista naquela posição. Se a lista não estava vazia, registra uma colisão.

#### Busca — `search()`

```cpp
Item* search(int id) {
    int index = getIndex(id);
    for (auto& item : table[index]) {
        if (item.id == id) return &item;
    }
    return nullptr;
}
```

Calcula o índice e percorre apenas a lista naquela posição. Na ausência de colisões, encontra o item em O(1). Retorna um ponteiro para o item ou `nullptr` se não encontrado.

#### Remoção — `remove()`

```cpp
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
```

Usa um iterador para percorrer a lista e remove o elemento quando encontrado. O uso de `erase(iterator)` é necessário porque `std::list` não suporta acesso por índice.

### Monitoramento de Colisões

```cpp
int getCollisions() const { return collisions; }
void resetCollisions() { collisions = 0; }
```

Permite medir a qualidade do método de hash durante os benchmarks.

---

## 6. MaxHeap — Fila de Prioridade Máxima

**Arquivo:** `include/MaxHeap.hpp`

### O problema

Após gerar centenas de itens, como saber rapidamente quais são os mais raros/valiosos? Uma lista ordenada seria O(n log n) para manter. O Max-Heap resolve isso mantendo sempre o item mais valioso acessível em O(1) e permitindo inserção/remoção em O(log n).

### Representação em array

O heap é armazenado em um `std::vector<Item>`, onde a posição de cada elemento define sua relação pai-filho:

```
                heap[0]           <- raiz (item mais raro)
              /         \
          heap[1]       heap[2]
          /    \        /    \
       heap[3] heap[4] heap[5] heap[6]
```

As relações são calculadas aritmeticamente:

```cpp
int parent(int i)     { return (i - 1) / 2; }
int leftChild(int i)  { return (2 * i) + 1; }
int rightChild(int i) { return (2 * i) + 2; }
```

### Propriedade do Max-Heap

Todo nó pai tem raridade **maior ou igual** à raridade de seus filhos. Portanto, `heap[0]` é sempre o item mais raro.

### siftUp — Subir elemento

```cpp
void siftUp(int i) {
    while (i > 0 && heap[i].raridade > heap[parent(i)].raridade) {
        std::swap(heap[i], heap[parent(i)]);
        i = parent(i);
    }
}
```

Chamado após a **inserção**. O novo elemento é colocado no final do array e vai "subindo" trocando de posição com o pai enquanto for maior que ele. Complexidade: **O(log n)**.

### siftDown — Descer elemento

```cpp
void siftDown(int i) {
    int maxIndex = i;
    int l = leftChild(i);
    if (l < heap.size() && heap[l].raridade > heap[maxIndex].raridade)
        maxIndex = l;
    int r = rightChild(i);
    if (r < heap.size() && heap[r].raridade > heap[maxIndex].raridade)
        maxIndex = r;
    if (i != maxIndex) {
        std::swap(heap[i], heap[maxIndex]);
        siftDown(maxIndex);
    }
}
```

Chamado após a **extração da raiz**. Encontra qual dos filhos é maior e troca com ele, recursivamente, até que o elemento esteja numa posição onde é maior que ambos os filhos. Complexidade: **O(log n)**.

### Operações

#### `insert(item)`

```cpp
void insert(const Item& item) {
    heap.push_back(item);
    siftUp(heap.size() - 1);
}
```

Adiciona ao final e restaura a propriedade do heap subindo o elemento.

#### `extractMax()`

```cpp
Item extractMax() {
    Item result = heap[0];       // Salva a raiz
    heap[0] = heap.back();      // Move o último para a raiz
    heap.pop_back();             // Remove o último
    if (!heap.empty()) siftDown(0); // Restaura a propriedade
    return result;
}
```

Salva a raiz (item mais raro), move o último elemento para a posição 0 e desce ele até a posição correta. Complexidade: **O(log n)**.

#### `peekMax()`

Retorna o item mais raro sem removê-lo. Complexidade: **O(1)** — é sempre `heap[0]`.

---

## 7. DropSystem — Sistema de Drop Balanceado

**Arquivo:** `include/DropSystem.hpp`

### O problema

Em jogos, se itens raros aparecem com muita frequência, perdem o valor (inflação). Se aparecem raramente demais, os jogadores ficam frustrados. O `DropSystem` resolve isso com um **mecanismo de feedback dinâmico** que ajusta a probabilidade em tempo real.

### Atributos

```cpp
PRNG* prng;               // Gerador de números aleatórios (injetado externamente)
double baseRareChance;    // Probabilidade base (ex: 0.1 = 10%)
double currentRareChance; // Probabilidade atual (ajustada dinamicamente)
int itemsGenerated;       // Total de itens gerados
int rareItemsGenerated;   // Total de itens raros gerados
const double targetRatio = 0.1;   // Meta: 10% de itens raros
const int rareThreshold = 80;     // Raridade > 80 = item raro
```

O uso de `PRNG*` (ponteiro para a interface) é o padrão de projeto **Strategy**: o algoritmo de geração pode ser trocado sem modificar o `DropSystem`.

### Geração de um drop — `generateDrop(int id)`

```cpp
Item generateDrop(int id) {
    itemsGenerated++;
    double roll = prng->nextDouble(); // Número entre 0 e 1

    int rarity;
    if (roll < currentRareChance) {
        rarity = 81 + (prng->next() % 20); // Item raro: raridade 81-100
        rareItemsGenerated++;
    } else {
        rarity = 1 + (prng->next() % 80);  // Item comum: raridade 1-80
    }
    ...
}
```

O processo é:
1. Gera um número `roll` entre 0 e 1.
2. Se `roll < currentRareChance`, gera um item raro (raridade 81–100).
3. Caso contrário, gera um item comum (raridade 1–80).

### Mecanismo de balanceamento dinâmico

Após cada geração, o sistema verifica se a proporção real está acima ou abaixo da meta:

```cpp
double actualRatio = static_cast<double>(rareItemsGenerated) / itemsGenerated;

if (actualRatio > targetRatio) {
    currentRareChance *= 0.95; // Muito raro: diminui 5% a chance
} else if (actualRatio < targetRatio) {
    currentRareChance *= 1.05; // Pouco raro: aumenta 5% a chance
    if (currentRareChance > 0.5) currentRareChance = 0.5; // Teto em 50%
}
```

Este é um **controlador proporcional simples**: se há itens raros demais, a chance cai 5%; se há de menos, sobe 5%. O limite de 50% previne que o sistema degrade completamente a raridade em cenários extremos.

### Exemplo de execução

| Item # | Roll | Chance atual | Resultado | Proporção real | Ajuste |
|--------|------|-------------|-----------|---------------|--------|
| 1 | 0.05 | 10% | Raro | 100% | Diminui para 9.5% |
| 2 | 0.08 | 9.5% | Raro | 100% | Diminui para 9.025% |
| 3 | 0.40 | 9.025% | Comum | 66.7% | Diminui para 8.57% |
| ... | ... | ... | ... | ... | ... |

Ao longo de muitos itens, a proporção converge para aproximadamente 10%.

---

## 8. main.cpp — Programa Principal

**Arquivo:** `src/main.cpp`

O `main.cpp` é uma **demonstração** do sistema completo, mostrando todas as estruturas em funcionamento.

### Fluxo de execução

```
1. Inicializa LCG e XORShift com seed 123
2. Imprime os primeiros 5 números de cada PRNG
3. Cria HashTable (tamanho 101, método Divisão), MaxHeap e DropSystem
4. Gera 20 drops e insere em HashTable e MaxHeap simultaneamente
5. Exibe estatísticas de drops raros
6. Extrai e exibe os 3 itens mais valiosos do MaxHeap
7. Busca o Item ID=10 na HashTable
```

### Ponto de atenção: uso simultâneo de HashTable e MaxHeap

Cada item gerado é inserido nas **duas** estruturas:

```cpp
Item item = ds.generateDrop(i + 1);
inventory.insert(item);   // Para busca O(1) por ID
heap.insert(item);        // Para extração do mais valioso em O(log n)
```

Isso é uma decisão de projeto válida: as duas estruturas servem a propósitos diferentes e se complementam. A HashTable responde "dado um ID, qual é o item?"; o MaxHeap responde "qual é o item mais valioso agora?".

---

## 9. benchmark.cpp — Análise de Desempenho

**Arquivo:** `tests/benchmark.cpp`

### Objetivo

Medir o desempenho das estruturas com volumes crescentes de dados: 1.000, 10.000 e 100.000 itens.

### O que é medido

Para cada volume, a função `runBenchmark(int numItems)` mede:

| Métrica | O que mede |
|---|---|
| Tempo total de inserção | Custo de popular HashTable (×2) e MaxHeap |
| Colisões (Divisão) | Qualidade da distribuição pelo método da divisão |
| Colisões (Multiplicação) | Qualidade da distribuição pelo método da multiplicação |
| Proporção de itens raros | Verificação do balanceamento do DropSystem |
| Tempo de 1.000 buscas | Custo médio de `search()` na HashTable |
| Tempo de 1.000 extrações | Custo de `extractMax()` no MaxHeap |

### Medição de tempo

```cpp
auto start = std::chrono::high_resolution_clock::now();
// ... operações ...
auto end = std::chrono::high_resolution_clock::now();
std::chrono::duration<double, std::milli> duration = end - start;
std::cout << duration.count() << " ms" << std::endl;
```

`std::chrono::high_resolution_clock` é o relógio de maior precisão disponível na plataforma, adequado para medir operações rápidas em milissegundos.

### Tamanho das tabelas hash no benchmark

```cpp
HashTable htDiv(numItems/2, DIVISION);
HashTable htMult(numItems/2, MULTIPLICATION);
```

O tamanho `numItems/2` significa que a tabela terá metade do número de elementos — **fator de carga de 2**. Isso garante que haverá colisões, tornando o benchmark mais realista e permitindo comparar os dois métodos em condições de estresse.

---

## 10. Makefile — Compilação

**Arquivo:** `Makefile`

```makefile
CXX = g++
CXXFLAGS = -std=c++11 -Iinclude -O3

all: main benchmark

main: src/main.cpp include/*.hpp
    $(CXX) $(CXXFLAGS) src/main.cpp -o main

benchmark: tests/benchmark.cpp include/*.hpp
    $(CXX) $(CXXFLAGS) tests/benchmark.cpp -o benchmark

clean:
    rm -f main benchmark
```

### Flags importantes

| Flag | Significado |
|---|---|
| `-std=c++11` | Habilita C++11 (necessário para `auto`, lambdas, `chrono`, etc.) |
| `-Iinclude` | Adiciona o diretório `include/` ao caminho de busca de headers |
| `-O3` | Nível máximo de otimização do compilador (essencial para benchmarks precisos) |

### Dependências declaradas

`include/*.hpp` como dependência garante que qualquer alteração em qualquer header recompila o executável, evitando binários desatualizados.

---

## 11. Complexidade Algorítmica

### Tabela Hash

| Operação | Caso Médio | Pior Caso |
|---|---|---|
| `insert()` | O(1) | O(n) |
| `search()` | O(1) | O(n) |
| `remove()` | O(1) | O(n) |

O pior caso O(n) ocorre quando todos os elementos colidem no mesmo bucket. Com um bom método de hash e fator de carga razoável, isso é extremamente improvável na prática.

### Max-Heap

| Operação | Complexidade |
|---|---|
| `insert()` | O(log n) |
| `extractMax()` | O(log n) |
| `peekMax()` | O(1) |

### DropSystem

| Operação | Complexidade |
|---|---|
| `generateDrop()` | O(1) |

### PRNG

| Algoritmo | Complexidade por chamada |
|---|---|
| LCG | O(1) |
| XORShift | O(1) |

---

## 12. Conclusão

O projeto demonstra com clareza como estruturas de dados clássicas se combinam para resolver um problema real. A **Tabela Hash** oferece acesso O(1) para consultas por ID, indispensável em inventários grandes. O **Max-Heap** garante recuperação imediata do item mais valioso sem manter o array totalmente ordenado. O **DropSystem** com controle dinâmico mostra como uma lógica simples de feedback proporcional pode manter a estabilidade de um sistema estocástico.

A separação em interfaces (`PRNG` abstrata) e a injeção de dependência (passar `PRNG*` para o `DropSystem`) ilustram boas práticas de engenharia de software que tornam o código extensível: é trivial adicionar um terceiro algoritmo PRNG (como Mersenne Twister) sem alterar nenhuma outra classe.

O benchmark com 100.000 itens valida que as estruturas se comportam dentro das complexidades teóricas esperadas mesmo sob carga real.
