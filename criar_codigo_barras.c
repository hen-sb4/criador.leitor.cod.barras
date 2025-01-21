#include "funcoes_compartilhadas.h"

// Função auxiliar para imprimir uma linha de caracteres repetidos
void imprimirLinhaRepetida(FILE* arquivo, char caractere, int repeticoes) {
    for (int i = 0; i < repeticoes; i++) {
        fputc(caractere, arquivo);
    }
}

// Função para salvar o código de barras em um arquivo PBM
void salvarCodigoBarrasPBM(struct CodigoBarras* codigo) {
    FILE* arquivo = fopen(codigo->nomeArquivo, "wb");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo!\n");
        return;
    }

    int areaLargura = (67 * codigo->area) + (codigo->espaco_lateral * 2);
    int alturaTotal = (codigo->espaco_lateral * 2) + codigo->altura;

    // Printa o cabeçalho do arquivo PBM
    fprintf(arquivo, "P1\n%d %d\n", areaLargura, alturaTotal);

    // Printa a barra superior de zeros
    for (int i = 0; i < codigo->espaco_lateral; i++) {
        imprimirLinhaRepetida(arquivo, '0', areaLargura);
        fprintf(arquivo, "\n");
    }

    // Printa o código de barras
    for (int i = 0; i < codigo->altura; i++) {
        imprimirLinhaRepetida(arquivo, '0', codigo->espaco_lateral);

        for (size_t j = 0; j < strlen(codigo->binCodBarras); j++) {
            for (int k = 0; k < codigo->area; k++) {
                fputc(codigo->binCodBarras[j], arquivo);
            }
        }
        // Printa os zeros do espaçamento lateral direito
        imprimirLinhaRepetida(arquivo, '0', codigo->espaco_lateral);
        fprintf(arquivo, "\n");
    }

    // Printa a barra inferior de zeros
    for (int i = 0; i < codigo->espaco_lateral; i++) {
        imprimirLinhaRepetida(arquivo, '0', areaLargura);
        fprintf(arquivo, "\n");
    }

    fclose(arquivo);
    printf("Arquivo %s salvo com sucesso!\n", codigo->nomeArquivo);
}

int main() {
    struct CodigoBarras codigo;
    char input[200];
    char inicio_e_fim[4] = "101";
    char meio[6] = "01010";
    char binCodBarras[300];
    char simounao[3];

    printf("Digite o numero de 8 digitos, o espacamento lateral, pixels por area do codigo, altura do codigo e nome do arquivo:\n");
    printf("Uso: (numero de 7 digitos + codigo verificador) <espaco lateral> <area> <altura> <nome do arquivo.pbm>\n");
    fgets(input, sizeof(input), stdin);
    int num_args = sscanf(input, "%8s %d %d %d %49s", 
    codigo.num8dig, 
    &codigo.espaco_lateral, 
    &codigo.area, 
    &codigo.altura, 
    codigo.nomeArquivo);

    // Validar quantidade de argumentos
    if (num_args < 2) codigo.espaco_lateral = defaultCodigoBarras.espaco_lateral;
    if (num_args < 3) codigo.area = defaultCodigoBarras.area;
    if (num_args < 4) codigo.altura = defaultCodigoBarras.altura;
    if (num_args < 5) strcpy(codigo.nomeArquivo, defaultCodigoBarras.nomeArquivo);

    // Validar se num8dig é um número
    if (strlen(codigo.num8dig) != 8) {
        printf("Erro: A entrada deve conter 8 digitos.\n");
        return 1;
    }

    // Validar se num8dig contém apenas números
    for (int i = 0; i < 8; i++) {
        if (!isdigit(codigo.num8dig[i])) {
            printf("Erro: A entrada deve conter apenas numeros.\n");
            return 1;
        }
    }

    // Validar se digito verificador é verdadeiro
    int ajuste = calcDigitoVer(codigo.num8dig);
    int ultimo_digito = codigo.num8dig[7] - '0';
    if (ajuste != ultimo_digito) {
        printf("Erro: O digito verificador esta incorreto.");
        return 1;
    }

    // Verificar se o arquivo já existe
    if (verificaArquivoExistente(codigo.nomeArquivo)) {
        printf("O arquivo %s ja existe, voce deseja sobreescreve-lo? (sim, nao)\n", codigo.nomeArquivo);
        scanf("%s", simounao);

        if (strcmp(simounao, "nao") == 0) {
            printf("Operacao cancelada.\n");
            return 1;
        } else {
            printf("Sobreescrevendo arquivo.\n");
        }
    }

    transnum8dig2Bin(&codigo);
    gerarCodigoBarras(inicio_e_fim, meio, &codigo);
    salvarCodigoBarrasPBM(&codigo);

    return 0;
}