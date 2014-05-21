#include <stdio.h>

typedef struct
{
	char inicio[8];
	char tamanho[4];
	char sequencia[4];
	char tipo[4];
	char erro[4];
	char dados[16];
} quadro;

#define TAMMSG 40
#define CD 0
#define LS 1
#define PUT 2
#define GET 3
#define CAT 4
#define DADOS 7
#define TAMARQ 8
#define MOSTRA 9
#define ACK 10
#define FIMTXT 11
#define SUCESSO 13
#define ERRO 14
#define NACK 15

int main(int argc, char const *argv[])
{

	FILE *infile = fopen ("teste.txt","r");
	return 0;
}