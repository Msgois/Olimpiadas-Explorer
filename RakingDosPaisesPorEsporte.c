# include <stdio.h>
# include <stdlib.h>
# include <string.h>

// Comecei criando um tipo de dado chamado "RakingDosPaises", nele vou armazenar O NOC, O nome do País e a quantidade de medalhas no geral.

typedef struct {
    char NationalOlympicCommittee [5];
    char regiao [100];
    int medalhas;
} RankigDosPaises;

// Criei a função "AcessarRegioes" para começar a manipular o arquivo "noc-regions.csv" e conseguir os dados que eu vou precisar, o NOC e região.

void AcessarRegioes (RankigDosPaises listaDosPaises [], int *TotalDePaises) {
    FILE *arquivoRegioes = fopen ("noc_regions.csv", "r"); // Criei um ponteiro chamado "arquivoRegioes" do tipo FILE que vai receber o retorno da fopen que por sua vez tem como argumento o nome do arquivo que quero abrir e o modo como vou abrir ele, no meu caso "r" que é somente para a  leitura do arquivo.

    if (arquivoRegioes == NULL) { // Se o retorno que o ponteiro receber for = NULL isso quer dizer que deu um erro ao tentar abrir o arquivo, por isso coloquei um aviso para eu ficar ciente se consegui ou não.
        printf ("Erro ao abrir o arquivo");
        return;
    }

    char linha [300]; // Crie a variável "linha" do tipo char, ela vai armazenar todo o caractere de todas linhas. linha por linha do arquivo regions, como a quantidade de caractere varia, coloquei um tamanho que garanta que não vou deixar passar nenhum em branco. Eu vou usar ela para achar o NOC e associar a região de um paise qualquer, para depois adcionar na variavel "TotalDepaises".

    fgets (linha, sizeof linha, arquivoRegioes); // Como a função fgets serve como um scanf somente para strings fiz o seguinte: no primeiro argumento usei "linha" que vai ser ocupado pela entrada "arquivoRegiõs" (terceiro argumento) que aponta para o arquivo regions. Fiz essa inicialização antes do While para descartar a primeira linha do regions.

    while (fgets(linha, sizeof linha, arquivoRegioes)) {
        char *BuscarMembro = strtok (linha, ","); // Criei um ponteiro chamado "BuscarMembro" para guardar o endereço de memória do NOCe regiao que vai ser retornado pela função "strtok" que descarta os caracteres a partir do segundo argumento.
            if (BuscarMembro != NULL) {
                strcpy (listaDosPaises [*TotalDePaises].NationalOlympicCommittee, BuscarMembro); // A função "strcpy" que vem da biblioteca de string me permite copiar a string do segundo argumento para o primeiro. Eu coloquei no priemiro argumento a  minha "lista de paises" usando como indice o "Total de paises" que seria o 0 para armazenar no NOC da struct desse índice.
            }

        BuscarMembro = strtok (NULL, ",");
            if (BuscarMembro != NULL) {
                BuscarMembro [strcspn (BuscarMembro, "\n\r")] = 0; // A função "strcpsn" vai ignorar o segundo argumento. Fiz isso para evitar que o /n saia no nome do país.
                strcpy (listaDosPaises [*TotalDePaises].regiao, BuscarMembro);
            }

            listaDosPaises [*TotalDePaises].medalhas = 0;
            (*TotalDePaises) ++; // Preenchido o NOC e regiao Do pais, o indice avança mais um para preeencher a struct do proximo indice da lista.
    }

    fclose (arquivoRegioes);
    printf ("Total de Paises: %d", *TotalDePaises);
}

void ContarMedalhas (RankigDosPaises listaDosPaises [], int *TotalDePaises, char esporte []) {
    FILE *arquivoResults = fopen ("results.csv", "r");

        if (arquivoResults == NULL) {
            printf ("ERRO! Arquivo não foi aberto");
            return;
        }


    char linha [1200];
    char *Descarte;
    char *BuscarEsporte;
    char *BuscarNOC;
    char *BuscarMedalha;

    fgets (linha, sizeof linha, arquivoResults);

    while (fgets (linha, sizeof linha, arquivoResults)) {
       Descarte = strtok (linha, ",");
       Descarte = strtok (NULL, ",");
       BuscarEsporte = strtok (NULL, ",");

        if (BuscarEsporte != NULL && strcmp (BuscarEsporte, esporte) == 0) {
            Descarte = strtok (NULL, ",");
            Descarte = strtok (NULL, ",");
            Descarte = strtok (NULL, ",");
            Descarte = strtok (NULL, ",");
            BuscarNOC = strtok (NULL, ",");
            Descarte = strtok (NULL, ",");
            Descarte = strtok (NULL, ",");
            Descarte = strtok (NULL, ",");
            BuscarMedalha = strtok (NULL, ",");
        }

            if (BuscarNOC != NULL && BuscarMedalha != NULL) {
                for (int i = 0; i < *TotalDePaises; i ++) {
                    if (strcmp(listaDosPaises [i].NationalOlympicCommittee, BuscarNOC) == 0) {
                        listaDosPaises [i].medalhas ++;
                        break;
            }

            if ((BuscarMedalha = strtok (NULL, ",")) != NULL) {
                for (int i = 0; i < *TotalDePaises; i ++) {
                    if (strcmp(listaDosPaises [i].NationalOlympicCommittee, BuscarNOC) == 0) {
                        listaDosPaises [i].medalhas ++;
                        break;
                    }
                }
            }
        }
    }
}

fclose (arquivoResults);

RankigDosPaises SalvaDado;

for (int i = 0; i < *TotalDePaises - 1; i ++) {
    for (int j = 0; j < *TotalDePaises - 1; j ++) {
        if (listaDosPaises [j].medalhas < listaDosPaises [j + 1].medalhas) {
            SalvaDado = listaDosPaises [j];
            listaDosPaises [j] = listaDosPaises [j + 1];
            listaDosPaises [j + 1] = SalvaDado;
        }
    }
}

printf("\n--- RANKING DE PAISES POR MEDALHA: %s ---\n\n", esporte);

for (int k = 0; k < *TotalDePaises; k++) {
    if (listaDosPaises[k].medalhas > 0) {
        printf("%d. %s (%s) - Medalhas: %d\n", k + 1,
                listaDosPaises[k].regiao,
                listaDosPaises[k].NationalOlympicCommittee,
                listaDosPaises[k].medalhas);
        }
    }
}


int main () {

// Criei uma lista chamada "listaDosPaises" para armazenar o "RakingFosPaises" em cada posição. Criei também a vairável int "TotalDePaises" para armazenar a quantidade de paises que for snedo inserinda na lista, sempre que eu tiver alocando um dado novo na lista vai ser somado + 1 no total de paises para eu saber quantos paises tem. Vai servir como um parâmetro de tamanho para quando eu for percorrer a lista atras de um certo paí lá dentro.

    RankigDosPaises listaDosPaises [300];
    int TotalDePaises = 0;
    char esporte [50];

    AcessarRegioes (listaDosPaises, &TotalDePaises);

    printf("\nDigite o esporte para o ranking em ingles: ");
    fflush (stdin);
    fgets (esporte, sizeof (esporte), stdin);
    esporte [strcspn(esporte, "\n\r")] = 0;

    ContarMedalhas (listaDosPaises, &TotalDePaises, esporte);

}
