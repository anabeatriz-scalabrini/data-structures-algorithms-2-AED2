#include <stdio.h>
#include <stdlib.h>

// estrutura do no da lista duplamente encadeada
typedef struct No {
    int chave;
    struct No* proximo;
    struct No* anterior;
} No;

// estrutura da lista
typedef struct Lista {
    No* inicio;
    No* fim;
} Lista;

// funcao para inserir na lista mantendo a ordem crescente
void inserir_ordenado(Lista* lista, int valor) {
    No* novo_no = (No*)malloc(sizeof(No));
    novo_no->chave = valor;
    novo_no->proximo = NULL;
    novo_no->anterior = NULL;

    // se a lista estiver vazia, o novo no e o inicio e o fim
    if (lista->inicio == NULL) {
        lista->inicio = novo_no;
        lista->fim = novo_no;
        return;
    }

    // insercao no inicio se o valor for menor ou igual ao atual head
    if (valor <= lista->inicio->chave) {
        novo_no->proximo = lista->inicio;
        lista->inicio->anterior = novo_no;
        lista->inicio = novo_no;
        return;
    }

    // insercao no fim se o valor for maior ou igual ao atual tail
    if (valor >= lista->fim->chave) {
        novo_no->anterior = lista->fim;
        lista->fim->proximo = novo_no;
        lista->fim = novo_no;
        return;
    }

    // percorre a lista para encontrar a posicao correta no meio
    No* atual = lista->inicio;
    while (atual != NULL && atual->chave < valor) {
        atual = atual->proximo;
    }

    // insere o novo no antes do no atual que e maior que ele
    novo_no->proximo = atual;
    novo_no->anterior = atual->anterior;
    atual->anterior->proximo = novo_no;
    atual->anterior = novo_no;
}

int main() {
    int tamanho_tabela;
    
    // leitura do tamanho da tabela hash
    if (scanf("%d", &tamanho_tabela) != 1) return 0;

    // alocacao de memoria e inicializacao da tabela hash
    Lista* tabela_hash = (Lista*)malloc(tamanho_tabela * sizeof(Lista));
    for (int i = 0; i < tamanho_tabela; i++) {
        tabela_hash[i].inicio = NULL;
        tabela_hash[i].fim = NULL;
    }

    int chave_entrada;
    
    // leitura dos dados e insercao ordenada mapeada pela funcao hash
    while (scanf("%d", &chave_entrada) == 1 && chave_entrada != -1) {
        int indice = chave_entrada % tamanho_tabela; //funçao de hash
        inserir_ordenado(&tabela_hash[indice], chave_entrada);
    }

    int chave_buscada;
    if (scanf("%d", &chave_buscada) != 1) return 0;

    int indice_busca = chave_buscada % tamanho_tabela;
    Lista* lista_busca = &tabela_hash[indice_busca];

    // avaliacao inicial da lista para a chave buscada
    if (lista_busca->inicio == NULL) {
        printf("NEP: 0 (Lista Vazia)\n");
        printf("Valor nao encontrado\n");
    } else {
        // calculo das distancias absolutas
        int distancia_inicio = abs(chave_buscada - lista_busca->inicio->chave);
        int distancia_fim = abs(lista_busca->fim->chave - chave_buscada);
        
        int nep = 0;
        int encontrou = 0;

        // condicao para busca partindo do inicio (head)
        if (distancia_inicio <= distancia_fim) {
            No* atual = lista_busca->inicio;
            while (atual != NULL) {
                nep++;
                
                // interrupcao por corte logico em ordem crescente
                if (atual->chave > chave_buscada) {
                    break;
                }
                
                if (atual->chave == chave_buscada) {
                    encontrou = 1;
                    No* remover = atual;
                    atual = atual->proximo;

                    // logica de remocao de chave encontrada
                    if (remover->anterior != NULL) remover->anterior->proximo = remover->proximo;
                    else lista_busca->inicio = remover->proximo;
                    
                    if (remover->proximo != NULL) remover->proximo->anterior = remover->anterior;
                    else lista_busca->fim = remover->anterior;
                    
                    free(remover);
                } else {
                    atual = atual->proximo;
                }
            }
            printf("NEP: %d (Sentido: Inicio)\n", nep);
        } else {
            // condicao para busca partindo do fim (tail)
            No* atual = lista_busca->fim;
            while (atual != NULL) {
                nep++;
                
                // interrupcao por corte logico em ordem decrescente
                if (atual->chave < chave_buscada) {
                    break; 
                }
                
                if (atual->chave == chave_buscada) {
                    encontrou = 1;
                    No* remover = atual;
                    atual = atual->anterior;

                    // logica de remocao de chave encontrada
                    if (remover->anterior != NULL) remover->anterior->proximo = remover->proximo;
                    else lista_busca->inicio = remover->proximo;
                    
                    if (remover->proximo != NULL) remover->proximo->anterior = remover->anterior;
                    else lista_busca->fim = remover->anterior;
                    
                    free(remover);
                } else {
                    atual = atual->anterior;
                }
            }
            printf("NEP: %d (Sentido: Fim)\n", nep);
        }

        // impressao caso o laco de varredura acabe sem encontrar o valor
        if (!encontrou) {
            printf("Valor nao encontrado\n");
        }
    }

    // impressao do estado final de toda a tabela
    for (int i = 0; i < tamanho_tabela; i++) {
        if (tabela_hash[i].inicio == NULL) {
            printf("%d :: NULL\n", i);
        } else {
            printf("%d :: ", i);
            
            // impressao no sentido direto
            No* atual = tabela_hash[i].inicio;
            while (atual != NULL) {
                printf("%d -> ", atual->chave);
                atual = atual->proximo;
            }
            printf("NULL :: ");
            
            // impressao no sentido reverso
            atual = tabela_hash[i].fim;
            while (atual != NULL) {
                printf("%d -> ", atual->chave);
                atual = atual->anterior;
            }
            printf("NULL\n");
        }
    }

    // liberacao dos ponteiros residuais para evitar vazamento de memoria
    for (int i = 0; i < tamanho_tabela; i++) {
        No* atual = tabela_hash[i].inicio;
        while (atual != NULL) {
            No* proximo = atual->proximo;
            free(atual);
            atual = proximo;
        }
    }
    free(tabela_hash);

    return 0;
}