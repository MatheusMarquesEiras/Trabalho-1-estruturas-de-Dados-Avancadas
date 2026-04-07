# Sistema de Inventário e Drop Balanceado (Normas IFPR)

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

## 📂 Estrutura de Arquivos
*   `/include`: Cabeçalhos (`.hpp`) com as implementações das estruturas.
*   `/src`: Código fonte do programa principal.
*   `/tests`: Scripts de teste e medição de performance.
*   `RELATORIO.md`: Documentação técnica detalhada e análise crítica (base para o relatório final).
