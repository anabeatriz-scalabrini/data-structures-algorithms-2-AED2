// Ana Beatriz Gualti Scalabrini 168706

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// estrutura do no da arvore B+
typedef struct NoArvore {
    bool folha;
    int num_chaves;
    int *chaves;
    struct NoArvore **filhos;
    struct NoArvore *prox; // ponteiro para interligar as folhas (lista encadeada)
} NoArvore;

// estrutura auxiliar para gerenciar a promocao de chaves durante o split
typedef struct Promocao {
    bool ocorreu;
    int chave_promovida;
    NoArvore *novo_filho;
} Promocao;

// funcao para criar e alocar dinamicamente um novo no
NoArvore* criar_no(int m, bool folha) {
    NoArvore *no = (NoArvore*)malloc(sizeof(NoArvore));
    no->folha = folha;
    no->num_chaves = 0;
    // aloca espaco extra (2m + 1) para permitir o transbordo antes do split
    no->chaves = (int*)malloc((2 * m + 1) * sizeof(int));
    no->filhos = (NoArvore**)malloc((2 * m + 2) * sizeof(NoArvore*));
    no->prox = NULL;
    return no;
}

// funcao recursiva para inserir e tratar divisoes bottom-up
Promocao inserir_recursivo(NoArvore *no, int chave, int m) {
    Promocao prom;
    prom.ocorreu = false;

    if (no->folha) {
        // insere na folha mantendo a ordenacao
        int i = no->num_chaves - 1;
        while (i >= 0 && no->chaves[i] > chave) {
            no->chaves[i + 1] = no->chaves[i];
            i--;
        }
        no->chaves[i + 1] = chave;
        no->num_chaves++;

        // verifica se a folha atingiu o limite de transbordo (2m + 1 chaves)
        if (no->num_chaves > 2 * m) {
            int indice_m = m;
            prom.chave_promovida = no->chaves[indice_m];
            prom.ocorreu = true;

            NoArvore *novo_no = criar_no(m, true);
            
            // na folha, o elemento mediano e copiado para a nova folha (duplicacao)
            int j = 0;
            for (int k = indice_m; k < no->num_chaves; k++) {
                novo_no->chaves[j++] = no->chaves[k];
                novo_no->num_chaves++;
            }
            no->num_chaves = indice_m; 

            // interliga a nova folha na lista encadeada simples
            novo_no->prox = no->prox;
            no->prox = novo_no;

            prom.novo_filho = novo_no;
        }
        return prom;
    } else {
        // encontra o indice do filho para descer na arvore
        int i = 0;
        while (i < no->num_chaves && chave >= no->chaves[i]) {
            i++;
        }

        Promocao prom_filho = inserir_recursivo(no->filhos[i], chave, m);

        // se houve divisao no filho, processa a promocao da chave
        if (prom_filho.ocorreu) {
            int j = no->num_chaves - 1;
            while (j >= i) {
                no->chaves[j + 1] = no->chaves[j];
                no->filhos[j + 2] = no->filhos[j + 1];
                j--;
            }
            no->chaves[i] = prom_filho.chave_promovida;
            no->filhos[i + 1] = prom_filho.novo_filho;
            no->num_chaves++;

            // verifica se o no interno estourou a capacidade
            if (no->num_chaves > 2 * m) {
                int indice_m = m;
                prom.chave_promovida = no->chaves[indice_m];
                prom.ocorreu = true;

                NoArvore *novo_no = criar_no(m, false);
                
                // no interno, a chave promovida sobe e nao fica (promocao classica)
                int k_novo = 0;
                for (int k = indice_m + 1; k < no->num_chaves; k++) {
                    novo_no->chaves[k_novo] = no->chaves[k];
                    novo_no->filhos[k_novo] = no->filhos[k];
                    novo_no->num_chaves++;
                    k_novo++;
                }
                novo_no->filhos[k_novo] = no->filhos[no->num_chaves];

                no->num_chaves = indice_m;
                prom.novo_filho = novo_no;
            }
        }
        return prom;
    }
}

// funcao para buscar elemento e imprimir exatamente o formato exigido
void buscar_e_imprimir(NoArvore *raiz, int chave) {
    NoArvore *no_atual = raiz;
    int paginas_visitadas = 0;

    // desce ate o nivel folha
    while (!no_atual->folha) {
        paginas_visitadas++;
        int i = 0;
        while (i < no_atual->num_chaves && chave >= no_atual->chaves[i]) {
            i++;
        }
        no_atual = no_atual->filhos[i];
    }

    // contabiliza a visita na pagina folha
    paginas_visitadas++;
    
    bool encontrado = false;
    for (int i = 0; i < no_atual->num_chaves; i++) {
        if (no_atual->chaves[i] == chave) {
            encontrado = true;
            break;
        }
    }

    // 2. imprime a quantidade de paginas (nos) visitadas
    printf("%d\n", paginas_visitadas);

    // 3. imprime todos os elementos da pagina folha ou a mensagem de erro
    if (encontrado) {
        for (int i = 0; i < no_atual->num_chaves; i++) {
            printf("%d", no_atual->chaves[i]);
            if (i < no_atual->num_chaves - 1) {
                printf(" ");
            }
        }
        printf("\n");
    } else {
        printf("Valor nao encontrado\n");
    }
}

// Funcao recursiva para liberar toda a memoria alocada pela arvore
void liberar_arvore(NoArvore *no) {
    // Caso base: se o no for nulo, nao ha o que liberar
    if (no == NULL) {
        return;
    }

    // Se nao for folha, precisa descer e liberar todos os filhos primeiro
    if (!no->folha) {
        for (int i = 0; i <= no->num_chaves; i++) {
            liberar_arvore(no->filhos[i]);
        }
    }

    // Libera os arrays dinamicos alocados dentro do no na funcao 'criar_no'
    free(no->chaves);
    free(no->filhos);
    
    // Por fim, libera a memoria do proprio no
    free(no);
}


// funcao principal
int main() {
    int m;
    
    // le a ordem m
    if (scanf("%d", &m) != 1) {
        return 0;
    }

    NoArvore *raiz = criar_no(m, true);
    int chave;

    // leitura e insercao dinamica dos dados
    while (scanf("%d", &chave) == 1 && chave >= 0) {
        Promocao prom = inserir_recursivo(raiz, chave, m);
        
        // se a raiz antiga sofreu split, cria uma nova raiz acima dela
        if (prom.ocorreu) {
            NoArvore *nova_raiz = criar_no(m, false);
            nova_raiz->chaves[0] = prom.chave_promovida;
            nova_raiz->filhos[0] = raiz;
            nova_raiz->filhos[1] = prom.novo_filho;
            nova_raiz->num_chaves = 1;
            raiz = nova_raiz;
        }
    }

    int chave_busca;
    
    // leitura do elemento a ser buscado
    if (scanf("%d", &chave_busca) == 1) {
        // 1. imprime a quantidade total de chaves contidas na pagina raiz
        printf("%d\n", raiz->num_chaves);
        
        // chama a funcao que lida com o restante da saida
        buscar_e_imprimir(raiz, chave_busca);
    }

    liberar_arvore(raiz);

    return 0;
}