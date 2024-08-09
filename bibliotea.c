#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
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
typedef struct {
    long long ISBN;
    char nomeAutor[51];
    char tituloDoLivro[100];
} Livro;

int BuscaBinaria(Livro* book, int n, long long ISBN, int* contadorDeComparacoes) {
    int inicio = 0;
    int fim = n - 1;
    *contadorDeComparacoes = 0;

    while (inicio <= fim) {
        (*contadorDeComparacoes)++;
        int meio = inicio + (fim - inicio) / 2;
        if (book[meio].ISBN == ISBN) 
            return meio;
        else if (book[meio].ISBN < ISBN) 
            inicio = meio + 1;
        else 
            fim = meio - 1;
    }
    return -1;
}

void LerDadosDosLivros(FILE* input, Livro* livros, int numeroDeLivros) {
    for (int i = 0; i < numeroDeLivros; i++) {
        fscanf(input, "%lld %50[^&] & %100[^\n]", &livros[i].ISBN, livros[i].nomeAutor, livros[i].tituloDoLivro);
    }
}

void LerConsultas(FILE* input, long long** consultas, int* numeroDeConsultas) {
    fscanf(input, "%d", numeroDeConsultas);
    *consultas = malloc((*numeroDeConsultas) * sizeof(long long));
    for (int i = 0; i < *numeroDeConsultas; i++) {
        if (fscanf(input, "%lld", &(*consultas)[i]) != 1) {
            printf("Erro ao ler o ISBN da consulta %d.\n", i + 1);
            exit(1);
        }
    }
}

int Busca_Interpolada(Livro* book, int n, long long ISBN, int* contador) {
    int inicio = 0;
    int fim = n - 1;
    *contador = 0;

    while (inicio <= fim && ISBN >= book[inicio].ISBN && ISBN <= book[fim].ISBN) {
        (*contador)++;
        if (inicio == fim) {
            if (book[inicio].ISBN == ISBN) 
                return inicio;
            return -1;
        }

        int meio = inicio + (int)((double)(fim - inicio) * (ISBN - book[inicio].ISBN) / (book[fim].ISBN - book[inicio].ISBN));

        if (book[meio].ISBN == ISBN) 
            return meio;
        else if (book[meio].ISBN < ISBN) 
            inicio = meio + 1;
        else 
            fim = meio - 1;
    }
    return -1;
}

int main(int argc, char* argv[]) {
    FILE* input = fopen(argv[1], "r");
    FILE* output = fopen(argv[2], "w");

    if (input == NULL) {
        printf("Erro ao abrir o arquivo de entrada.\n");
        return 1;
    }

    if (output == NULL) {
        printf("Erro ao abrir o arquivo de saída.\n");
        fclose(input);
        return 1;
    }

    int binarioVitoria = 0;
    int interpoladaVitoria = 0;
    int numDeChamadasBinaria = 0;
    int numDeChamadasInterpolada = 0;

    int numeroDeLivros;
    if (fscanf(input, "%d", &numeroDeLivros) != 1) {
        printf("Erro ao ler o número de livros.\n");
        fclose(input);
        fclose(output);
        return 1;
    }

    Livro* livros = malloc(numeroDeLivros * sizeof(Livro));
    if (livros == NULL) {
        printf("Erro ao alocar memória para livros.\n");
        fclose(input);
        fclose(output);
        return 1;
    }

    LerDadosDosLivros(input, livros, numeroDeLivros);

    int numeroDeConsultas;
    long long* consultas;
    LerConsultas(input, &consultas, &numeroDeConsultas);

    fclose(input);

    for (int i = 0; i < numeroDeConsultas; i++) {
        int contBinario, contInterpolar;
        int resulBinario = BuscaBinaria(livros, numeroDeLivros, consultas[i], &contBinario);
        int resulInterpolada = Busca_Interpolada(livros, numeroDeLivros, consultas[i], &contInterpolar);

        numDeChamadasBinaria += contBinario;
        numDeChamadasInterpolada += contInterpolar;

        if (resulBinario != -1) {
            fprintf(output, "[%lld]B=%d,I=%d:Author:%s,Title:%s\n", 
                    consultas[i], contBinario, contInterpolar, 
                    livros[resulBinario].nomeAutor, livros[resulBinario].tituloDoLivro);
        } else {
            fprintf(output, "[%lld]B=%d,I=%d:ISBN_NOT_FOUND\n", 
                    consultas[i], contBinario, contInterpolar);
        }

        if (contBinario >= contInterpolar) {
            interpoladaVitoria++;
        } else {
            binarioVitoria++;
        }
    }

    int mediaTruncadaBinaria = (numeroDeConsultas > 0) ? (numDeChamadasBinaria / numeroDeConsultas) : 0;
    int mediaTruncadaInterpolada = (numeroDeConsultas > 0) ? (numDeChamadasInterpolada / numeroDeConsultas) : 0;

    fprintf(output, "BINARY=%d:%d\n", binarioVitoria, mediaTruncadaBinaria);
    fprintf(output, "INTERPOLATION=%d:%d\n", interpoladaVitoria, mediaTruncadaInterpolada);

    free(livros);
    free(consultas);
    fclose(output);

    return 0;
}
