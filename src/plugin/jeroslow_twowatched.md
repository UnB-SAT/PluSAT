# Plugin PluSAT: Jeroslow-Wang e Two Watched Literals

Autores: [Guilherme Westphall](https://github.com/west7) e [Lucas Martins Gabriel](https://github.com/martinsglucas)

> O PluSAT é um SAT Solver modular que permite a criação de plugins para resolver problemas de satisfatibilidade booleana (SAT). Os plugins implementam as principais etapas de um algoritmo SAT DPLL.

Este plugin implementa:

* A **heurística de decisão de variáveis** de *Jeroslow-Wang*.
* A técnica **Two Watched Literals** para otimizar o processo de BCP (*Boolean Constraint Propagation*).


## Heurística de Decisão de Variáveis: Jeroslow-Wang

A heurística de Jeroslow-Wang seleciona a variável de decisão com base na frequência de ocorrência e no tamanho das cláusulas onde ela aparece. A ideia é favorecer variáveis que aparecem em cláusulas menores e mais numerosas, atribuindo maior peso a essas ocorrências. Ao iniciar o solver, no pré-processamento, calcula-se o score de cada literal e armazena em um vetor. Depois, verificamos se o score do literal positivo é maior que o negativo. Se for, escolhemos TRUE, caso contrário, escolhemos FALSE. Ordenamos o vetor de scores e escolhemos o maior score a cada decisão. A fórmula para calcular o score de uma variável ( l ) é dada por:   

$$
score(l) = \sum _{c \in C} 2^{-|c|}    
$$

Onde:

- $l$ é o literal.
- $c$ é uma cláusula contendo $l$.
- $|c|$ é o tamanho da cláusula.
- $C$ é o conjunto de cláusulas onde $l$ aparece.

## Two Watched Literals

O Two Watched Literals é uma técnica de otimização que melhora a eficiência do processo de BCP. Em vez de monitorar todos os literais de uma cláusula, o algoritmo mantém dois "watched literals" (literais observados) para cada cláusula. Com isso, o BCP só precisa verificar caso algum dos dois literais observados for atribuído como FALSO. O algoritmo então verifica se há outro literal na cláusula que ainda não foi atribuído como FALSO através da função `find_and_swap()`. Se houver, ele passa a ser o novo "watched literal". Se não houver, a cláusula se torna unitária, com somente um literal relevante. Caso ele for TRUE, a cláusula é satisfeita (nada a ser feito), se for não-valorado, é feita a implicação unitária (o literal **deve** ser TRUE para satisfazer a cláusula), e se for FALSO, o algoritmo detecta um conflito. Toda implicação unitária gera uma nova atribuição e o literal oposta é adicionado à fila de propagação para continuar o processo de BCP.

Vantagens de usar o Two Watched Literals:
- Lazy verification: O algoritmo só verifica as cláusulas quando elas se tornam críticas.
- Detecta conflitos rapidamente: Se uma cláusula se torna unitária, o algoritmo pode detectar rapidamente se há um conflito.

## Estrutura do Plugin

- `PreProcessing(Form *form)`: Calcula os scores dos literais, ordena o vetor e aloca as estruturas necessárias para o Two Watched Literals.
- `Decide(const Form *form)`: Consome o vetor de `scores`.
- `BCP(Form *form, const Decision decision)`: Implementa o Two Watched Literals, propagando as implicações unitárias e detectando conflitos.
- `resolveConflict()`: Realiza o backtracking retornando ao nível de decisão do conflito.
