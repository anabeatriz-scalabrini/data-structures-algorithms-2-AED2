/*
 * Atividade: Caminho Mínimo em Grafo com Restrição
 * Abordagens: Bellman-Ford e Dijkstra adaptados com restrição dinâmica.
 */

#include <stdio.h>
#include <stdlib.h>

#define INF 9999999

typedef struct no {
    int destino;
    int peso;
    struct no* prox;
} no;

// Função para inserir mantendo ordem crescente de destino
void inserir_aresta(no** grafo, int origem, int destino, int peso) {
    no* novo = (no*)malloc(sizeof(no));
    novo->destino = destino;
    novo->peso = peso;
    novo->prox = NULL;

    if (grafo[origem] == NULL || grafo[origem]->destino > destino) {
        novo->prox = grafo[origem];
        grafo[origem] = novo;
    } else {
        no* atual = grafo[origem];
        while (atual->prox != NULL && atual->prox->destino <= destino) {
            atual = atual->prox;
        }
        novo->prox = atual->prox;
        atual->prox = novo;
    }
}

// Imprime a rota final com tratamento de inatingíveis
void imprimir_caminho(int* pai, int origem, int destino) {
    if (pai[destino] == -1 && origem != destino) {
        printf("Destino nao alcancado\n");
        return;
    }
    
    int caminho[105];
    int tam = 0;
    int atual = destino;
    
    while (atual != -1) {
        caminho[tam++] = atual;
        atual = pai[atual];
    }
    
    for (int i = tam - 1; i >= 0; i--) {
        printf("%d", caminho[i]);
        if (i > 0) printf(" ");
    }
    printf("\n");
}

int main() {
    int origem_busca;
    if (scanf("%d", &origem_busca) != 1) return 0;

    int restri;
    scanf("%d", &restri);

    int num_nos, num_arestas;
    scanf("%d %d", &num_nos, &num_arestas);

    no** grafo = (no**)malloc(num_nos * sizeof(no*));
    for (int i = 0; i < num_nos; i++) {
        grafo[i] = NULL;
    }

    int tem_negativo = 0;

    for (int i = 0; i < num_arestas; i++) {
        int u, v, p;
        scanf("%d %d %d", &u, &v, &p);
        inserir_aresta(grafo, u, v, p);
        if (p < 0) tem_negativo = 1;
    }

    int destino_final;
    scanf("%d", &destino_final);

    // ==========================================
    // ALGORITMO 1: BELLMAN-FORD
    // ==========================================
    int dist_bf[105];
    int pai_bf[105];
    for (int i = 0; i < num_nos; i++) {
        dist_bf[i] = INF;
        pai_bf[i] = -1;
    }
    dist_bf[origem_busca] = 0;

    int atualizacoes = 0;

    for (int i = 0; i < num_nos - 1; i++) {
        for (int u = 0; u < num_nos; u++) {
            if (dist_bf[u] != INF) {
                no* atual = grafo[u];
                while (atual != NULL) {
                    int v = atual->destino;
                    int p = atual->peso;
                    int bloqueada = 0;
                    
                    if (p > restri && dist_bf[u] <= restri / 2.0) {
                        bloqueada = 1;
                    }
                    
                    if (!bloqueada) {
                        if (dist_bf[u] + p < dist_bf[v]) {
                            dist_bf[v] = dist_bf[u] + p;
                            pai_bf[v] = u;
                            atualizacoes++;
                        }
                    }
                    atual = atual->prox;
                }
            }
        }
    }

    for (int i = 0; i < num_nos; i++) {
        if (dist_bf[i] == INF) printf("INF ");
        else printf("%d ", dist_bf[i]);
    }
    printf("| Upd: %d\n", atualizacoes);

    imprimir_caminho(pai_bf, origem_busca, destino_final);

    if (tem_negativo) {
        printf("SOLUCAO NAO CONFIAVEL PARA DIJKSTRA\n");
    }

    // ==========================================
    // ALGORITMO 2: DIJKSTRA
    // ==========================================
    int dist_dj[105];
    int pai_dj[105];
    int visitado[105];
    for (int i = 0; i < num_nos; i++) {
        dist_dj[i] = INF;
        pai_dj[i] = -1;
        visitado[i] = 0;
    }
    dist_dj[origem_busca] = 0;

    int seq[105];
    int tam_seq = 0;

    for (int i = 0; i < num_nos; i++) {
        int u = -1;
        int min_dist = INF;
        
        // Busca linear com `<=` para desempate correto (maior índice)
        // O `dist_dj[j] < INF` evita que vértices inatingíveis sejam processados
        for (int j = 0; j < num_nos; j++) {
            if (!visitado[j] && dist_dj[j] < INF && dist_dj[j] <= min_dist) {
                min_dist = dist_dj[j];
                u = j;
            }
        }

        // Interrompe imediatamente se todos os restantes forem inatingíveis
        if (u == -1) break;

        visitado[u] = 1;
        seq[tam_seq++] = u;

        no* atual = grafo[u];
        while (atual != NULL) {
            int v = atual->destino;
            int p = atual->peso;
            int bloqueada = 0;
            
            if (p > restri && dist_dj[u] <= restri / 2.0) {
                bloqueada = 1;
            }
            
            if (!bloqueada && !visitado[v]) {
                if (dist_dj[u] + p < dist_dj[v]) {
                    dist_dj[v] = dist_dj[u] + p;
                    pai_dj[v] = u;
                }
            }
            atual = atual->prox;
        }
    }

    for (int i = 0; i < num_nos; i++) {
        if (dist_dj[i] == INF) printf("INF ");
        else printf("%d ", dist_dj[i]);
    }
    printf("| Seq: ");
    for (int i = 0; i < tam_seq; i++) {
        printf("%d", seq[i]);
        if (i < tam_seq - 1) printf(" ");
    }
    printf("\n");

    imprimir_caminho(pai_dj, origem_busca, destino_final);

    return 0;
}