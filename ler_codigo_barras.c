#include "funcoes_compartilhadas.h"

void leitorPBM(struct PBMImage* img, struct CodigoBarras codigo) {
    // Abre o arquivo e valida se abriu corretamente
    FILE* arquivo = fopen(codigo.nomeArquivo, "r");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    // Verifica se o arquivo é um arquivo PBM do tipo P1
    char buffer[16];
    if (!fgets(buffer, sizeof(buffer), arquivo) || strcmp(buffer, "P1\n") != 0) {
        perror("Arquivo PBM invalido");
        fclose(arquivo);
        return;
    }

    // Ignorar comentários
    char c = fgetc(arquivo);
    while (c == '#') {
        while (fgetc(arquivo) != '\n');
        c = fgetc(arquivo);
    }
    ungetc(c, arquivo);

    // Ler largura e altura
    if (fscanf(arquivo, "%d %d", &codigo.largura, &codigo.altura) != 2) {
        perror("Erro ao ler largura e altura");
        fclose(arquivo);
        return;
    }

    // Ignorar linha após dimensões
    while (fgetc(arquivo) != '\n');

    // Alocar memória para os dados da imagem
    int tamanho = codigo.largura * codigo.altura;
    img->data = (unsigned char *)malloc(tamanho);
    img->largura = codigo.largura;
    img->altura = codigo.altura;
    if (!img->data) {
        perror("Erro ao alocar memoria");
        fclose(arquivo);
        return;
    }

    // Ler dados da imagem
    int i = 0;
    while (i < tamanho) {
        int ch = fgetc(arquivo);
        // Validação para ele não tentar ler EOF ou avisar caso tenha chegado ao fim do arquivo inesperadamente
        if (ch == EOF) {
            fprintf(stderr, "Fim do arquivo inesperado na posicao %d\n", i);
            break;
        }

        // Ignorar outros caracteres (como espaços e novas linhas)
        if (ch == '0' || ch == '1') {
            img->data[i] = (unsigned char)(ch - '0');
            i++;
        }
    }
    fclose(arquivo);
}

void processarPBM(struct PBMImage* img, char* resultado) {
    int tamanho = img->largura * img->altura;
    int encontrouPrimeiro1 = 0;
    int contadorDe1s = 0;
    int i;

    resultado[0] = '\0'; // Inicializar string

    // Encontrar o primeiro 1 na imagem
    for (i = 0; i < tamanho; i++) {
        if (img->data[i] == 1) {
            encontrouPrimeiro1 = 1;
            break;
        }
    }

    // Se encontrou o primeiro 1, processar a primeira linha com numero 1
    if (encontrouPrimeiro1) {
        int linhaInicio = (i / img->largura) * img->largura;
        int linhaFim = linhaInicio + img->largura;

        for (i = linhaInicio; i < linhaFim; i++) {
            char buffer[4];
            sprintf(buffer, "%d", img->data[i]);
            strcat(resultado, buffer);
            if (i < linhaFim - 1) {
                strcat(resultado, "");
            }
        }
    } else {
        printf("Nenhum codigo de barras encontrado na imagem.\n");
    }
}

int main() {
    struct CodigoBarras codigo;
    struct PBMImage img;
    char resultado[1024];

    printf("Insira o arquivo .pbm com o codigo de barras (arquivo.pbm): ");
    fgets(codigo.nomeArquivo, sizeof(codigo.nomeArquivo), stdin);
    codigo.nomeArquivo[strcspn(codigo.nomeArquivo, "\n")] = 0;

    leitorPBM(&img, codigo);

    // Processar a imagem PBM e validar se ela não retornou vazia
    if (img.data) {
        processarPBM(&img, resultado);
        if(strlen(resultado) == 0) {
            free(img.data);
            return 1;
        } else {
            int indice = {0};
            printf("Digito verificador: %d", transBin2num8dig(&codigo, resultado));
            free(img.data);
        }
    } else {
        printf("Falha ao processar o arquivo PBM.\n");
    }
    return 0;
}