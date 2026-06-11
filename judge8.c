// Ana Beatriz Gualti Scalabrini 168706
// Seleção por Substituição utilizando um Min-Heap de tamanho M.
// Intercalação Balanceada utilizando um Min-Heap de tamanho K.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// fita magnetica
typedef struct {
    int *data;
    int capacity;
    int size;
    
    int *block_sizes;
    int bs_capacity;
    int num_blocks;
    
    int read_pos;
    int current_block;
    int elements_read_in_current_block;
} Tape;

// Inicializa uma fita vazia
void init_tape(Tape *t) {
    t->capacity = 100;
    t->data = (int *)malloc(t->capacity * sizeof(int));
    t->size = 0;
    
    t->bs_capacity = 10;
    t->block_sizes = (int *)malloc(t->bs_capacity * sizeof(int));
    t->num_blocks = 0;
    
    t->read_pos = 0;
    t->current_block = 0;
    t->elements_read_in_current_block = 0;
}

// Escreve um valor no final da fita
void write_element(Tape *t, int val) {
    if (t->size >= t->capacity) {
        t->capacity *= 2;
        t->data = (int *)realloc(t->data, t->capacity * sizeof(int));
    }
    t->data[t->size++] = val;
}

// Finaliza o bloco atual na fita, registrando seu tamanho
void end_block(Tape *t) {
    if (t->num_blocks >= t->bs_capacity) {
        t->bs_capacity *= 2;
        t->block_sizes = (int *)realloc(t->block_sizes, t->bs_capacity * sizeof(int));
    }
    int elements_in_previous_blocks = 0;
    for (int i = 0; i < t->num_blocks; i++) {
        elements_in_previous_blocks += t->block_sizes[i];
    }
    t->block_sizes[t->num_blocks++] = t->size - elements_in_previous_blocks;
}

// Limpa o conteúdo da fita para ser reutilizada como destino
void clear_tape(Tape *t) {
    t->size = 0;
    t->num_blocks = 0;
    t->read_pos = 0;
    t->current_block = 0;
    t->elements_read_in_current_block = 0;
}

/* ================= FUNÇÕES DE HEAP ================= */

// Sift-Down para a Fase 1 (Min-Heap simples)
void sift_down(int *heap, int i, int size) {
    int min_idx = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    
    if (left < size && heap[left] < heap[min_idx]) min_idx = left;
    if (right < size && heap[right] < heap[min_idx]) min_idx = right;
    
    if (min_idx != i) {
        int temp = heap[i];
        heap[i] = heap[min_idx];
        heap[min_idx] = temp;
        sift_down(heap, min_idx, size);
    }
}

// Nó do Heap para a Fase 2 (armazena valor e a fita de origem)
typedef struct {
    int val;
    int tape_id;
} Heap2Node;

// Sift-Down para a Fase 2 (Min-Heap com nós compostos)
void sift_down2(Heap2Node *heap, int i, int size) {
    int min_idx = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    
    if (left < size && heap[left].val < heap[min_idx].val) min_idx = left;
    if (right < size && heap[right].val < heap[min_idx].val) min_idx = right;
    
    if (min_idx != i) {
        Heap2Node temp = heap[i];
        heap[i] = heap[min_idx];
        heap[min_idx] = temp;
        sift_down2(heap, min_idx, size);
    }
}

/* ================= PROGRAMA PRINCIPAL ================= */

int main() {
    int N, M, K;
    if (scanf("%d %d %d", &N, &M, &K) != 3) return 0;
    
    int *input_data = (int *)malloc(N * sizeof(int));
    for (int i = 0; i < N; i++) {
        scanf("%d", &input_data[i]);
    }
    
    // Alocando e inicializando as 2*K fitas
    Tape *tapes = (Tape *)malloc(2 * K * sizeof(Tape));
    for (int i = 0; i < 2 * K; i++) {
        init_tape(&tapes[i]);
    }
    
    /* ================= FASE 1: SELEÇÃO POR SUBSTITUIÇÃO ================= */
    int *heap = (int *)malloc(M * sizeof(int));
    int total_in_heap = (N < M) ? N : M;
    int active_size = total_in_heap;
    int input_ptr = total_in_heap;
    
    for (int i = 0; i < total_in_heap; i++) {
        heap[i] = input_data[i];
    }
    
    for (int i = active_size / 2 - 1; i >= 0; i--) {
        sift_down(heap, i, active_size);
    }
    
    printf("[Fase 1 - Heap Inicial]:");
    for (int i = 0; i < active_size; i++) printf(" %d", heap[i]);
    printf("\n");
    
    int tape_idx = 0;
    bool first_block_done = false;
    
    while (1) {
        if (active_size == 0) {
            end_block(&tapes[tape_idx]);
            
            if (total_in_heap == 0) {
                break; // Processamento de todos os elementos concluído
            }
            
            if (!first_block_done) {
                printf("[Fase 1 - Heap Alterado]:");
                for (int i = 0; i < total_in_heap; i++) printf(" %d", heap[i]);
                printf("\n");
                first_block_done = true;
            }
            
            tape_idx = (tape_idx + 1) % K;
            active_size = total_in_heap;
            
            for (int i = active_size / 2 - 1; i >= 0; i--) {
                sift_down(heap, i, active_size);
            }
            continue;
        }
        
        int val = heap[0];
        write_element(&tapes[tape_idx], val);
        
        if (input_ptr < N) {
            int next_val = input_data[input_ptr++];
            if (next_val < val) {
                heap[0] = heap[active_size - 1];
                heap[active_size - 1] = next_val;
                active_size--;
                sift_down(heap, 0, active_size);
            } else {
                heap[0] = next_val;
                sift_down(heap, 0, active_size);
            }
        } else {
            heap[0] = heap[active_size - 1];
            active_size--;
            total_in_heap--;
            sift_down(heap, 0, active_size);
        }
    }
    
    printf("[Fase 1 - Fitas Geradas]:\n");
    for (int i = 0; i < K; i++) {
        printf("Fita %d:", i);
        if (tapes[i].num_blocks > 0) {
            int pos = 0;
            for (int b = 0; b < tapes[i].num_blocks; b++) {
                printf(" [");
                for (int e = 0; e < tapes[i].block_sizes[b]; e++) {
                    printf("%d", tapes[i].data[pos++]);
                    if (e < tapes[i].block_sizes[b] - 1) printf(" ");
                }
                printf("]");
            }
        }
        printf("\n");
    }
    
    /* ================= FASE 2: INTERCALAÇÃO BALANCEADA K-WAY ================= */
    int in_start = 0;
    int out_start = K;
    bool is_first_pass = true;
    
    while (1) {
        int out_idx = out_start;
        int total_input_blocks = 0;
        
        for (int i = in_start; i < in_start + K; i++) {
            tapes[i].read_pos = 0;
            tapes[i].current_block = 0;
            total_input_blocks += tapes[i].num_blocks;
        }
        
        if (total_input_blocks <= 1 && !is_first_pass) {
            break;
        }
        
        bool is_first_merge_step = true;
        int blocks_generated_this_pass = 0;
        
        while (total_input_blocks > 0) {
            int heap2_size = 0;
            Heap2Node *heap2 = (Heap2Node *)malloc(K * sizeof(Heap2Node));
            
            for (int i = in_start; i < in_start + K; i++) {
                if (tapes[i].current_block < tapes[i].num_blocks) {
                    heap2[heap2_size].val = tapes[i].data[tapes[i].read_pos];
                    heap2[heap2_size].tape_id = i;
                    tapes[i].elements_read_in_current_block = 0;
                    heap2_size++;
                }
            }
            
            for (int i = heap2_size / 2 - 1; i >= 0; i--) {
                sift_down2(heap2, i, heap2_size);
            }
            
            if (is_first_pass && is_first_merge_step) {
                printf("[Fase 2 - Heap Intercalacao]:");
                for (int i = 0; i < heap2_size; i++) printf(" %d", heap2[i].val);
                printf("\n");
                is_first_merge_step = false;
            }
            
            while (heap2_size > 0) {
                Heap2Node min_node = heap2[0];
                int t_id = min_node.tape_id;
                
                write_element(&tapes[out_idx], min_node.val);
                tapes[t_id].read_pos++;
                tapes[t_id].elements_read_in_current_block++;
                
                if (tapes[t_id].elements_read_in_current_block < tapes[t_id].block_sizes[tapes[t_id].current_block]) {
                    heap2[0].val = tapes[t_id].data[tapes[t_id].read_pos];
                    sift_down2(heap2, 0, heap2_size);
                } else {
                    heap2[0] = heap2[heap2_size - 1];
                    heap2_size--;
                    sift_down2(heap2, 0, heap2_size);
                    
                    tapes[t_id].current_block++;
                    total_input_blocks--;
                }
            }
            
            end_block(&tapes[out_idx]);
            blocks_generated_this_pass++;
            out_idx++;
            if (out_idx >= out_start + K) {
                out_idx = out_start;
            }
            free(heap2);
        }
        
        if (is_first_pass) {
            printf("[Fase 2 - Fitas apos 1a Passada]:\n");
            for (int i = out_start; i < out_start + K; i++) {
                printf("Fita %d:", i);
                if (tapes[i].num_blocks > 0) {
                    int pos = 0;
                    for (int b = 0; b < tapes[i].num_blocks; b++) {
                        printf(" [");
                        for (int e = 0; e < tapes[i].block_sizes[b]; e++) {
                            printf("%d", tapes[i].data[pos++]);
                            if (e < tapes[i].block_sizes[b] - 1) printf(" ");
                        }
                        printf("]");
                    }
                }
                printf("\n");
            }
            is_first_pass = false;
        }
        
        for (int i = in_start; i < in_start + K; i++) {
            clear_tape(&tapes[i]);
        }
        
        int temp = in_start;
        in_start = out_start;
        out_start = temp;
        
        if (blocks_generated_this_pass <= 1) {
            break;
        }
    }
    
    printf("[Resultado Final]:");
    Tape *final_tape = &tapes[in_start];
    for (int i = 0; i < final_tape->size; i++) {
        printf(" %d", final_tape->data[i]);
    }
    printf("\n");
    
    // Liberação de memória
    free(heap);
    free(input_data);
    for (int i = 0; i < 2 * K; i++) {
        free(tapes[i].data);
        free(tapes[i].block_sizes);
    }
    free(tapes);
    
    return 0;
}