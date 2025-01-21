#ifndef FUNCOES_COMPARTILHADAS_H
#define FUNCOES_COMPARTILHADAS_H
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// Struct para armazenar os dados do código de barras
struct CodigoBarras {
    char num8dig[9];
    int espaco_lateral;
    int area;
    int altura;
    char nomeArquivo[50];
    char binCodBarras[1000];
    int largura;
    char l_code[100];
    char r_code[100];
} defaultCodigoBarras = {
    .num8dig = {0},
    .espaco_lateral = 4,
    .area = 3,
    .altura = 50,
    .nomeArquivo = "codigo de barras.pbm",
    .binCodBarras = {0},
    .l_code = {0},
    .r_code = {0}
};

struct PBMImage {
    unsigned char *data;
    int largura;
    int altura;
};

// Array de códigos de barras para cada dígito
const char *codigos[10][2] = {
    {"0001101", "1110010"}, // Dígito 0
    {"0011001", "1100110"}, // Dígito 1
    {"0010011", "1101100"}, // Dígito 2
    {"0111101", "1000010"}, // Dígito 3
    {"0100011", "1011100"}, // Dígito 4
    {"0110001", "1001110"}, // Dígito 5
    {"0101111", "1010000"}, // Dígito 6
    {"0111011", "1000100"}, // Dígito 7
    {"0110111", "1001000"}, // Dígito 8
    {"0001011", "1110100"}  // Dígito 9
};

// Função para verificar se um arquivo existe
int verificaArquivoExistente(const char *nomeArquivo) {
    FILE *file = fopen(nomeArquivo, "r");
    if (file) {
        fclose(file);
        return 1; // Arquivo existe
    }
    return 0; // Arquivo não existe
}

// Função para calcular o número com o dígito verificador
int calcDigitoVer(char *num8dig) {
    int soma = 0;
    int peso, ajuste;

    // Soma dos numeros com pesos para gerar verificador
    int length = strlen(num8dig);
    for (int i = 0; i < length-1; i++) {
        peso = (i % 2 == 0) ? 3 : 1;
        int digito = num8dig[i] - '0';
        soma += digito * peso;
    }

    ajuste = (soma % 10 == 0) ? 0 : 10 - (soma % 10);
    return ajuste;
}

// Função para transformar um número de 8 dígitos decimais em binário
void transnum8dig2Bin(struct CodigoBarras *codigo) {
    codigo->l_code[0] = '\0'; 
    codigo->r_code[0] = '\0';

    // Transforma os 4 primeiros digitos em binário
    for (int i = 0; i < 4; i++) {
        int digito = codigo->num8dig[i] - '0';
        strcat(codigo->l_code, codigos[digito][0]);
    }

    // Transforma os 4 últimos digitos em binário
    for (int i = 4; i < 8; i++) {
        int digito = codigo->num8dig[i] - '0';
        strcat(codigo->r_code, codigos[digito][1]);
    }
}

// Função para limpar zeros à esquerda e à direita de uma string
void limpaZeros(char *str) {
    char *start = str;
    char *end = str + strlen(str) - 1;

    // Remove zeros do início
    while (*start == '0') {
        start++;
    }

    // Remove zeros do final
    while (end > start && *end == '0') {
        end--;
    }

    // Copia a string sem os zeros para o início do buffer original
    memmove(str, start, end - start + 1);
    str[end - start + 1] = '\0'; // Garante terminação nula
}

// Função para transformar um número binário de volta para 8 dígitos decimais
int transBin2num8dig(struct CodigoBarras* codigo, char* resultado) {
    
    // Uso da função para limpar os zeros da string
    limpaZeros(resultado);

    // Conta quantas repetições há na matriz
    int contador = 0;
    for (size_t i = 0; i < strlen(resultado); i++) {
        if (resultado[i] == '1') {
            contador++;
        } else {
            break;
        }	
    }

    // Preenche binCodBarras com o valor do resultado pulando a quantidade de repetições
    int j = 0;
    for (size_t i = 0; i < strlen(resultado);) {
        codigo->binCodBarras[j] = resultado[i];
        i = i + contador;
        j++;
    }

    // Pega o binário do código de barras e coloca no char separador
    char separador[8][10] = {{0}};
    codigo->num8dig[0] = '\0';
    int k = 3;

    for (int i = 0; i < 8; i++) {
        int memoria = k + 7;
        separador[i][0] = '\0';
        while (k < memoria && k < (int)sizeof(codigo->binCodBarras)) {
            char temp[2] = {codigo->binCodBarras[k], '\0'};
            strncat(separador[i], temp, 1);
            k++;
        }
        if (i == 3) {
            k += 5;
        }
    }

    // Compara os valores da matriz com os valores da matriz de códigos
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 10; j++) {
            if (i < 4) {
                if (strcmp(separador[i], codigos[j][0]) == 0) {
                    char temp[2];
                    sprintf(temp, "%d", j);
                    strcat(codigo->num8dig, temp);
                    break;
                }
            } else {
                if (strcmp(separador[i], codigos[j][1]) == 0) {
                    char temp[2];
                    sprintf(temp, "%d", j);
                    strcat(codigo->num8dig, temp);
                    break;
                }
            }
        }
    }

    return(calcDigitoVer(codigo->num8dig));
}

// Função para gerar o código de barras em binário
void gerarCodigoBarras(char *inicioefim, char *meio, struct CodigoBarras *codigo) {
    char codigodeBarras[300] = {0};

    // codigodeBarras = inicioefim + l_code + meio + r_code + inicioefim
    strcat(codigodeBarras, inicioefim);
    strcat(codigodeBarras, codigo->l_code);
    strcat(codigodeBarras, meio);
    strcat(codigodeBarras, codigo->r_code);
    strcat(codigodeBarras, inicioefim);

    // copia o codigo da variavel para a struct
    strncpy(codigo->binCodBarras, codigodeBarras, sizeof(codigo->binCodBarras)-1);
    codigo->binCodBarras[sizeof(codigo->binCodBarras) - 1] = '\0'; // Garante terminação nula
}

#endif