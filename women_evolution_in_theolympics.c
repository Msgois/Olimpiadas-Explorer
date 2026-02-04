#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXIMO_linha 1200
/*Vi que o bios.csv tem 145501 linhas, considerando q cada uma \
corresponde a um id de atleta, coloquei 150000 de segurança*/
#define MAXIMO_identificador 150000
/*Foi colocado o numero 53, pois é o número exato de edições que
ocorreram entre 1896 até 2022*/
#define Quantidade_Edicoes 53

typedef struct {
  int quantidademulheres, ano;

} RegistroMulheres;

/*Guarda os id das Atletas; É colocado o identificador como indice, indicando
 * como true se for verdadeiro que é Uma Mulher que participou das Olimpiadas */
bool MulheresOlimpicas[MAXIMO_identificador];

void mulheresEvolution() {
  FILE* arquivobios = fopen("bios.csv", "r");

  char linha[MAXIMO_linha];

  /*Guarda o campo da determinada coluna que está sendo
analisada*/
  char buffer[256];

  RegistroMulheres registro[Quantidade_Edicoes];

  /*Laço que percorre linha por linha*/

  while (fgets(linha, sizeof(linha), arquivobios) != NULL) {
    /*Aqui foi criado um ponteiro para aumentar a eficiência do
   sistema e Variavéis Booleanas com o objetivo durante os
   Laços, verificar se foi achado uma MULHER ATLETA OLIMPICA*/

    int coluna = 0, controlebuffer = 0, identificador = 0;
    bool isDentrodeAspas = false, isParticipante = false, isMulher = false;
    char* ponteiro;

    /*Laço que percorre caracter por caracter de uma determinada Linha*/
    for (ponteiro = linha; *ponteiro != '\0'; ponteiro++) {
      /*O if abaixo foi criado com o objetivo de maniopular o arquivo bios.csv
       * da maneira correta, pois em algumas zonas havia o uso de "" e de
       * multiplas virgulas dentro das "", não sendo possível o uso de funções
       * simples, como strstr ou strtok, sendo assim esse if serve para ignorar
       * as virgulas presentes dentro de "" */
      if (*ponteiro == '\"') {
        isDentrodeAspas = !isDentrodeAspas;
        continue;
      }
      /*O if abaixo serve pra verificar se a coluna acabou, mediante a quebra de
       * linha ou virgula*/
      if ((*ponteiro == ',' || *ponteiro == '\n' || *ponteiro == '\r') &&
          !isDentrodeAspas) {
        buffer[controlebuffer] = '\0';
        /*A estrutura condicional abaixo serve pra analisar as colunas
         * importantes*/
        if (coluna == 0 && (strcmp(buffer, "Competed in Olympic Games") == 0)) {
          isParticipante = true;
        } else if (coluna == 1 && (strcmp(buffer, "Female") == 0)) {
          isMulher = true;
        } else if (coluna == 7) {
          /*O if abaixo confirma se está realmente analisando a Uma Mulher
           * atleta ao chegar na coluna 7, se sim, ele salva o identificador que
           * está presente em tal coluna, e passa ele como indice do array
           * MulheresOlimpicas, passando o valor true para tal posição*/
          if (isParticipante && isMulher) {
            identificador = atoi(buffer);
            MulheresOlimpicas[identificador] = true;
          }
          /*Se o id já foi lido, não é necessário ler o resto da linha*/
          break;
        }
        /*Passa para a próxima Coluna*/
        coluna++;
        /*Limpa o buffer*/
        controlebuffer = 0;
        continue;
      }
      /*O if abaixo serve para guardar os caracteres no buffer, formando o campo
       * de uma determinada coluna*/
      if (controlebuffer < (int)sizeof(buffer) - 1) {
        buffer[controlebuffer++] = *ponteiro;
      }
    }
  }
}
fclose(arquivobios);
/*Fechamento do bios.csv, após o mapeamento das Atletas Olimpicas.*/
