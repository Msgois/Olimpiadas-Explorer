#include <search.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOC_REGIONS_ROWS 231
#define RESULTS_ROWS 308409
#define MAX_LINE_LENGTH 2048

/**
 * @brief Function to read a quoted string inside a CSV.
 *
 * This function parses the quoted fields found in the CSVs of the project.
 * These fields are quoted in this format because CSV stands for
 * "Comma-Separated Values". This is just one of the ways to use commas in
 * CSV. Another way is to change the delimiter.
 *
 * @param cursor Pointer to the pointer of the current character in the field.
 * Expected to be a double quote.
 * @param start The current position in the field.
 * @return A pointer to the start of the string inside the quotes.
 */
char *parse_quoted_field(char **cursor, char *start) {
  start++;
  // Looks for the characters ahead so we can find where the string ends.
  char *end = start;

  while (*end && *end != '"') {
    end++;
  }

  if (*end == '"') {
    *end = '\0'; // Stops the string in the closing quote.
    *cursor = end + 1;

    // Skips the comma at the end, so we can get back to parsing the CSV
    // just fine.
    if (**cursor == ',') {
      (*cursor)++;
    }
  }
  return start;
}

/**
 * @brief Reads the fields of a CSV file.
 * Supports quoted fields through @ref{parse_quoted_field}.
 *
 * @param cursor Pointer to the pointer of the current character in the field.
 * @return A pointer to the start of the parsed string.
 */
char *parse_csv_field(char **cursor) {
  char *start = *cursor;

  // Handling empty fields.
  if (start == NULL || *start == '\0') {
    return NULL;
  }

  // Handling quoted fields.
  if (*start == '"') {
    return parse_quoted_field(cursor, start);
  }

  // Looks for the characters ahead so we can find where the string ends.
  char *end = start;
  while (*end && *end != ',' && *end != '\n') {
    end++;
  }

  if (*end != '\0') {
    *end = '\0';
    // Moves the cursor to the start of the next field so we can read the next
    // field more comfortably.
    *cursor = end + 1;
  } else {
    *cursor = end;
  }

  return start;
}

/** @struct Country
 * @brief Struct for storing country data specific to this problem.
 */
typedef struct {
  char noc[5];                  /**< National Occupational Classification. */
  char country_name[257];       /**< The name of the country. */
  int athlete_count;            /**< The number of athletes that played for this country. */
} Country;

/**
 * @brief Comparator for the @ref{Country} struct.
 * @param current The first country for comparison.
 * @param other The second country for comparison.
 * @return The country with more athletes that played for it. If the count is
 * the same, then compare by lexicographical order instead.
 */
int country_comparator(const void *current, const void *other) {
  Country *current_converted = (Country *)current;
  Country *other_converted = (Country *)other;

  if (current_converted->athlete_count != other_converted->athlete_count) {
    return other_converted->athlete_count - current_converted->athlete_count;
  }

  return strcmp(current_converted->country_name, other_converted->country_name);
}

/**
 * Given a sport, show a top 10 with the countries with most participants in
 * the sport.
 */
int countries_with_most_participation() {
  /**************************
   * noc_regions.csv section.
   **************************/
  FILE *noc_regions = fopen("noc_regions.csv", "r");

  if (!noc_regions) {
    perror("Error reading noc_regions.csv");

    return 1;
  }

  Country countries[NOC_REGIONS_ROWS];
  char line[MAX_LINE_LENGTH];
  int current_country_index = 0;

  // Skips the header.
  fgets(line, sizeof(line), noc_regions);

  // Parsing the lines of "noc_regions.csv" to build the list of countries.
  while (fgets(line, sizeof(line), noc_regions) && current_country_index < NOC_REGIONS_ROWS) {
    char *cursor = line;
    char *field;
    // Temporary integer for the Tuvalu specific case. Read on to learn
    // more about this specific case.
    int noc_tuvalu = 0;

    // NOC.
    field = parse_csv_field(&cursor);
    strncpy(countries[current_country_index].noc, field, 4);

    if (strcmp(field, "TUV") == 0) {
      noc_tuvalu = 1;
    }

    // Region.
    field = parse_csv_field(&cursor);

    // Case where the country is unknown. The NOC for this case is "UNK".
    // Here, we change the region from NA ("Not Applicable"?) to something that
    // makes more sense.
    if (strcmp(field, "NA") == 0) {
      field = "Unknown";
    }

    // Case where the NOC is "TUV" ("Tuvalu"). For some reason, the creator of
    // the original file placed "Tuvalu" in the "notes" column instead of
    // "region".
    if (noc_tuvalu) {
      field = "Tuvalu";
    }

    strncpy(countries[current_country_index].country_name, field, 256);

    // Initializing the athlete count.
    countries[current_country_index].athlete_count = 0;

    current_country_index++;
  }

  /**********************
   * results.csv section.
   **********************/
  FILE *results = fopen("results.csv", "r");

  if (!results) {
    perror("Error reading results.csv");

    return 1;
  }

  // POSIX defines the search.h header for hash tables in C. It's a bit hard to
  // work with, but it does the job. See hsearch(3) for more details (or just go
  // here: <https://linux.die.net/man/3/hsearch>.)
  // I got the idea from here: <https://www.delftstack.com/howto/c/dictionary-in-c/>.
  // Creating a hash table for the countries. Contains "NOC-Athletes" pairs.
  ENTRY nocs_athletes;
  ENTRY *nocs_athletes_pointer; // "p" = pointer

  hcreate(NOC_REGIONS_ROWS);

  // Initializing the hash table.
  for (int i = 0; i < NOC_REGIONS_ROWS - 2; i++) {
    nocs_athletes.key = countries[i].noc;
    nocs_athletes.data = 0;

    nocs_athletes_pointer = hsearch(nocs_athletes, ENTER);

    if (!nocs_athletes_pointer) {
      perror("Entry failed");

      return 1;
    }
  }

  // Sport to be searched for. See the README for a list of sports.
  char discipline[257];
  printf("Pick a country from the list in the README:\n");
  scanf(" %256s", discipline);

  // Parsing the lines of "results.csv" to get the athlete count for athletes
  // that played in the given sport.
  while (fgets(line, sizeof(line), results) && current_country_index < RESULTS_ROWS) {
    char *cursor = line;
    ENTRY field;
    ENTRY *field_pointer;

    // Skips the first seven columns.
    for (int i = 0; i < 7; i++) {
      parse_csv_field(&cursor);
    }

    // NOC.
    field.key = parse_csv_field(&cursor);

    // Discipline (sport).
    char *discipline_field = parse_csv_field(&cursor);

    // Skips the line if the sport isn't the one the user chose.
    if (!discipline_field || strcmp(discipline_field, discipline) != 0) {
      continue;
    }

    field_pointer = hsearch(field, FIND);

    // Incrementing the number of athletes IN THE HASH TABLE. Later on, we'll
    // modify the array of structs.
    if (field_pointer) {
      field_pointer->data++;
    }
  }

  // Now, you may ask, "Why was this done? Why use a hash table? Couldn't you
  // just take the NOC and then loop through the array of countries?". Well,
  // I tried to do that, and the code ran significantly slower. This works
  // better. But there's probably an easier way to do this without using an
  // obscure POSIX tool.
  for (int i = 0; i < current_country_index; i++) {
    ENTRY country;
    country.key = countries[i].noc;
    ENTRY *countryp = hsearch(country, FIND);

    if (countryp) {
      // data is of void * type. If we don't do a conversion like this the
      // compiler complains (no conversion is an error, direct conversion to
      // int is a warning).
      countries[i].athlete_count = (int)(long)countryp->data;
    }
  }

  qsort(countries, current_country_index, sizeof(Country), country_comparator);

  /**************************
   * Plotting section.
   **************************/
  FILE *data = fopen("data.dat", "w");

  if (!data) {
    perror("Error writing data.dat\n");

    return 1;
  }

  printf("=== Top 10 Countries with the Most %s Players ===", discipline);
  printf("\n");
  for (int i = 0; i < 10; i++) {
    fprintf(data, "%d %s %d\n", i, countries[i].country_name, countries[i].athlete_count);
    printf("%s: %d\n", countries[i].country_name, countries[i].athlete_count);
  }

  FILE *gnuplot_pipe = popen("gnuplot -persist", "w");

  if (!gnuplot_pipe) {
    perror("Error opening a pipe to gnuplot");

    return 1;
  }

  // See
  // <https://stackoverflow.com/questions/327576/how-do-you-plot-bar-charts-in-gnuplot>
  // and
  // <https://gist.github.com/arosenb4/ab627b80035ca2e7bbf4ce9b39de4910>.
  fprintf(gnuplot_pipe, "set term pngcairo\n");
  fprintf(gnuplot_pipe, "set output 'graph.png'\n");
  fprintf(gnuplot_pipe,
          "set title 'Top 10 Countries with the Most %s Players'\n",
          discipline);
  fprintf(gnuplot_pipe, "set boxwidth 0.5\n");
  fprintf(gnuplot_pipe, "set style fill solid\n");
  // Prevents overlapping issues with the labels.
  fprintf(gnuplot_pipe, "set xtics rotate by -45\n");
  fprintf(gnuplot_pipe, "plot 'data.dat' using 1:3:xtic(2) ti 'Athletes' with boxes\n");

  hdestroy();
  fclose(noc_regions);
  fclose(results);
  fclose(data);
  fflush(gnuplot_pipe);
  pclose(gnuplot_pipe);

  return 0;
}
