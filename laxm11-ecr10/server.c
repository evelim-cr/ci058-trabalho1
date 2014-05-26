#include <stdio.h>
#include <strings.h>
#include "protocolo.h"
#include "conexao.h"

int main(int argc, char const *argv[])
{
	int s = criaConexao();
	char string[50];
	mensagem msg;
	bzero (&msg,sizeof(mensagem));
	mensagem_bin msg_bin;
	bzero (&msg_bin, TAMMSG);
	int i=0,j;
	while(1)
	{
		recebe_mensagem_bin(s, &msg_bin);
		msg = Mensagem_binToMensagem(msg_bin);
		printf("%d mensagem:\n", i++);
		for (j=0; j < msg.tamanho; j++)
			putchar(msg.dados[j]);
		bzero (&msg_bin, TAMMSG);
		bzero (&msg,sizeof(mensagem));
	}
	return 0;
}