# Sistema de Inventário e Drop Balanceado (Normas IFPR)

# Integrantes: Matheus Marques Eiras e Ciro

Este projeto implementa o núcleo de um sistema de inventário para jogos, focado em estruturas de dados eficientes e geração de números pseudo-aleatórios (PRNG) com controle de economia (Drop Balanceado). Desenvolvido como parte dos requisitos de Estruturas de Dados Avançadas do IFPR.

## 🚀 Funcionalidades Implementadas

### 1. Estruturas de Dados Manuais
*   **Tabela Hash:** Implementação com encadeamento separado. Suporta troca dinâmica entre:
    *   **Método da Divisão:** Rápido e direto ($h(k) = k \mod m$).
    *   **Método da Multiplicação:** Robusto para diferentes tamanhos de tabela ($h(k) = \lfloor m(kA \mod 1) \rfloor$).
*   **Max-Heap:** Organiza os itens por raridade, garantindo que o item mais valioso esteja sempre no topo ($O(1)$ para consulta).

### 2. Geração de Drops e Aleatoriedade
*   **PRNGs Configuráveis:** Implementação de dois algoritmos com seeds ajustáveis:
    *   **LCG (Linear Congruential Generator):** Algoritmo clássico e performático.
    *   **XORShift:** Algoritmo moderno baseado em operações de bits, altamente eficiente.
*   **Mecanismo de Drop Balanceado:** Lógica que monitora a taxa de itens raros gerados e ajusta a probabilidade em tempo real para evitar inflação ou escassez no mundo do jogo.

### 3. Análise de Desempenho
*   **Benchmark Suite:** Script que testa o sistema com volumes crescentes (1k a 100k itens).
*   **Métricas:** Medição de tempo de execução, contagem de colisões em hash e verificação da estabilidade da taxa de drop.

---

## 🛠️ Como Rodar o Projeto

### Pré-requisitos
*   Compilador C++ (g++ compatível com C++11 ou superior).
*   `make` (opcional, para facilidade de compilação).

### Compilação
No terminal, na raiz do projeto, execute:
```bash
make all
```

### Execução

1.  **Demonstração Principal:**
    Roda o fluxo básico: gera itens, insere no inventário, organiza no heap e busca um item específico com seed = 1.
    ```bash
    ./main
    ```
    Para seed's personalizadas com um ou mais caracteres inteiros, podemos ter por exemplo a seed 123.
    ```bash
    ./main -s 123
    ```

2.  **Testes de Benchmark:**
    Roda a análise de performance com grandes entradas de dados e exibe estatísticas de colisões e tempo.
    ```bash
    ./benchmark
    ```

### Limpeza
Para remover os arquivos binários gerados:
```bash
make clean
```

---

## 📂 Descrição dos Arquivos

### Diretórios

*   **`/include`** - Cabeçalhos contendo implementações das estruturas de dados
    *   `HashTable.hpp`: Implementação de tabela hash com método da divisão e multiplicação
    *   `MaxHeap.hpp`: Implementação de heap máximo para organização por raridade
    *   `Item.hpp`: Estrutura de dados que representa um item do inventário
    *   `PRNG.hpp`: Geradores de números pseudo-aleatórios (LCG e XORShift)
    *   `DropSystem.hpp`: Sistema de drop balanceado com ajuste automático de probabilidades

*   **`/src`** - Código-fonte principal
    *   `main.cpp`: Programa principal que demonstra o funcionamento do sistema com suporte a seeds personalizadas

*   **`/tests`** - Testes e benchmarks
    *   `benchmark.cpp`: Suite de testes de performance com volumes crescentes (1k a 100k itens)

### Arquivos de Configuração

*   **`Makefile`** - Arquivo de compilação com targets: `all`, `clean`, `main`, `benchmark`
*   **`README.md`** - Este arquivo com documentação do projeto

### Arquivo de Relatório

*   **`RELATORIO.md`** - Documentação técnica detalhada, análise crítica das estruturas e cálculos de complexidade
