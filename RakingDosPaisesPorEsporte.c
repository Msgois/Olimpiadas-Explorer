# include <stdio.h>
# include <stdlib.h>
# include <string.h>

// Comecei criando um tipo de dado chamado "RakingDosPaises".
// Nele vou armazenar O NOC, O nome do País e a quantidade de medalhas no geral.
typedef struct {
    char NationalOlympicCommittee [5];
    char regiao [100];
    int medalhas;
} RankigDosPaises;

// Criei a função "AcessarRegioes" para começar a manipular o arquivo "noc-regions.csv"
// E conseguir os dados que eu vou precisar, o NOC e a região.
// Note que no primeiro argumento eu criei uma lista "listaDosPaises" do tipo "RankigDosPaises"
// E também um ponteiro do tipo int "TotalDePaises", ele vai servir como um indice de paises que armazena o total de paises.

void AcessarRegioes (RankigDosPaises listaDosPaises [], int *TotalDePaises) {

// Criei um ponteiro chamado "arquivoRegioes" do tipo FILE que vai receber o retorno da fopen
// A fopen tem como argumento o nome do arquivo que quero abrir e o modo como vou abrir ele
// No meu caso "r" que é somente para a  leitura do arquivo.
    FILE *arquivoRegioes = fopen ("noc_regions.csv", "r"); 

// Se o retorno que o ponteiro receber for = NULL isso quer dizer que deu um erro ao tentar abrir o arquivo.
// Por isso coloquei um aviso para eu ficar ciente se consegui ou não.
    if (arquivoRegioes == NULL) {
        printf ("ERRO. Arquivo nao foi aberto");
            return; 
    }

// Crie a variável "linha" do tipo char, ela vai armazenar todo o caractere de todas linhas,
// linha por linha do arquivo regions, como a quantidade de caractere varia, 
// coloquei um tamanho que garanta que não vou deixar passar nenhum em branco.
// Eu vou usar ela para achar o NOC e associar a região de um paise qualquer, para depois adcionar na variavel "TotalDepaises".

    char linha [300];

// Como a função fgets serve como um scanf somente para strings fiz o seguinte:
// No primeiro argumento usei "linha" que vai ser ocupado pela entrada "arquivoRegiõs" (terceiro argumento)
// que aponta para o arquivo regions. Fiz essa inicialização antes do While para descartar a primeira linha do regions.
    fgets (linha, sizeof linha, arquivoRegioes); 

// O laço while vai permitir a manipulação de cada linha para eu armazenar os dados importantes para mim.
// O NOC e a região.
    while (fgets(linha, sizeof linha, arquivoRegioes)) {
        
// Criei um ponteiro chamado "BuscarMembro" para guardar o endereço de memória do NOC e regiao.
// Que vai ser retornado pela função "strtok". 
        char *BuscarMembro = strtok (linha, ",");

// Se o conteúdo de BuscarMembro for diferente de NULL, "strcpy" vai copiar o conteúdo para o membro NOC
// da lista dos paises no indice que "TotalDePaises" apontar.
            if (BuscarMembro != NULL) {
                strcpy (listaDosPaises [*TotalDePaises].NationalOlympicCommittee, BuscarMembro);
            }

// Faço "buscarMembro" armazenar o contúdo até a próxima vírgula.
// Se for diferente de NULL, eu retiro o enter do final da string com "strcspn"
// e uso "strcpy" para copiar para o membro região
// do mesmo indice que eu copiei o NOC de antes.
        BuscarMembro = strtok (NULL, ",");
            if (BuscarMembro != NULL) {
                BuscarMembro [strcspn (BuscarMembro, "\n\r")] = 0; 
                strcpy (listaDosPaises [*TotalDePaises].regiao, BuscarMembro);
            }

// Inicializei a quantidade de medelhas em 0 do mesmo indice dos outros 2 membro: O NOC e a região.
            listaDosPaises [*TotalDePaises].medalhas = 0;

// Preenchido o NOC e regiao do pais,
// o indice "TotalDePaises" avança mais um para preeencher a struct do proximo indice da lista.
            (*TotalDePaises) ++;
    }

    fclose (arquivoRegioes); 

// Esse print serve para verificar se o total de países do arquivo realmente foram carregados para a lista.
    printf ("Total de Paises: %d", *TotalDePaises); 
}

// Crie a função "ContarMedalhas" ela recebe 3 argumentos.
// O primero é a lista dos paises.
// O segundo o indice da lista.
// O terceiro é  o esporte que for escolhido.
void ContarMedalhas (RankigDosPaises listaDosPaises [], int *TotalDePaises, char esporte []) {

// Abertura do arquivo results que contem os dados que eu quero.
    FILE *arquivoResults = fopen ("results.csv", "r");

// Um condicional só para saber se o arquivo realmente foi aberto.
        if (arquivoResults == NULL) {
            printf ("ERRO! Arquivo não foi aberto");
            return;
        } 

 // Criei a lista "linha" que vai receber os dados de cada linha do results. Uma por Uma.
 // Mais pra frente eu vou dividir a linha em 11 colunas.   
    char linha [1200];

// Criei 5 ponteiros tipo char. "inicio" vai armazenar o endereço do início da coluna.
// "fim" vai armazenar o endereço do fim da coluna.
// "BuscarEsporte" vai armazenar o endereço da coluna que tiver o esporte.
// "BuscarNOC" vai armazenar o endereço da coluna que tiver o NOC.
// "BuscarMedalha" vai armazenar o endreço da coluna que tiver a medalha.
    char *inicio = linha;
    char *fim;
    char *BuscarEsporte = NULL;
    char *BuscarNOC = NULL;
    char *BuscarMedalha = NULL;

//Fiz essa inicialização antes do While para descartar a primeira linha do regions.    
    fgets (linha, sizeof linha, arquivoResults);

// O laço while vai permitir a manipulação de cada linha para eu armazenar os dados importantes para mim.
// O esporte, NOC e medalha.
    while (fgets (linha, sizeof linha, arquivoResults)) {
    inicio = linha;
// Criei um for que vai dividir a linha em 11 colunas.
        for (int coluna = 1; coluna < 12; coluna ++) {

// O primeiro if verifica se a coluna é do tipo que começa com aspas.
// Como o endereço de incio é igual ao inicio da linha, o if verifica se ele é igual a aspas.
// se for, chamo a função "strchr" para buscar a próxima aspas começando a partir da posição seguinte de inicio.
// Depois uso outro if para o ponteiro fim receber o endereço da proxima vírgula a partir da posição seguinte da aspas.
            if (*inicio == '\"') {
                fim = strchr (inicio + 1, '\"');
                if (fim) {                   
                    fim = strchr (fim + 1, ',');
                }
            } 

// Aqui seria o exemplo de uma coluna normal com inicio diferente de aspas.
// eu fiz o ponteiro fim receber o endereço da próxima vírgula e igualEI a \0 para finalizar a string ali e guardar o conteúdo.
             
             else {
                fim = strchr (inicio, ',');
                if (fim) {
                    *fim = '\0';
                }
            }

// Criei condicionais para as colunas importantes.
// Se for a coluna do esporte, o ponteiro "BuscarEsporte" vai receber início.
// Se for a coluna do NOC, o ponteiro "BuscarNOC" vai receber início.
// Se for a coluna da medalha, o ponteiro "BuscarMedalha" vai receber início.
// Também chamei a "strspn" para tirar a quebra de linha, já que é a última coluna.

            if (coluna == 3) BuscarEsporte = inicio;
            if (coluna == 7) BuscarNOC = inicio;
            if (coluna == 11) {
                BuscarMedalha = inicio;
                BuscarMedalha[strcspn(BuscarMedalha, "\n\r")] = 0;
            }

// Se fim receber NULL é pq chegou ao fim da linha então acaba por aí.
            if (fim == NULL) break;

// Se não recebr o ponteiro inicio recebe o endereço do ponteiro fim + 1.
// O que faz com que inicio comece na coluna seguinte.
            inicio = fim + 1;
            }

// O primeiro if verifica se o esporte digitado é igual ao esporte de linha que está snedo linda.
// se for o segundo if verifica se a coluna de medalhas é diferende de nada e se o tamanho do ponteiro é maior que 0.
// Eu fiz assim pq percebi que toda linha que possui alguma medalha o tamanho da coluna é maior que zero.
// Se não tiver medlha o tamanho é 0. Vai ter só um \0 já que removi o enter algumas linha de cod atrás.
// Verificando que possui medalhas, criei um for que vai usar como parametro o Total de paises para navegar pela lista de paises.
// O proximo if verifica se o NOC da coluna 7 coincidir com o membro NOC da struct da listaDosPaises do indice atual
// Vai ser somado + 1 no membro de medalhas da struct de mesmo indice.
            if (strcmp (BuscarEsporte, esporte) == 0) {
                if (strcmp (BuscarMedalha, "NA") != 0 && strlen (BuscarMedalha) > 0) {
                    for (int i = 0; i < *TotalDePaises; i ++) {
                        if (strcmp (listaDosPaises [i].NationalOlympicCommittee, BuscarNOC) == 0) {
                            listaDosPaises [i].medalhas ++;
                            break;
                        }
                    }
                }
            }
        }       
    

fclose(arquivoResults);

// Apliquei a ordenação bubble sort para ordenar na ordem descrescente
RankigDosPaises SalvaDado;
    for (int i = 0; i < *TotalDePaises - 1; i++) {
        for (int j = 0; j < *TotalDePaises - 1; j++) {
            if (listaDosPaises[j].medalhas < listaDosPaises[j + 1].medalhas) {
                SalvaDado = listaDosPaises[j];
                listaDosPaises[j] = listaDosPaises[j + 1];
                listaDosPaises[j + 1] = SalvaDado;
            }
        }
    }

// Exibição do raking somente dos paises que possuem uma medalha ou mais.
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


int RakingDosPaisesPorEsporte () {

// Aplicação das funções.

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



