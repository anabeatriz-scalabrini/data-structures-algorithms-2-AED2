#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define TAM_PEDACO 512

// lista encadeada de array

typedef struct Pedaco {
    int dados[TAM_PEDACO];
    struct Pedaco *prox;
} Pedaco;

typedef struct {
    Pedaco *inicio;
    Pedaco *fim;
    int tamanho;
    
    int *tam_blocos;
    int cap_tam_blocos;
    int num_blocos;
    int cont_tam_bloco_atual; 
    
    int pos_leitura_no;
    int pos_escrita_no;
    int bloco_atual;
    int lidos_no_bloco_atual;
} Fita;

// prepara a fita zerando tudo
void iniciar_fita(Fita *f) {
    f->inicio = NULL;
    f->fim = NULL;
    f->tamanho = 0;
    
    f->cap_tam_blocos = 16;
    f->tam_blocos = (int *)malloc(f->cap_tam_blocos * sizeof(int));
    f->num_blocos = 0;
    f->cont_tam_bloco_atual = 0;
    
    f->pos_leitura_no = 0;
    f->pos_escrita_no = 0;
    f->bloco_atual = 0;
    f->lidos_no_bloco_atual = 0;
}

// escreve um numero no final da fita alocando novo pedaco se precisar
void escrever_elemento(Fita *f, int valor) {
    if (f->fim == NULL || f->pos_escrita_no == TAM_PEDACO) {
        Pedaco *novo_pedaco = (Pedaco *)malloc(sizeof(Pedaco));
        novo_pedaco->prox = NULL;
        if (f->fim) {
            f->fim->prox = novo_pedaco;
        } else {
            f->inicio = novo_pedaco;
        }
        f->fim = novo_pedaco;
        f->pos_escrita_no = 0;
    }
    f->fim->dados[f->pos_escrita_no++] = valor;
    f->tamanho++;
    f->cont_tam_bloco_atual++;
}

// registra o tamanho do bloco atual para a fase de intercalacao
void finalizar_bloco(Fita *f) {
    if (f->cont_tam_bloco_atual == 0) return;
    
    if (f->num_blocos >= f->cap_tam_blocos) {
        f->cap_tam_blocos *= 2;
        f->tam_blocos = (int *)realloc(f->tam_blocos, f->cap_tam_blocos * sizeof(int));
    }
    f->tam_blocos[f->num_blocos++] = f->cont_tam_bloco_atual;
    f->cont_tam_bloco_atual = 0;
}

// pega o valor atual que o ponteiro ta lendo
int ler_elemento_atual(Fita *f) {
    return f->inicio->dados[f->pos_leitura_no];
}

// avanca leitura e ja da free no pedaco lido pra nao dar memory limit
void avancar_ponteiro_leitura(Fita *f) {
    f->pos_leitura_no++;
    if (f->pos_leitura_no == TAM_PEDACO) {
        Pedaco *temp = f->inicio;
        f->inicio = f->inicio->prox;
        free(temp); 
        f->pos_leitura_no = 0;
        
        if (f->inicio == NULL) {
            f->fim = NULL;
            f->pos_escrita_no = 0;
        }
    }
}

// recicla a fita limpando todos os pedacos da memoria
void limpar_fita(Fita *f) {
    Pedaco *atual = f->inicio;
    while (atual) {
        Pedaco *temp = atual;
        atual = atual->prox;
        free(temp);
    }
    f->inicio = NULL;
    f->fim = NULL;
    f->tamanho = 0;
    f->num_blocos = 0;
    f->cont_tam_bloco_atual = 0;
    
    f->pos_leitura_no = 0;
    f->pos_escrita_no = 0;
    f->bloco_atual = 0;
    f->lidos_no_bloco_atual = 0;
}

// funcoes do heap

// ajeita o min heap da primeira fase
void descer_heap(int *heap, int i, int tamanho) {
    int idx_menor = i;
    int esq = 2 * i + 1;
    int dir = 2 * i + 2;
    
    if (esq < tamanho && heap[esq] < heap[idx_menor]) idx_menor = esq;
    if (dir < tamanho && heap[dir] < heap[idx_menor]) idx_menor = dir;
    
    if (idx_menor != i) {
        int temp = heap[i];
        heap[i] = heap[idx_menor];
        heap[idx_menor] = temp;
        descer_heap(heap, idx_menor, tamanho);
    }
}

// guarda o valor e de qual fita ele veio pra fase 2
typedef struct {
    int valor;
    int id_fita;
} NoHeapFase2;

// ajeita o min heap modificado da segunda fase
void descer_heap_fase2(NoHeapFase2 *heap, int i, int tamanho) {
    int idx_menor = i;
    int esq = 2 * i + 1;
    int dir = 2 * i + 2;
    
    if (esq < tamanho && heap[esq].valor < heap[idx_menor].valor) idx_menor = esq;
    if (dir < tamanho && heap[dir].valor < heap[idx_menor].valor) idx_menor = dir;
    
    if (idx_menor != i) {
        NoHeapFase2 temp = heap[i];
        heap[i] = heap[idx_menor];
        heap[idx_menor] = temp;
        descer_heap_fase2(heap, idx_menor, tamanho);
    }
}

int main() {
    int N, M, K;
    if (scanf("%d %d %d", &N, &M, &K) != 3) return 0;
    if (N == 0) return 0;
    
    Fita *fitas = (Fita *)malloc(2 * K * sizeof(Fita));
    for (int i = 0; i < 2 * K; i++) {
        iniciar_fita(&fitas[i]);
    }
    
    // comecando a fase 1 de selecao por substituicao
    int total_lidos = 0;
    int *heap = (int *)malloc(M * sizeof(int));
    
    int total_no_heap = (N < M) ? N : M;
    int tam_ativo = total_no_heap;
    int tam_morto = 0;
    
    for (int i = 0; i < total_no_heap; i++) {
        scanf("%d", &heap[i]);
        total_lidos++;
    }
    
    // build heap inicial
    for (int i = tam_ativo / 2 - 1; i >= 0; i--) {
        descer_heap(heap, i, tam_ativo);
    }
    
    // saida obrigatoria da primeira parte
    printf("[Fase 1 - Heap Inicial]:");
    for (int i = 0; i < tam_ativo; i++) printf(" %d", heap[i]);
    printf("\n");
    
    int idx_fita = 0;
    bool primeiro_bloco_pronto = false;
    
    while (1) {
        if (tam_ativo == 0) {
            finalizar_bloco(&fitas[idx_fita]);
            
            if (tam_morto == 0) {
                break; // acabou tudo e nao tem mais nada na area morta
            }
            
            // puxa os elementos mortos pro comeco do array pra montar o bloco novo
            for (int i = 0; i < tam_morto; i++) {
                heap[i] = heap[total_no_heap - tam_morto + i];
            }
            
            total_no_heap = tam_morto;
            tam_ativo = tam_morto;
            tam_morto = 0;
            
            // printa soh uma vez quando o heap enche com os mortos
            if (!primeiro_bloco_pronto) {
                printf("[Fase 1 - Heap Alterado]:");
                for (int i = 0; i < tam_ativo; i++) printf(" %d", heap[i]);
                printf("\n");
                primeiro_bloco_pronto = true;
            }
            
            idx_fita = (idx_fita + 1) % K;
            
            // refaz o heap pro novo bloco
            for (int i = tam_ativo / 2 - 1; i >= 0; i--) {
                descer_heap(heap, i, tam_ativo);
            }
            continue;
        }
        
        int valor = heap[0];
        escrever_elemento(&fitas[idx_fita], valor);
        
        // le mais coisas se ainda tiver entrada
        if (total_lidos < N) {
            int prox_valor;
            scanf("%d", &prox_valor);
            total_lidos++;
            
            if (prox_valor < valor) {
                // menor que o recem tirado entao vai pra area morta
                heap[0] = heap[tam_ativo - 1];
                heap[tam_ativo - 1] = prox_valor;
                tam_ativo--;
                tam_morto++;
                descer_heap(heap, 0, tam_ativo);
            } else {
                // entra no lugar normal
                heap[0] = prox_valor;
                descer_heap(heap, 0, tam_ativo);
            }
        } else {
            // acabo o arquivo entao soh tira e reduz tamanho ativo sem bugar os mortos
            heap[0] = heap[tam_ativo - 1];
            tam_ativo--;
            descer_heap(heap, 0, tam_ativo);
        }
    }
    
    // print das fitas apos gerar os blocos
    printf("[Fase 1 - Fitas Geradas]:\n");
    for (int i = 0; i < K; i++) {
        printf("Fita %d:", i);
        if (fitas[i].num_blocos > 0) {
            Pedaco *atual = fitas[i].inicio;
            int pos = 0;
            for (int b = 0; b < fitas[i].num_blocos; b++) {
                printf(" [");
                for (int e = 0; e < fitas[i].tam_blocos[b]; e++) {
                    printf("%d", atual->dados[pos++]);
                    if (pos == TAM_PEDACO) {
                        atual = atual->prox;
                        pos = 0;
                    }
                    if (e < fitas[i].tam_blocos[b] - 1) printf(" ");
                }
                printf("]");
            }
        }
        printf("\n");
    }
    
    // comecando a fase 2 de intercalacao
    int inicio_entrada = 0;
    int inicio_saida = K;
    bool eh_primeira_passada = true;
    
    NoHeapFase2 *heap2 = (NoHeapFase2 *)malloc(K * sizeof(NoHeapFase2));
    
    while (1) {
        int idx_saida = inicio_saida;
        int total_blocos_entrada = 0;
        
        for (int i = inicio_entrada; i < inicio_entrada + K; i++) {
            fitas[i].pos_leitura_no = 0; 
            fitas[i].bloco_atual = 0;
            total_blocos_entrada += fitas[i].num_blocos;
        }
        
        // se sobrou um bloco ou nenhum ja ordenou tudo
        if (total_blocos_entrada <= 1 && !eh_primeira_passada) {
            break;
        }
        
        bool eh_primeira_mescla = true;
        int blocos_gerados_passada = 0;
        
        // mescla os blocos ate acabar
        while (total_blocos_entrada > 0) {
            int tam_heap2 = 0;
            
            for (int i = inicio_entrada; i < inicio_entrada + K; i++) {
                if (fitas[i].bloco_atual < fitas[i].num_blocos) {
                    heap2[tam_heap2].valor = ler_elemento_atual(&fitas[i]);
                    heap2[tam_heap2].id_fita = i;
                    fitas[i].lidos_no_bloco_atual = 0;
                    tam_heap2++;
                }
            }
            
            // arruma o heap2
            for (int i = tam_heap2 / 2 - 1; i >= 0; i--) {
                descer_heap_fase2(heap2, i, tam_heap2);
            }
            
            // printa uma vez o estado inicial do heap de intercalacao
            if (eh_primeira_passada && eh_primeira_mescla && total_blocos_entrada > 0) {
                printf("[Fase 2 - Heap Intercalacao]:");
                for (int i = 0; i < tam_heap2; i++) printf(" %d", heap2[i].valor);
                printf("\n");
                eh_primeira_mescla = false;
            }
            
            while (tam_heap2 > 0) {
                NoHeapFase2 menor_no = heap2[0];
                int id_t = menor_no.id_fita;
                
                escrever_elemento(&fitas[idx_saida], menor_no.valor);
                avancar_ponteiro_leitura(&fitas[id_t]); // free rola aqui
                fitas[id_t].lidos_no_bloco_atual++;
                
                // ve se ainda tem coisa nesse bloco pra jogar no heap
                if (fitas[id_t].lidos_no_bloco_atual < fitas[id_t].tam_blocos[fitas[id_t].bloco_atual]) {
                    heap2[0].valor = ler_elemento_atual(&fitas[id_t]);
                    descer_heap_fase2(heap2, 0, tam_heap2);
                } else {
                    // bloco acabou
                    heap2[0] = heap2[tam_heap2 - 1];
                    tam_heap2--;
                    descer_heap_fase2(heap2, 0, tam_heap2);
                    
                    fitas[id_t].bloco_atual++;
                    total_blocos_entrada--;
                }
            }
            
            finalizar_bloco(&fitas[idx_saida]);
            blocos_gerados_passada++;
            idx_saida++;
            
            // volta pro comeco do disco de saida se passou do K
            if (idx_saida >= inicio_saida + K) {
                idx_saida = inicio_saida;
            }
        }
        
        if (eh_primeira_passada) {
            printf("[Fase 2 - Fitas apos 1a Passada]:\n");
            for (int i = inicio_saida; i < inicio_saida + K; i++) {
                printf("Fita %d:", i);
                if (fitas[i].num_blocos > 0) {
                    Pedaco *atual = fitas[i].inicio;
                    int pos = 0;
                    for (int b = 0; b < fitas[i].num_blocos; b++) {
                        printf(" [");
                        for (int e = 0; e < fitas[i].tam_blocos[b]; e++) {
                            printf("%d", atual->dados[pos++]);
                            if (pos == TAM_PEDACO) {
                                atual = atual->prox;
                                pos = 0;
                            }
                            if (e < fitas[i].tam_blocos[b] - 1) printf(" ");
                        }
                        printf("]");
                    }
                }
                printf("\n");
            }
            eh_primeira_passada = false;
        }
        
        // limpa as fitas que acabaram de ser lidas
        for (int i = inicio_entrada; i < inicio_entrada + K; i++) {
            limpar_fita(&fitas[i]); 
        }
        
        // troca os discos de lugar
        int temp = inicio_entrada;
        inicio_entrada = inicio_saida;
        inicio_saida = temp;
        
        // se soh gerou um bloco entao ja era ta ordenado
        if (blocos_gerados_passada <= 1) {
            break;
        }
    }
    
    printf("[Resultado Final]:");
    Fita *fita_final = &fitas[inicio_entrada];
    Pedaco *atual = fita_final->inicio;
    int pos = 0;
    for (int i = 0; i < fita_final->tamanho; i++) {
        printf(" %d", atual->dados[pos++]);
        if (pos == TAM_PEDACO) {
            atual = atual->prox;
            pos = 0;
        }
    }
    printf("\n");
    
    // free final pra deixar a casa limpa
    for (int i = 0; i < 2 * K; i++) {
        limpar_fita(&fitas[i]);
        free(fitas[i].tam_blocos);
    }
    free(fitas);
    free(heap);
    free(heap2);
    
    return 0;
}