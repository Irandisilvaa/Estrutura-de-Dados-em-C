#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura do nó da árvore binária
typedef struct arquivo {
    int numInsercao;         // Número de inserção do nó na árvore
    char nomeArquivo[51];
    char permissao[3];
    int tamArquivo;
    struct arquivo *filhoDireita;
    struct arquivo *filhoEsquerda;
} Arquivo;

// Função para criar um novo nó com os dados do arquivo
Arquivo* criarNo(char* nome, char* permissao, int tamanho, int indice) {
    Arquivo* novoNo = (Arquivo*)malloc(sizeof(Arquivo));
    strcpy(novoNo->nomeArquivo, nome);
    strcpy(novoNo->permissao, permissao);
    novoNo->tamArquivo = tamanho;
    novoNo->numInsercao = indice;  // Armazena o índice de inserção
    novoNo->filhoDireita = NULL;
    novoNo->filhoEsquerda = NULL;
    return novoNo;
}

// Função para inserir um nó na árvore
Arquivo* inserirNo(Arquivo* raiz, char* nome, char* permissao, int tamanho, int indice) {
    if (raiz == NULL) {
        // Cria um novo nó com o índice de inserção atual
        return criarNo(nome, permissao, tamanho, indice);
    }

    int cmp = strcmp(nome, raiz->nomeArquivo);

    if (cmp == 0) {
        // Atualiza as informações do arquivo existente
        if (strcmp(raiz->permissao, "rw") == 0 || strcmp(permissao, "rw") == 0) {
            raiz->tamArquivo = tamanho;
            raiz->numInsercao = indice; // Atualiza a posição para o último índice de inserção
            strcpy(raiz->permissao, permissao);
        } else {
            printf("Arquivo %s é somente leitura, não pode ser atualizado.\n", nome);
        }
    } else if (cmp < 0) {
        raiz->filhoEsquerda = inserirNo(raiz->filhoEsquerda, nome, permissao, tamanho, indice);
    } else {
        raiz->filhoDireita = inserirNo(raiz->filhoDireita, nome, permissao, tamanho, indice);
    }

    return raiz;
}

// Função de percurso em ordem (EPD)
void percursoEPD(Arquivo* raiz, FILE* output) {
    if (raiz == NULL) return;

    percursoEPD(raiz->filhoEsquerda, output);
    fprintf(output, "%d %s %s %d %s\n", raiz->numInsercao, raiz->nomeArquivo, raiz->permissao, raiz->tamArquivo, raiz->tamArquivo == 1 ? "byte" : "bytes");
    percursoEPD(raiz->filhoDireita, output);
}

// Função de percurso pré-ordem (PED)
void percursoPED(Arquivo* raiz, FILE* output) {
    if (raiz == NULL) return;

    fprintf(output, "%d %s %s %d %s\n", raiz->numInsercao, raiz->nomeArquivo, raiz->permissao, raiz->tamArquivo, raiz->tamArquivo == 1 ? "byte" : "bytes");
    percursoPED(raiz->filhoEsquerda, output);
    percursoPED(raiz->filhoDireita, output);
}

// Função de percurso pós-ordem (EDP)
void percursoEDP(Arquivo* raiz, FILE* output) {
    if (raiz == NULL) return;

    percursoEDP(raiz->filhoEsquerda, output);
    percursoEDP(raiz->filhoDireita, output);
    fprintf(output, "%d %s %s %d %s\n", raiz->numInsercao, raiz->nomeArquivo, raiz->permissao, raiz->tamArquivo, raiz->tamArquivo == 1 ? "byte" : "bytes");
}

int main(int argc, char* argv[]) {
	
	FILE* input = fopen(argv[1], "r");
	FILE* output = fopen(argv[2], "w");

    Arquivo* raiz = NULL;
    int quantDocumentos;
    fscanf(input, "%d", &quantDocumentos);  // Leitura da quantidade de documentos

    for (int i = 0; i < quantDocumentos; i++) {
        char nome[51];
        char permissao[3];
        int tamanho;
        fscanf(input, "%s %s %d", nome, permissao, &tamanho);
        raiz = inserirNo(raiz, nome, permissao, tamanho, i );  // `i + 1` para ajustar o índice de inserção
    }

    // Gera as saídas em ordem, pré-ordem e pós-ordem
    fprintf(output, "EPD:\n");
    percursoEPD(raiz, output);

    fprintf(output, "PED:\n");
    percursoPED(raiz, output);

    fprintf(output, "EDP:\n");
    percursoEDP(raiz, output);

    fclose(input);
    fclose(output);
    return 0;
}
