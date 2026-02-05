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

void extrair_campo(char *linha, int indice_campo, char *destino) {
    int campo_atual = 0;
    int dentro_de_aspas = 0;
    int j = 0;

    for (int i = 0; linha[i] != '\0'; i++) {
        if (linha[i] == '"') {
            dentro_de_aspas = !dentro_de_aspas;
        } else if (linha[i] == ',' && !dentro_de_aspas) {
            if (campo_atual == indice_campo) break;
            campo_atual++;
            j = 0;
        } else if (campo_atual == indice_campo) {
            destino[j++] = linha[i];
        }
    }
    destino[j] = '\0';
}

int Top_Des_medalhistas(){

}
