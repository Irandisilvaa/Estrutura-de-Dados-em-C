
/* A empresa de tecnologia Poxim Tech está realizando
um estudo comparativo entre a busca binária e
interpolada para um sistema de biblioteca, para
determinar qual das abordagens é mais eficiente
▶ Os livros são identificados unicamente pelo
International Standard Book Number (ISBN) que é
composto de 13 dígitos numéricos
▶ Para realização de consulta dos livros é utilizado o
ISBN, contabilizando o número total de chamadas
realizadas para realização da busca binária e
interpolada e retornando o nome do autor (até 50
caracteres) e do título do livro (até 100 caracteres)
▶ A busca interpolada é feita da função de heurística
h(i, j) = 
i +
 Formato do arquivo de entrada
▶ [#Livros]
▶ [#ISBN1] [Autor1] & [Titulo1]
▶ · · ·
▶ [#ISBNn] [Autorn] & [Titulon]
▶ [#Consultas]
▶ [#ISBN1]
▶ · · ·
▶ [#ISBNm]*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_ISBN 14
#define MAX_NOME_AUTOR 51
#define MAX_NOME_LIVRO 101

typedef struct livros {
    char ISBN[MAX_ISBN];
    char nomeAutor[MAX_NOME_AUTOR];
    char nomeLivro[MAX_NOME_LIVRO];
} Livros;

int buscaBinaria(int* contagem, char pesquisa[MAX_ISBN], Livros* livros, int inicio, int fim) {
    (*contagem)++;
    if (fim < inicio) {
        return -1;  // Não encontrado
    }
    
    int meio = (inicio + fim) / 2;
    int comparacao = strcmp(livros[meio].ISBN, pesquisa);

    if (comparacao == 0) {
        return meio;  // Item encontrado
    } else if (comparacao > 0) {
        return buscaBinaria(contagem, pesquisa, livros, inicio, meio - 1);
    } else {
        return buscaBinaria(contagem, pesquisa, livros, meio + 1, fim);
    }
}


int  buscaInterpolada(int* count, char pesquisa[14], Livros* livros, int inicio, int fim) {
    while (inicio <= fim) {
        (*count)++;

        long long isbn_i = strtoll(livros[inicio].ISBN, NULL, 10);
        long long isbn_j = strtoll(livros[fim].ISBN, NULL, 10);

        // Evitar divisão por zero
        if (isbn_j == isbn_i) {
            return strcmp(livros[inicio].ISBN, pesquisa) == 0 ? inicio : -1;
        }

        int p = inicio + ((isbn_j - isbn_i) % (fim - inicio + 1));

        // Verificar se o índice calculado está dentro do intervalo
        if (p < inicio || p > fim) return -1;

        int cmp = strcmp(livros[p].ISBN, pesquisa);
        if (cmp == 0) return p;
        if (cmp < 0) inicio = p + 1;
        else fim = p - 1;
    }
    return -1;
}
void processarConsultas(int quantidadeConsultas, char pesquisa[][MAX_ISBN], Livros* livros, int quantidadeLivros, FILE* saida) {
    int contagemBinaria, contagemInterpolada;
    int vitoriasBinaria = 0, vitoriasInterpolada = 0;
    int totalBinaria = 0, totalInterpolada = 0;

    for (int i = 0; i < quantidadeConsultas; i++) {
        contagemBinaria = 0;
        contagemInterpolada = 0;

        int resultadoBinario = buscaBinaria(&contagemBinaria, pesquisa[i], livros, 0, quantidadeLivros - 1);
        int resultadoInterpolado = buscaInterpolada(&contagemInterpolada, pesquisa[i], livros, 0, quantidadeLivros - 1);


        fprintf(saida, "[%s]B=%d,I=%d", pesquisa[i], contagemBinaria, contagemInterpolada);

        totalBinaria += contagemBinaria;
        totalInterpolada += contagemInterpolada;


        if (contagemInterpolada <= contagemBinaria) {
            vitoriasInterpolada++;
        } else {
            vitoriasBinaria++;
        }


        if (resultadoBinario != -1) {
            fprintf(saida, ":Author:%s,Title:%s\n", livros[resultadoBinario].nomeAutor, livros[resultadoBinario].nomeLivro);
        } else if (resultadoInterpolado != -1) {
            fprintf(saida, ":Author:%s,Title:%s\n", livros[resultadoInterpolado].nomeAutor, livros[resultadoInterpolado].nomeLivro);
        } else {
            fprintf(saida, ":ISBN_NOT_FOUND\n");
        }
    }

    // Calcula e imprime as médias de desempenho
    fprintf(saida, "BINARY=%d:%d\n", vitoriasBinaria, totalBinaria / quantidadeConsultas);
    fprintf(saida, "INTERPOLATION=%d:%d\n", vitoriasInterpolada, totalInterpolada / quantidadeConsultas);
}

int main(int argc, char* argv[]) {
    FILE* entrada = fopen(argv[1], "r");
    FILE* saida = fopen(argv[2], "w");

    if (!entrada || !saida) {
        fprintf(stderr, "Erro ao abrir os arquivos.\n");
        return 1;
    }

    int quantidadeLivros;
    fscanf(entrada, "%d", &quantidadeLivros);

    Livros* livros = (Livros*)malloc(quantidadeLivros * sizeof(Livros));
    if (!livros) {
        fprintf(stderr, "Erro de alocação de memória.\n");
        fclose(entrada);
        fclose(saida);
        return 1;
    }

    for (int i = 0; i < quantidadeLivros; i++) {
        fscanf(entrada, "%s %50[^&]&%100[^\n]", livros[i].ISBN, livros[i].nomeAutor, livros[i].nomeLivro);
    }

    int quantidadeConsultas;
    fscanf(entrada, "%d", &quantidadeConsultas);

    char pesquisa[quantidadeConsultas][MAX_ISBN];
    for (int i = 0; i < quantidadeConsultas; i++) {
        fscanf(entrada, "%s", pesquisa[i]);
    }

    processarConsultas(quantidadeConsultas, pesquisa, livros, quantidadeLivros, saida);

    fclose(entrada);
    fclose(saida);
    free(livros);

    return 0;
}
