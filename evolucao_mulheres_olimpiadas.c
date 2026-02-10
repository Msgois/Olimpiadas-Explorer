#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXIMO_linha 4096
/*Vi que o bios.csv tem 145501 linhas, considerando q cada uma \
corresponde a um id de atleta, coloquei 150000 de segurança*/
#define MAXIMO_identificador 150000
/*Foi colocado o numero 36, pois é o número exato dos anos que
que houveram edições,isso entre 1896 até 2022*/
#define Quantidade_anosolimpiadas 36

/*Guarda os id das Atletas; É colocado o identificador como indice, indicando
 * como true se for verdadeiro(é Uma Mulher que participou das Olimpiadas) */
bool MulheresOlimpicas[MAXIMO_identificador];
char linha[MAXIMO_linha];
int i = 0, j = 0;
int anoscontabilizados[MAXIMO_identificador];
/*Array que contém os anos em que ocorreram as edições entre 1896 até 2022*/
int edicoes[Quantidade_anosolimpiadas] = {
    1896, 1900, 1904, 1908, 1912, 1920, 1924, 1928, 1932, 1936, 1948, 1952,
    1956, 1960, 1964, 1968, 1972, 1976, 1980, 1984, 1988, 1992, 1994, 1996,
    1998, 2000, 2002, 2004, 2006, 2008, 2010, 2014, 2016, 2018, 2020, 2022};
/*Array que irá conter a quantidade de mulheres por ano*/
int quantidademulheres[Quantidade_anosolimpiadas] = {0};
/*Guarda o campo da determinada coluna que está sendo
analisada*/
char buffer[256];

int evolucao_mulheres_olimpiadas() {
  FILE* arquivobios = fopen("bios.csv", "r");

  // O if abaixo serve para verificar se o arquivo realmente foi aberto/
  if (arquivobios == NULL) {
    printf("ERRO NA LEITURA DO ARQUIVO!");
    return 1;
  }

  /*Pula o cabeçalho do bios.csv*/
  fgets(linha, sizeof(linha), arquivobios);

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
  /*Fechamento do bios.csv, após o mapeamento das Atletas Olimpicas.*/
  fclose(arquivobios);

  FILE* arquivoresults = fopen("results.csv", "r");

  /*O if abaixo serve para verificar se o arquivo realmente foi aberto*/
  if (arquivoresults == NULL) {
    printf("ERRO NA LEITURA DO ARQUIVO!");
    return 1;
  }

  /*Pula o cabeçalho do results.csv*/
  fgets(linha, sizeof(linha), arquivoresults);

  /*Laço que percorre linha por linha*/
  while (fgets(linha, sizeof(linha), arquivoresults) != NULL) {
    int coluna = 0, controlebuffer = 0, identificador = 0, anovisualizado = 0;
    bool isDentrodeAspas = false;
    char* ponteiro;
    /*O tamanho da String é 5 pois o ano é feito de 4 char, mas precisamos
     * colocar o \0 no final para indicar o fim da string*/
    char copiaAnovisualizado[5];

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
        /*o if abaixo tem o objetivo de verificar se a coluna é a 0, se sim, os
         * 4 primeiros caracteres da coluna 0, ou seja, o ano, sejam copiados
         * para dentro de uma nova string, após isso é adicionado \0 para
         * indicar o fim da string, tal string é convertida em numero inteiro e
         * armazenada na variavél ano visualizado, que servirá futuramente para
         * verificar se uma determinada atleta já foi devidamente somada a 1 ano
         * em qual ela participou*/
        if (coluna == 0) {
          strncpy(copiaAnovisualizado, buffer, 4);
          copiaAnovisualizado[4] = '\0';
          anovisualizado = atoi(copiaAnovisualizado);
          /*Guarda o id do/a Atleta*/
        } else if (coluna == 6) {
          identificador = atoi(buffer);
          break;
        }
        coluna++;
        controlebuffer = 0;
        continue;
      }
      if (controlebuffer < (int)sizeof(buffer) - 1) {
        buffer[controlebuffer++] = *ponteiro;
      }
    }
    /*A estrutura abaixo faz o seguinte:
    -Verifica se o id guardado é de uma mulher, por meio do Array de booleanos
    MulheresOlimpicas
    -Ao confirmar a situação anterior,é verificado se o
    anovisualizado é diferente do ultimo ano contabilzado por esse id, para que
    não haja repetições
    -Ao confirmar a situação anterior, o laço de repetição juntamente com a
    estrutura condicional presente no seu interior, faz o match entre o
    anovisualizado e sua respectiva posição no array edicoes, soma-se +1 na
    quantidade de mulheres e por fim, de acordo com a respectiva posição é
    atualizado o array de anoscontabilizados do determinado id*/
    if (MulheresOlimpicas[identificador]) {
      if (anovisualizado != anoscontabilizados[identificador]) {
        for (i = 0; i < Quantidade_anosolimpiadas; i++) {
          if (anovisualizado == edicoes[i]) {
            quantidademulheres[i]++;
            anoscontabilizados[identificador] = anovisualizado;
            break;
          }
        }
      }
    }
  }
  fclose(arquivoresults);
  
  //LEITURA DA QUANTIDADE DE MULHERES POR ANO
  for (i = 0; i < Quantidade_anosolimpiadas; i++) {
    printf("%d: %d\n", edicoes[i], quantidademulheres[i]);
  }
//CRIAÇÃO E PREENCHIMENTO DE UM ARQUIVO TXT 
  FILE *arquivodados = fopen("dados.txt", "w");
    if (arquivodados) {
        for (i = 0; i < Quantidade_anosolimpiadas; i++) {
            fprintf(arquivodados, "%d %d\n", edicoes[i], quantidademulheres[i]);
        }
        fclose(arquivodados);
    }

    // CRIAÇÃO DO ARQUIVO SCRIPT DO GNUPLOT
    FILE *f_script = fopen("script_evolucao_mulheres.gp", "w");
    if (f_script) {
      
        fprintf(f_script, "set title \"Evolucao da Participacao Feminina nas Olimpiadas\"\n");
        fprintf(f_script, "set xlabel \"Ano\"\n");
        fprintf(f_script, "set ylabel \"Quantidade\"\n");
        fprintf(f_script, "set grid y\n");
        
        //CONFIG PARA UM GRÁFICO DE BARRAS
        fprintf(f_script, "set style fill solid 0.5 border -1\n");
        fprintf(f_script, "set boxwidth 0.8 relative\n");
        fprintf(f_script, "set xtics rotate by -45\n");
        
        //PLOT
        fprintf(f_script, "plot \"dados.txt\" using 1:2 with boxes title \"Mulheres\" lc rgb \"blue\"\n");
        fclose(f_script);
    }

    printf("\nGerando grafico... Aguarde.\n");
    //ABERTURA AUTOMATICA DO GRÁFICO
    system("gnuplot -persist script_evolucao_mulheres.gp");
  /*Fim da Função*/
}
