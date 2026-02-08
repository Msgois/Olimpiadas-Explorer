#include <search.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOC_REGIONS_LINHAS 231
#define RESULTS_LINHAS 308409
#define TAMANH_MAXIMO_DE_LINHA 2048

/**
 * @brief Função para ler uma string cercada por aspas dentro de um CSV.
 *
 * Essa função analisa células cotadas encontradas nos CSVs do projeto.
 * Esses campos estão cotados nesse formato porque CSV significa
 * "Comma-Separated Values". Isso é apenas um de vários jeitos de usar
 * vírgulas num CSV. Outro jeito é mudar o delimitador.
 *
 * @param cursor Ponteiro para o pointeiro do caractere atual da célula. É
 * esperado que o caractere seja uma aspa dupla.
 * @param inicio A posição atual na célula.
 * @return Um ponteiro para o início da string cotada.
 */
char *analisar_celula_cotada(char **cursor, char *inicio) {
  inicio++;
  // Procura caracteres a frente para descobrir onde a string acaba.
  char *fim = inicio;

  while (*fim && *fim != '"') {
    fim++;
  }

  if (*fim == '"') {
    *fim = '\0'; // Para a string na aspa que fecha.
    *cursor = fim + 1;

    // Pula a vírgula no fim, deixando o ponteiro pronto para ler a próxima
    // célula.
    if (**cursor == ',') {
      (*cursor)++;
    }
  }
  return inicio;
}

/**
 * @brief Lê células de um arquivo CSV.
 * Suporta campos cotados através da função @ref{parse_quoted_field}.
 *
 * @param cursor Ponteiro para o pointeiro do caractere atual da célula.
 * @return Um ponteiro para o início da string analizada.
 */
char *analisar_celula_csv(char **cursor) {
  char *inicio = *cursor;

  // Cuidando de células vazias.
  if (inicio == NULL || *inicio == '\0') {
    return NULL;
  }

  // Cuidando de células cotadas.
  if (*inicio == '"') {
    return analisar_celula_cotada(cursor, inicio);
  }


  // Procura caracteres a frente para descobrir onde a string acaba.
  char *fim = inicio;
  while (*fim && *fim != ',' && *fim != '\n') {
    fim++;
  }

  if (*fim != '\0') {
    *fim = '\0';
    // Move o cursor para o início da próxima célula para deixar a leitura mais
    // confortável.
    *cursor = fim + 1;
  } else {
    *cursor = fim;
  }

  return inicio;
}

/** @struct Pais
 * @brief Struct para guardar informações dos países específicas para esse
 * problema.
 */
typedef struct {
  char noc[5];                  /**< National Occupational Classification. */
  char nome_do_pais[257];       /**< O nome do país. */
  int quantidade_de_atletas;    /**< O número de atletas que competiram em nome do país. */
} Pais;

/**
 * @brief Comparador para o struct @ref{Pais}.
 * @param atual O primeiro país para comparação.
 * @param outro O segundo país para comparação.
 * @return O país com mais atletas que competiram por ele. Se a quantidade for
 * igual, então retorna aquele com maior prioridade na ordem lexicográfica.
 */
int pais_comparador(const void *atual, const void *outro) {
  Pais *atual_convertido = (Pais *)atual;
  Pais *outro_convertido = (Pais *)outro;

  if (atual_convertido->quantidade_de_atletas != outro_convertido->quantidade_de_atletas) {
    return outro_convertido->quantidade_de_atletas - atual_convertido->quantidade_de_atletas;
  }

  return strcmp(atual_convertido->nome_do_pais, outro_convertido->nome_do_pais);
}

/**
 * Dado um esporte, mostra um top 10 com os países com mais participantes no
 * esporte.
 */
int paises_com_mais_participacao() {
  /**************************
   * Seção noc_regions.csv.
   **************************/
  FILE *noc_regions = fopen("noc_regions.csv", "r");

  if (!noc_regions) {
    perror("Erro ao abrir noc_regions.csv");

    return 1;
  }

  Pais paises[NOC_REGIONS_LINHAS];
  char linha[TAMANH_MAXIMO_DE_LINHA];
  int paises_indice = 0;

  // Pula o cabeçalho.
  fgets(linha, sizeof(linha), noc_regions);

  // Analisando as linhasde "noc_regions.csv" para criar a lista de países.
  while (fgets(linha, sizeof(linha), noc_regions) && paises_indice < NOC_REGIONS_LINHAS) {
    char *cursor = linha;
    char *celula;
    // Inteiro temporairo para o caso específico de Tuvalu. Para mais
    // informações, prossiga com a leitura.
    int noc_tuvalu = 0;
    int noc_refugee_olympic_team = 0;

    // NOC.
    celula = analisar_celula_csv(&cursor);
    strncpy(paises[paises_indice].noc, celula, 4);

    if (strcmp(celula, "TUV") == 0) {
      noc_tuvalu = 1;
    }

    if (strcmp(celula, "ROT") == 0) {
      noc_refugee_olympic_team = 1;
    }

    // region.
    celula = analisar_celula_csv(&cursor);

    // Caso onde o país do atleta é desconhecido. O NOC nesse caso é "UNK".
    // Aqui, mudamos a região de NA ("Not Applicable"?) para algo que faça mais
    // sentido.
    if (strcmp(celula, "NA") == 0) {
      celula = "Unknown";
    }

    // Caso onde o NOC é "TUV" ("Tuvalu"). Por algum motivo, o criador do arquivo
    // original colocou "Tuvalu" na coluna "notes" em vez da coluna "region".
    if (noc_tuvalu) {
      celula = "Tuvalu";
    }

    // Caso semelhante, só que "ROT" é o time olímpico de refugiados.
    if (noc_refugee_olympic_team) {
      celula = "Refugee Olympic Team";
    }

    strncpy(paises[paises_indice].nome_do_pais, celula, 256);

    // Inicializando a quantidade de atletas.
    paises[paises_indice].quantidade_de_atletas = 0;

    paises_indice++;
  }

  /**********************
   * Seção results.csv.
   **********************/
  FILE *results = fopen("results.csv", "r");

  if (!results) {
    perror("Erro ao abrir results.csv");

    return 1;
  }

  // POSIX define o header search.h para hash tables em C. É meio chatinho de
  // usar, mas serve. Veja hsearch(3) para mais detalhas (ou só use esse link:
  // <https://linux.die.net/man/3/hsearch>.)
  // Eu peguei a ideia aqui.
  //
  // Criando uma hash table para países. Contém pares "NOC-Atletas".
  ENTRY nocs_atletas;
  ENTRY *nocs_atletas_ponteiro;

  hcreate(NOC_REGIONS_LINHAS);

  // Inicializando a hash table.
  for (int i = 0; i < NOC_REGIONS_LINHAS - 2; i++) {
    nocs_atletas.key = paises[i].noc;
    nocs_atletas.data = 0;

    nocs_atletas_ponteiro = hsearch(nocs_atletas, ENTER);

    if (!nocs_atletas_ponteiro) {
      perror("Entrada falhou");

      return 1;
    }
  }

  // Esporte que será pesquisado.
  char discipline[257];
  printf("Escolha um esporte:\n");
  scanf(" %256s", discipline);

  // Analisando as linhas de "results.csv" para pegar a quantidade de atletas
  // que participaram no dado esporte.
  while (fgets(linha, sizeof(linha), results) && paises_indice < RESULTS_LINHAS) {
    char *cursor = linha;
    ENTRY celula;
    ENTRY *celula_ponteiro;

    // Pula as primeiras sete colunas pois não precisamos delas.
    for (int i = 0; i < 7; i++) {
      analisar_celula_csv(&cursor);
    }

    // NOC.
    celula.key = analisar_celula_csv(&cursor);

    // discipline (esporte).
    char *celula_discipline = analisar_celula_csv(&cursor);

    // Pula a linha se o esporte não for aquele que o usuário escolheu.
    if (!celula_discipline || strcmp(celula_discipline, discipline) != 0) {
      continue;
    }

    celula_ponteiro = hsearch(celula, FIND);

    // Incrementando o número de atletas NA HASH TABLE. Mais tarde, modificamos
    // o array de structs.
    if (celula_ponteiro) {
      celula_ponteiro->data++;
    }
  }

  // Agora, você deve estar perguntando: "Por que isso foi feito? Por que usar
  // uma hash table? Não dava para só pegar o NOC e depois fazer um loop pelo
  // array de países?". Bem, eu tentei fazer isso, mas o programa rodou
  // significantemente mais lento. Isso funciona melhor. Mas deve ter um jeito
  // mais fácil de fazer isso sem usar uma ferramenta POSIX obscura.
  for (int i = 0; i < paises_indice; i++) {
    ENTRY pais;
    pais.key = paises[i].noc;
    ENTRY *pais_ponteiro = hsearch(pais, FIND);

    if (pais_ponteiro) {
      // data é de tipo "void *". Se uma conversão não for feita, o compilador
      // reclama (sem conversão dá um erro, conversão direta para int dá um aviso).
      paises[i].quantidade_de_atletas = (int)(long)pais_ponteiro->data;
    }
  }

  qsort(paises, paises_indice, sizeof(Pais), pais_comparador);

  /**************************
   * Seção Plotting.
   **************************/
  FILE *data = fopen("data.dat", "w");

  if (!data) {
    perror("Erro ao escrever data.dat\n");

    return 1;
  }

  printf("=== Top 10 Países com mais jogadores de %s (nomes dos países em inglês) ===", discipline);
  printf("\n");
  for (int i = 0; i < 10; i++) {
    fprintf(data, "%d %s %d\n", i, paises[i].nome_do_pais, paises[i].quantidade_de_atletas);
    printf("%s: %d\n", paises[i].nome_do_pais, paises[i].quantidade_de_atletas);
  }


  FILE *gnuplot_pipe = popen("gnuplot -persist", "w");

  if (!gnuplot_pipe) {
    perror("Erro ao abrir um pipe para o gnuplot");

    return 1;
  }

  // Veja
  // <https://stackoverflow.com/questions/327576/how-do-you-plot-bar-charts-in-gnuplot>
  // e
  // <https://gist.github.com/arosenb4/ab627b80035ca2e7bbf4ce9b39de4910>.
  fprintf(gnuplot_pipe, "set term pngcairo\n");
  fprintf(gnuplot_pipe, "set output 'graph.png'\n");
  fprintf(gnuplot_pipe,
          "set title 'Top 10 Países com mais jogadores de %s (nomes dos países em inglês)'\n",
          discipline);
  fprintf(gnuplot_pipe, "set boxwidth 0.5\n");
  fprintf(gnuplot_pipe, "set style fill solid\n");
  // Prevents overlapping issues with the labels.
  fprintf(gnuplot_pipe, "set xtics rotate by -45\n");
  fprintf(gnuplot_pipe, "plot 'data.dat' using 1:3:xtic(2) ti 'Atletas' with boxes\n");

  hdestroy();
  fclose(noc_regions);
  fclose(results);
  fclose(data);
  fflush(gnuplot_pipe);
  pclose(gnuplot_pipe);

  return 0;
}
