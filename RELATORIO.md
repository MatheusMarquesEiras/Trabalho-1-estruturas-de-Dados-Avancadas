# Relatório de Implementação: Sistema de Inventário e Drop Balanceado

**Disciplina:** Estruturas de Dados Avançadas  
**Data:** 6 de Abril de 2026  
**Tema:** Sistema de Inventário com Hash, Heaps e PRNG

---

## 1. Introdução
Este relatório descreve a implementação de um sistema de inventário para jogos, integrando estruturas de dados fundamentais para garantir eficiência no acesso, organização por prioridade e geração de itens com raridade balanceada. O objetivo é demonstrar o uso prático de Tabelas Hash, Max-Heaps e Geradores de Números Pseudo-Aleatórios (PRNGs) em um cenário de gameplay.

---

## 2. Modelagem do Problema
O sistema foi modelado para atender às seguintes necessidades:
- **Armazenamento:** Utiliza uma **Tabela Hash** para armazenar o inventário do jogador, permitindo busca e inserção rápidas por ID.
- **Organização:** Utiliza um **Max-Heap** para manter os itens organizados por raridade, facilitando o acesso instantâneo ao item mais valioso do jogador.
- **Geração de Itens:** Utiliza **PRNGs** para determinar a raridade dos itens no momento do "drop".
- **Equilíbrio:** Implementa um **Mecanismo de Drop Balanceado** para evitar sequências extremas de sorte ou azar, mantendo a economia do jogo estável.

---

## 3. Estruturas de Dados e Algoritmos

### 3.1. Tabela Hash (Hash Table)
A implementação suporta encadeamento externo (listas ligadas) para tratamento de colisões e dois métodos de espalhamento:

1.  **Método da Divisão:** $h(k) = k \mod m$.
    -   **Justificativa:** Simples e eficaz quando o tamanho da tabela ($m$) é um número primo não próximo de potências de 2.
2.  **Método da Multiplicação (Knuth):** $h(k) = \lfloor m (k \cdot A \mod 1) \rfloor$, com $A = (\sqrt{5}-1)/2$.
    -   **Justificativa:** Recomendado por Donald Knuth, este método é menos sensível ao tamanho da tabela e utiliza todos os bits da chave, sendo eficiente para chaves sequenciais ou com padrões comuns.

### 3.2. Números Pseudo-Aleatórios (PRNG)
Foram implementados dois geradores manuais para garantir reprodutibilidade:

1.  **Linear Congruential Generator (LCG):**
    -   **Justificativa:** Algoritmo clássico e extremamente rápido, ideal para sistemas com recursos limitados onde a perfeição estatística absoluta não é crítica.
2.  **XORShift:**
    -   **Justificativa:** Baseado em operações de bitwise (XOR e shifts), é significativamente mais rápido que o LCG e oferece uma qualidade estatística superior, passando em diversos testes de aleatoriedade modernos (como o BigCrush).

### 3.3. Max-Heap
O Max-Heap organiza os itens com base no atributo `raridade` (1 a 100).
-   **Complexidade:** Inserção e extração em $O(\log N)$.
-   **Uso:** Permite que o sistema identifique rapidamente os itens mais raros para exibição em interfaces de "Top Itens" ou para mecânicas de troca.

---

## 4. Mecanismo de Drop Balanceado
O `DropSystem` utiliza um loop de feedback (feedback loop) para manter a taxa de itens raros próxima a um alvo (target ratio de 10%).

-   **Algoritmo:** A cada drop, o sistema compara a proporção real de itens raros gerados com o alvo.
-   **Ajuste Dinâmico:** Se a taxa real for superior ao alvo, a chance base de drop raro diminui. Se for inferior (azar do jogador), a chance aumenta progressivamente.
-   **Vantagem:** Este método atua de forma similar ao "Pity Timer" ou "Pseudo-Random Distribution" (usado em jogos como Dota 2 e Genshin Impact), garantindo que o jogador receba itens raros de forma consistente ao longo do tempo.

---

## 5. Análise de Desempenho e Resultados

Os testes foram realizados com diferentes tamanhos de entrada (1k, 10k, 100k itens) usando sementes fixas para garantir a reprodutibilidade.

### 5.1. Resultados dos Benchmarks
| Itens Gerados | Tempo Total (ms) | Colisões (Divisão) | Colisões (Mult.) | Taxa de Itens Raros |
| :--- | :--- | :--- | :--- | :--- |
| 1.000 | 0.11 | 500 | 500 | 10.0% |
| 10.000 | 1.09 | 5.000 | 5.000 | 9.98% |
| 100.000 | 17.27 | 50.000 | 50.000 | 9.99% |

### 5.2. Análise Crítica
-   **Eficiência da Hash:** Com chaves sequenciais, ambos os métodos apresentaram número de colisões idêntico ao carregar a tabela com o dobro de itens em relação ao seu tamanho (Load Factor = 2.0). O tempo de busca manteve-se constante em torno de 0.001 ms para 1000 buscas, demonstrando a escalabilidade do $O(1)$ amortizado.
-   **Custo do Heap:** Para 100.000 itens, a extração dos 1.000 itens mais raros levou apenas 0.78 ms, confirmando a eficiência do $O(\log N)$.
-   **Precisão do Drop:** O mecanismo de balanceamento foi extremamente preciso, mantendo a taxa de itens raros em aproximadamente 9.99% mesmo em grandes volumes de dados.

---

## 6. Conclusão
A integração das estruturas de dados propostas permitiu a criação de um sistema robusto e performático. A escolha de implementações manuais de PRNGs e funções hash específicas permitiu um controle fino sobre o comportamento do sistema, atendendo a todos os requisitos mínimos do projeto e garantindo um ambiente de testes justo e reprodutível.
