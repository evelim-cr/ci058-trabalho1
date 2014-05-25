#include <stdio.h>
#include <strings.h>
#include "protocolo.h"
#include "conexao.h"

int main(int argc, char const *argv[])
{
	int s = criaConexao();
	char string[50];
	while(1)
	{
		bzero (string,50);
		recebe_string(s, string);
		printf("String recebida: "); puts (string);
	}
	return 0;
}