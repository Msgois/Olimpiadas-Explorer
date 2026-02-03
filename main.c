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
