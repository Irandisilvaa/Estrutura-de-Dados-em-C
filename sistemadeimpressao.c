#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*A empresa de tecnologia Poxim Tech está
desenvolvendo um sistema de impressão
centralizado para otimizar a utilização das
impressoras e reduzir os custos com manutenção e
reposição de suprimentos
▶ Todos os documentos enviados para impressão são
organizados por ordem de chegada, sendo
despachados para uma impressora que estiver ociosa
▶ Os nomes dos arquivos e das impressoras possuem até
50 caracteres, sendo limitados a letras e números
▶ A quantidade de páginas do documento determina
quanto tempo será utilizado na impressora alocada,
assumindo que todas as impressoras possuem a
mesma velocidade de impressão
▶ Após cada impressão ser concluída, as folhas
impressas de todas as impressoras são
automaticamente recolhidas e empilhadas para
serem entregues;
Formato do arquivo de entrada
▶ [#n]
▶ [Impressora 1]
▶ · · ·
▶ [Impressora n]
▶ [#m]
▶ [Documento 1] [#Paginas ´ 1]
▶ · · ·
▶ [Documento m] [#Paginas m ´ ]
*/
// Estrutura para representar um documento
typedef struct no {
    char nomeArquivo[51];
    int paginas;
    struct no *proximo;
} No;

// Estrutura para representar uma impressora
typedef struct impressora {
    char nomeImpressora[51];
    int status; // 0 para ociosa, 1 para ocupada
    struct impressora *proxima;
    No *pilhaDeDocumentos;     // Pilha de documentos impressos
    int tempoRestante;         // Tempo restante para finalizar a impressão atual
    int totalPaginasImpressas; // Total de páginas impressas
} Impressora;

// Estrutura para representar a fila global de documentos
typedef struct {
    No *inicio;
    No *fim;
} FilaDocumentos;

FilaDocumentos filaGlobal = {NULL, NULL};

// Função para adicionar uma impressora à fila de impressoras
void adicionarImpressoras(Impressora **inicio, Impressora **fim, FILE *input, int quantImpressoras) {
    for (int i = 0; i < quantImpressoras; i++) {
        Impressora *novaImpressora = (Impressora *)malloc(sizeof(Impressora));
        if (novaImpressora == NULL) {
            perror("Erro ao alocar memória para impressora");
            exit(1);
        }
        fscanf(input, "%s", novaImpressora->nomeImpressora);
        novaImpressora->pilhaDeDocumentos = NULL;
        novaImpressora->tempoRestante = 0;
        novaImpressora->totalPaginasImpressas = 0;
        novaImpressora->status = 0; // Inicialmente ociosa
        novaImpressora->proxima = NULL;

        if (*inicio == NULL) {
            *inicio = novaImpressora;
            *fim = novaImpressora;
        } else {
            (*fim)->proxima = novaImpressora;
            *fim = novaImpressora;
        }
    }
}

// Função para adicionar um documento à fila global
void adicionarDocumentoFilaGlobal(FilaDocumentos *fila, const char *nomeArquivo, int paginas) {
    No *novoDocumento = (No *)malloc(sizeof(No));
    if (novoDocumento == NULL) {
        perror("Erro ao alocar memória para documento");
        exit(1);
    }

    strcpy(novoDocumento->nomeArquivo, nomeArquivo);
    novoDocumento->paginas = paginas;
    novoDocumento->proximo = NULL;

    if (fila->fim == NULL) {
        fila->inicio = novoDocumento;
        fila->fim = novoDocumento;
    } else {
        fila->fim->proximo = novoDocumento;
        fila->fim = novoDocumento;
    }
}

// Função para empilhar um documento na pilha de uma impressora e atualizar o histórico
void empilharDocumento(Impressora *imp, No *doc, FILE *output, FilaDocumentos *filaGlobal) {
    doc->proximo = imp->pilhaDeDocumentos;
    imp->pilhaDeDocumentos = doc;
    imp->totalPaginasImpressas += doc->paginas;
    imp->status = 1;                    // Define o status como ocupado
    imp->tempoRestante += doc->paginas; // Atualiza o tempo restante de impressão
    adicionarDocumentoFilaGlobal(filaGlobal, doc->nomeArquivo, doc->paginas);
  
    // Atualizar histórico no arquivo de saída
    fprintf(output, "[%s] ", imp->nomeImpressora);
    No *aux = imp->pilhaDeDocumentos;
    int primeiro = 1;
    while (aux != NULL) {
        if (!primeiro) {
            fprintf(output, ", ");
        }
        fprintf(output, "%s-%dp", aux->nomeArquivo, aux->paginas);
        primeiro = 0;
        aux = aux->proximo;
    }
    fprintf(output, "\n");
}

// Função para encontrar a impressora ociosa
Impressora *encontrarImpressoraOciosa(Impressora *inicio) {
    Impressora *imp = inicio;
    while (imp != NULL) {
        if (imp->status == 0) {
            return imp;
        }
        imp = imp->proxima;
    }
    return NULL;
}

// Função para encontrar a impressora com o menor tempo restante
Impressora *encontrarImpressoraComMenorTempoRestante(Impressora *inicio) {
    Impressora *imp = inicio;
    Impressora *impComMenorTempo = NULL;
    int menorTempoRestante = -1;

    while (imp != NULL) {
        if (imp->tempoRestante > 0 &&
            (menorTempoRestante == -1 || imp->tempoRestante < menorTempoRestante)) {
            menorTempoRestante = imp->tempoRestante;
            impComMenorTempo = imp;
        }
        imp = imp->proxima;
    }

    return impComMenorTempo;
}

// Função para adicionar documentos à fila de impressoras e à fila global
void adicionarDocumentos(Impressora **inicio, FILE *input, int quantDocumentos, FILE *output,FilaDocumentos *filaGlobal) {
    for (int i = 0; i < quantDocumentos; i++) {
        No *novoDocumento = (No *)malloc(sizeof(No));
        if (novoDocumento == NULL) {
            perror("Erro ao alocar memória para documento");
            exit(1);
        }

        fscanf(input, "%s %d", novoDocumento->nomeArquivo, &novoDocumento->paginas);
        novoDocumento->proximo = NULL;
        Impressora *imp = encontrarImpressoraOciosa(*inicio);

        if (imp == NULL) {
            imp = encontrarImpressoraComMenorTempoRestante(*inicio);
        }

        if (imp != NULL) {
            empilharDocumento(imp, novoDocumento, output, filaGlobal);
        }
        
    }
}

// Função para desempilhar e imprimir documentos da fila global
void desempilharEImprimirDocumentos(FilaDocumentos *fila, FILE *output, Impressora *inicio) {
    No *documentoAtual = fila->inicio;
    No *documentoAnterior = NULL;

    // Calcula o total de páginas impressas a partir das impressoras
    int totalPaginas = 0;
    Impressora *impAux = inicio;
    while (impAux != NULL) {
        totalPaginas += impAux->totalPaginasImpressas;
        impAux = impAux->proxima;
    }
    fprintf(output, "\n%dp\n\n", totalPaginas);

    // Imprimir os documentos da fila global na ordem inversa
    No *pilhaInversa = NULL;
    while (documentoAtual != NULL) {
        No *novoDoc = (No *)malloc(sizeof(No));
        if (novoDoc == NULL) {
            perror("Erro ao alocar memória para documento na pilha inversa");
            exit(1);
        }
        strcpy(novoDoc->nomeArquivo, documentoAtual->nomeArquivo);
        novoDoc->paginas = documentoAtual->paginas;
        novoDoc->proximo = pilhaInversa;
        pilhaInversa = novoDoc;

        // Avançar para o próximo documento
        documentoAnterior = documentoAtual;
        documentoAtual = documentoAtual->proximo;

        // Liberar a memória do documento atual
        free(documentoAnterior);
    }

    // Imprimir documentos na ordem inversa
    while (pilhaInversa != NULL) {
        fprintf(output, "%s-%dp\n", pilhaInversa->nomeArquivo, pilhaInversa->paginas);

        // Avançar para o próximo documento
        No *temp = pilhaInversa;
        pilhaInversa = pilhaInversa->proximo;
        free(temp);
    }

    // Atualizar a fila global
    fila->inicio = NULL;
    fila->fim = NULL;
}

// Função principal
int main() {
  
  
  
  FILE* input = fopen("entrada.txt", "r");
  if (input == NULL) {
        perror("Erro ao abrir o arquivo de entrada");
        return 1;
    }
  
  FILE* output = fopen("saida.txt", "w");
   if (output == NULL) {
        perror("Erro ao abrir o arquivo de saída");
        fclose(input);
        return 1;
    }

    Impressora *inicioFila = NULL;
    Impressora *fimFila = NULL;
    int quantImpressoras = 0;
    int quantDocumentos = 0;

    // Inicializar a fila global
    FilaDocumentos filaGlobal = {NULL, NULL};

    fscanf(input, "%d", &quantImpressoras);
    adicionarImpressoras(&inicioFila, &fimFila, input, quantImpressoras);

    fscanf(input, "%d", &quantDocumentos);
    adicionarDocumentos(&inicioFila, input, quantDocumentos, output, &filaGlobal);

    // Desempilha e imprime os documentos na fila global
    desempilharEImprimirDocumentos(&filaGlobal, output, inicioFila);

    fclose(input);
    fclose(output);

    // Liberação de memória
    Impressora *imp = inicioFila;
    while (imp != NULL) {
        No *doc = imp->pilhaDeDocumentos;
        while (doc != NULL) {
            No *tempDoc = doc;
            doc = doc->proximo;
            free(tempDoc);
        }
        Impressora *tempImp = imp;
        imp = imp->proxima;
        free(tempImp);
    }

    return 0;
}
