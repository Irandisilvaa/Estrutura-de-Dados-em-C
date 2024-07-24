#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
A empresa de tecnologia Poxim Tech está
desenvolvendo uma rede social para os melhores
amigos, com a ideia de unir as pessoas como se
estivessem de mãos dadas através de um círculo de
pessoas que interagem com os vizinhos
▶ Os nomes dos usuários desta rede são compostos
exclusivamente por letras com até 50 caracteres
▶ Quando um usuário é adicionado ele sempre será
amigo do último e do primeiro usuário da rede social
▶ Caso seja removido da rede social, os amigos do
usuário passam a ser amigos entre si
▶ É possível buscar uma determinada pessoa através
do seu nome e mostrar os nomes de seus amigos
*/


//definição de structs para uso

typedef struct no {
    char nome[51];
    struct no *anterior;
    struct no *proximo;
} No;

typedef struct {
    int contador;
    No *inicio;
    No *final;
} Lista;

No* criarNo (const char *nome) {
    No *novo = (No *)malloc(sizeof(No));

    if (novo) {
        strcpy(novo->nome, nome);
        novo->anterior = NULL;
        novo->proximo = NULL;
    }
    return novo;
}


void inserirNo (Lista *lista, const char *nome) {
    No *novo = criarNo(nome);
    if (!novo) {
        printf("Erro ao alocar memória para novo nó.\n");
        return;
    }

    if (lista->inicio == NULL) {
        lista->inicio = novo;
        lista->final = novo;
        novo->proximo = novo;
        novo->anterior = novo;
    } else {
        // Lista não vazia
        novo->proximo = lista->inicio;
        novo->anterior = lista->final;
        lista->final->proximo = novo;
        lista->inicio->anterior = novo;
        lista->final = novo;
    }
    lista->contador++;
}

No* removerNo (Lista *lista, const char *elemento) {
    if (lista->inicio == NULL) {
        return NULL;}

    No *remover = NULL;

    // lista com um único nó
    if (lista->inicio == lista->final) {
        if (strcmp(lista->inicio->nome, elemento) == 0) {
            remover = lista->inicio;
            lista->inicio = NULL;
            lista->final = NULL;
            lista->contador--;
        }
        return remover;
    }

    if (strcmp(lista->inicio->nome, elemento) == 0) {
        remover = lista->inicio;
        lista->inicio = remover->proximo;
        lista->final->proximo = lista->inicio;
        lista->inicio->anterior = lista->final;
        lista->contador--;
        return remover;
    }

    No *atual = lista->inicio;
    do {
        if (strcmp(atual->proximo->nome, elemento) == 0) {
            remover = atual->proximo;
            if (remover == lista->final) {
                lista->final = atual;
            }
            atual->proximo = remover->proximo;
            remover->proximo->anterior = atual;
            lista->contador--;
            break;
        }
        atual = atual->proximo;
    } while (atual != lista->inicio);

    return remover;
}


No* buscarNo (Lista *lista, const char *nome) {
    if (lista->inicio == NULL) {
        return NULL; 
    }
    No *atual = lista->inicio;
    do {
        if (strcmp(atual->nome, nome) == 0) {
            return atual;
        }
        atual = atual->proximo;
    } while (atual != lista->inicio);

    return NULL; 
}


void mostrarNo (Lista *lista, const char *nome, FILE *saida) {
    No *no = buscarNo(lista, nome);
    if (no) {
        fprintf(saida, "[ OK  ] %s<-%s->%s\n", no->anterior->nome, no->nome, no->proximo->nome);
    } else {
        fprintf(saida, "[ERROR] SHOW %s\n", nome);
    }
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <arquivo de entrada> <arquivo de saída>\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (input == NULL) {
        perror("Erro ao abrir o arquivo de entrada");
        return 1;
    }

    FILE *output = fopen(argv[2], "w");
    if (output == NULL) {
        perror("Erro ao abrir o arquivo de saída");
        fclose(input);
        return 1;
    }

    Lista lista = {0, NULL, NULL};

    char linha[100];
    char comando[10];
    char nome[51];

    while (fgets(linha, sizeof(linha), input)) {
        linha[strcspn(linha, "\n")] = '\0'; 
        if (sscanf(linha, "%s %50[^\n]", comando, nome) == 2) {
            
            for (char *p = comando; *p; ++p) {
                *p = toupper(*p);
            }

            int adicionar = strcmp(comando, "ADD") == 0;
            int remover = strcmp(comando, "REMOVE") == 0;
            int show = strcmp(comando, "SHOW") == 0;

            if (adicionar) {

                No *noExistente = buscarNo(&lista, nome);
                if (noExistente) {
                    fprintf(output, "[ERROR] ADD %s\n", nome);
                } else {
                    inserirNo(&lista, nome);
                    fprintf(output, "[ OK  ] ADD %s\n", nome);
                }

            } else if (remover) {

                No *noRemovido = removerNo(&lista, nome);

                if (noRemovido ) {
                    fprintf(output, "[ OK  ] REMOVE %s\n", nome);
                    free(noRemovido );
                } else {
                    fprintf(output, "[ERROR] REMOVE %s\n", nome);
                }

            } else if (show) {
                mostrarNo (&lista, nome, output);
            } else {
                fprintf(output, "[ERROR] Comando inválido: %s\n", linha);
            }
        } else {
            fprintf(output, "[ERROR] Formato de linha inválido: %s\n", linha);
        }
    }

    fclose(input);
    fclose(output);
    return 0;
}
