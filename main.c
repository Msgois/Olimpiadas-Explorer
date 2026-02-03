#include <search.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COUNTRIES_COUNT 231
#define MAX_LINE_LENGTH 2048
#define RESULTS_ROWS 308409

char *parse_csv_field(char **cursor) {
  char *start = *cursor;

  if (start == NULL || *start == '\0') {
    return NULL;
  }

  // TODO: botar numa função separada.
  if (*start == '"') {
    start++;
    char *end = start;

    while (*end && *end != '"') {
      end++;
    }

    if (*end == '"') {
      *end = '\0'; // Para a string na aspa fechada.
      *cursor = end + 1;

      if (**cursor == ',') {
        (*cursor)++;
      }
    }
    return start;
  }

  char *end = start;
  while (*end && *end != ',' && *end != '\n') {
    end++;
  }

  if (*end != '\0') {
    *end = '\0';
    *cursor = end + 1;
  } else {
    *cursor = end;
  }

  return start;
}

typedef struct {
  char noc[5];
  char country_name[257];
  int count;
} Country;

int country_comparator(const void *current, const void *other) {
  Country *current_converted = (Country *)current;
  Country *other_converted = (Country *)other;

  if (current_converted->count != other_converted->count) {
    return other_converted->count - current_converted->count;
  }

  return strcmp(current_converted->country_name, other_converted->country_name);
}

int paises_com_mais_pariticipacao() {
  FILE *noc_regions = fopen("noc_regions.csv", "r");

  if (!noc_regions) {
    perror("Error opening noc_regions.csv");

    return 1;
  }

  Country countries[COUNTRIES_COUNT];
  char line[MAX_LINE_LENGTH];
  int counter = 0;

  // Pula o cabeçalho.
  fgets(line, sizeof(line), noc_regions);

  while (fgets(line, sizeof(line), noc_regions) && counter < COUNTRIES_COUNT) {
    char *cursor = line;
    char *field;
    // Inteiro temporário para o caso específico de Tuvalu. Continue lendo para
    // saber mais sobre.
    int noc_tuvalu = 0;

    // 1. NOC.
    field = parse_csv_field(&cursor);
    strncpy(countries[counter].noc, field, 4);

    if (strcmp(field, "TUV") == 0) {
      noc_tuvalu = 1;
    }

    // 2. Region.
    field = parse_csv_field(&cursor);

    // Caso onde o país é desconhecido. O NOC é "UNK". Aqui mudamos a região
    // para algo que faça mais sentido.
    if (strcmp(field, "NA") == 0) {
      field = "Unknown";
    }

    // Caso onde o "NOC" é "TUV" ("Tuvalu"). Por algum motivo o criador colocou
    // "Tuvalu" em "notes", mas não em "region".
    if (noc_tuvalu) {
      field = "Tuvalu";
    }

    strncpy(countries[counter].country_name, field, 256);

    countries[counter].count = 0;

    counter++;
  }

  FILE *results = fopen("results.csv", "r");

  if (!results) {
    perror("Error opening results.csv");

    return 1;
  }

  ENTRY country;
  ENTRY *countryp;

  hcreate(COUNTRIES_COUNT);

  for (int i = 0; i < COUNTRIES_COUNT - 2; i++) {
    country.key = countries[i].noc;
    country.data = 0;

    countryp = hsearch(country, ENTER);

    if (!countryp) {
      perror("Entry failed");

      return 1;
    }
  }

  while (fgets(line, sizeof(line), results) && counter < RESULTS_ROWS) {
    char *cursor = line;
    ENTRY field;

    // Pula as primeiras seis entradas pois só ligamos pro NOC.
    for (int i = 0; i < 6; i++) {
      parse_csv_field(&cursor);
    }

    field.key = parse_csv_field(&cursor);
    countryp = hsearch(field, FIND);

    if (countryp) {
      countryp->data++;
    }
  }

  for (int i = 0; i < counter; i++) {
    ENTRY country;
    country.key = countries[i].noc;
    ENTRY *countryp = hsearch(country, FIND);

    if (countryp) {
      countries[i].count = (int)(long)countryp->data;
    }
  }

  qsort(countries, counter, sizeof(Country), country_comparator);

  for (int i = 0; i < 10; i++) {
    printf("%s: %d\n", countries[i].country_name, countries[i].count);
  }

  hdestroy();
  fclose(noc_regions);
  fclose(results);

  return 0;
}
// Comecei criando um tipo de dado chamado "RakingDosPaises", nele vou armazenar O NOC, O nome do País e a quantidade de medalhas no geral.

typedef struct {
    char NationalOlympicCommittee [4];
    char regiao;
    int medalhas;
} RankigDosPaises;

// Crie uma lista chamada "listaDosPaises" para armazenar o "RakingFosPaises" em cada posição. Criei também a vairável int "TotalDePaises" para armazenar a quantidade de paises que for snedo inserinda na lista, sempre que eu tiver alocando um dado novo na lista vai ser somado + 1 no total de paises para eu saber quantos paises tem. Vai servir como um parâmetro de tamanho para quando eu for percorrer a lista atras de um certo paí lá dentro.

RankigDosPaises listaDosPaises [300];
int TotalDePaises = 0;


// Criei a função "AcessarRegioes" para começar a manipular o arquivo "noc-regions.csv" e conseguir os dados que eu vou precisar, o NOC e região.

void AcessarRegioes () {
    FILE *arquivoRegioes = fopen ("noc_regions.cvs", "r"); // Criei um ponteiro chamado "arquivoRegioes" do tipo FILE que vai receber o retorno da fopen que por sua vez tem como argumento o nome do arquivo que quero abrir e o modo como vou abrir ele, no meu caso "r" que é somente para a  leitura do arquivo.

    if (arquivoRegioes == NULL) { // Se o retorno que o ponteiro receber for = NULL isso quer dizer que deu um erro ao tentar abrir o arquivo, por isso coloquei um aviso para eu ficar ciente se consegui ou não.
        printf ("Erro ao abrir o arquivo");
        return;
    } 
        
    char linha [300]; // Crie a variável "linha" do tipo char, ela vai armazenar todo o caractere de todas linhas. linha por linha do arquivo regions, como a quantidade de caractere varia, coloquei um tamanho que garanta que não vou deixar passar nenhum em branco. Eu vou usar ela para achar o NOC e associar a região de um paise qualquer, para depois adcionar na variavel "TotalDepaises".

    fgets (linha, sizeof linha, arquivoRegioes); // Como a função fgets serve como um scanf somente para strings fiz o seguinte: no primeiro argumento usei "linha" que vai ser ocupado pela entrada "arquivoRegiõs" (terceiro argumento) que aponta para o arquivo regions. Fiz essa inicialização antes do While para descartar a primeira linha do regions.

    while (fgets(linha, sizeof linha, arquivoRegioes)) {
        char *BuscarMembro = strtok (linha, ","); // Criei um ponteiro chamado "BuscarMembro" para guardar o endereço de memória do NOCe regiao que vai ser retornado pela função "strtok" que descarta os caracteres a partir do segundo argumento. 
            if (BuscarMembro != NULL) {
                strcpy (listaDosPaises [TotalDePaises].NationalOlympicCommittee, BuscarMembro); // A função "strcpy" que vem da biblioteca de string me permite copiar a string do segundo argumento para o primeiro. Eu coloquei no priemiro argumento a  minha "lista de paises" usando como indice o "Total de paises" que seria o 0 para armazenar no NOC da struct desse índice.
            }

        BuscarMembro = strtok (NULL, ",");
            if (BuscarMembro != NULL) {
                strcpy (listaDosPaises [TotalDePaises].regiao, BuscarMembro);
            }

            TotalDePaises ++; // Preenchido o NOC e regiao Do pais, o indice avança mais um para preeencher a struct do proximo indice da lista.
    }

    fclose (arquivoRegioes);
    
}





int main() {
  char questao;

  printf("Escolha uma questão:\n\n");
  printf("[1]: Para um esporte escolhido (ex.: Atletismo, Natação, Esqui), mostre o ranking dos países com mais medalhas nesse esporte.\n");
  printf("[2]: Calcule a evolução da quantidade de mulheres em cada olimpíada.\n");
  printf("[3]: Para um esporte escolhido (ex.: Atletismo, Natação, Esqui), mostre os 10 países que mais tiveram participação nesse esporte.\n");
  printf("[4]: Liste as 10 maiores medalhistas femininas nas olimpíadas.\n");
  printf("[q]: Sair\n");
  scanf("%c", &questao);

  switch (questao) {
  case '1':
    printf("1\n");

    return 0;
  case '2':
    printf("2\n");

    return 0;
  case '3':
    paises_com_mais_pariticipacao();
  case '4':
    printf("4\n");

    return 0;
  case 'q':
    printf("Saindo\n");

    return 0;
  default:
    perror("Questão inválida");

    return 1;
  }
}
