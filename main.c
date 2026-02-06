#include <stdio.h>
#include "RakingDosPaisesPorEsporte.c"
#include "paises_com_mais_participacao.c"
#include "Top_Dez_medalhistas.c"
#include "evolucao_mulheres_olimpiadas.c"

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
    AcessarRegioes();

    return 0;
  case '2':
    evolucao_mulheres_olimpiadas();

    return 0;
  case '3':
    paises_com_mais_participacao();

    return 0;
  case '4':
    Top_Des_medalhistas();

    return 0;
  case 'q':
    printf("Saindo\n");

    return 0;
  default:
    perror("Questão inválida");

    return 1;
  }
}
