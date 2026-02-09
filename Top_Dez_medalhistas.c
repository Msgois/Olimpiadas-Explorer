#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#define MAX_LINHA 1024
#define MAX_ATLETAS 10000

typedef struct {
    char nome[100];
    int medalhas;
} Atleta;

// extrair_campo
// Objetivo: Extrai um campo específico de uma linha CSV, levando em conta que campos podem estar entre aspas e conter vírgulas internas.
// Funcionamento geral:
// Percorre a linha caractere por caractere, identifica vírgulas que
// realmente separam campos (desconsiderando vírgulas dentro de aspas)
// e copia o conteúdo do campo desejado para 'destino'.
void extrair_campo(char *linha, int indice_campo, char *destino) {
    int campo_atual = 0;  // Indica em qual campo estamos enquanto percorremos a linha
    int dentro_de_aspas = 0; // Flag que diz se estamos dentro de aspas (0 = não, 1 = sim)
    int j = 0; // Posição de escrita em 'destino'

    // Percorre cada caractere da linha até encontrar o fim da string
    for (int i = 0; linha[i] != '\0'; i++) {
        if (linha[i] == '"') {
            dentro_de_aspas = !dentro_de_aspas;

        }
        // Se encontrar vírgula e não estiver dentro de aspas entao ele achou separador real de campo
        else if (linha[i] == ',' && !dentro_de_aspas) {
            // Se já está no campo desejado, parar (campo terminou)
            if (campo_atual == indice_campo) {
                break;
            }
            // Senão, avança para o próximo campo
            campo_atual++;
            // Reinicia índice de escrita do destino
            j = 0;
        }
        // Se estamos no campo que queremos, copiar o caractere atual para 'destino'
        else if (campo_atual == indice_campo) {
            destino[j++] = linha[i];
        }
    }
    // Finaliza a string copiada com '\0'
    destino[j] = '\0';
}

int Top_Des_medalhistas(){

 // Tenta ajustar o idioma para o português; se falhar, usa en_US
    if (!setlocale(LC_ALL, "Portuguese")) {
        setlocale(LC_ALL, "en_US.UTF-8");
    }

    // Abre o arquivo CSV contendo os resultados olímpicos
    FILE *arquivo = fopen("results.csv", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return 1;
    }

    // Vetor que armazenará até 10.000 atletas
    Atleta lista[MAX_ATLETAS];
    int total_atletas = 0;              // Contador de atletas cadastrados
    char buffer_linha[MAX_LINHA];       // Armazena cada linha lida do CSV


    // Lê cada linha do CSV até o final do arquivo
    while (fgets(buffer_linha, MAX_LINHA, arquivo)) {

        char nome_atual[100];
        char medalha_atual[20];
        char evento_atual[150];

        // Extrai campos específicos da linha CSV
        // Índices baseados na estrutura do arquivo:
        extrair_campo(buffer_linha, 5, nome_atual); //  5 Nome do atleta
        extrair_campo(buffer_linha, 4, medalha_atual); //  4  Medalha
        extrair_campo(buffer_linha, 1, evento_atual); //  1 → Evento

        // Filtro aplicado a cada linha do CSV:
        // medalha_atual precisa ter conteúdo válido (não vazia)
        // medalha_atual não pode ser "NA" (indicador de ausência de medalha)
        // evento_atual deve conter "Women", garantindo que seja um evento feminino

        if (strlen(medalha_atual) > 1 &&
            strcmp(medalha_atual, "NA") != 0 &&
            strstr(evento_atual, "Women") != NULL) {

            int encontrado = 0;

    // Procura se o atleta já está na lista
            for (int i = 0; i < total_atletas; i++) {
                if (strcmp(lista[i].nome, nome_atual) == 0) {
                    // Se já existe, apenas incrementa a contagem de medalhas
                    lista[i].medalhas++;
                    encontrado = 1;
                    break;
                }
            }

            if (!encontrado && total_atletas < MAX_ATLETAS) {   // Se o atleta ainda não está na lista e ainda há espaço disponível
                strcpy(lista[total_atletas].nome, nome_atual);  // Copia o nome do atleta para a nova posição da lista
                lista[total_atletas].medalhas = 1;              // Inicializa a contagem de medalhas desse atleta com 1
                total_atletas++;                                // Avança o total de atletas cadastrados
            }


        }
    }

    // Ordena os atletas por número de medalhas (ordem decrescente)
    for (int i = 0; i < total_atletas - 1; i++) {
        for (int j = 0; j < total_atletas - i - 1; j++) {
            if (lista[j].medalhas < lista[j+1].medalhas) {
                // Troca de posição (Bubble Sort)
                Atleta temp = lista[j];
                lista[j] = lista[j+1];
                lista[j+1] = temp;
            }
        }
    }

    // Exibe as 10 atletas com mais medalhas no dataset
    printf("\n--- TOP 10 MEDALHISTAS OLIMPICAS (FEMININO) ---\n");
    for (int i = 0; i < 10 && i < total_atletas; i++) {
        printf("%d. %-30s | %d medalhas\n",
               i + 1,
               lista[i].nome,
               lista[i].medalhas);
    }

    // Fecha o arquivo CSV
    fclose(arquivo);
    return 0;
}
