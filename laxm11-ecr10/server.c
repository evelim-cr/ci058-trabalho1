#include <stdio.h>
#include <strings.h>
#include "protocolo.h"
#include "conexao.h"

int main(int argc, char const *argv[])
{
	int s = criaConexao();
	mensagem msg;
	bzero (&msg,sizeof(mensagem));
	mensagem_bin msg_bin;
	bzero (&msg_bin, TAMMSG);
	while(1)
	{
		recebe_mensagem_bin(s, &msg_bin);
		msg = Mensagem_binToMensagem(msg_bin);
		switch (msg.tipo)
		{
			case LS:
			{
				lsRemotoServer(s, msg);
				break;
			}
		}
	}
	return 0;
}

void lsRemotoServer(int socket, mensagem msg)
{
	mensagem_bin msg_bin
	char *comando, *ls;
	int i=0,j;
	while (msg.tipo!=FIMTXT)
	{
		comando = realloc(comando,i+msg.tamanho)
		for (j=0; j < msg.tamanho; j++, i++)
		{
			//printf("[%d]%c", msg.dados[j],msg.dados[j]);
			comando[i]=msg.dados[j];
		}
		bzero (&msg_bin, TAMMSG);
		recebe_mensagem_bin (s, &msg_bin);
		msg = Mensagem_binToMensagem(msg_bin);
	}
	ls = malloc (3+i+10);
	ls[0]='l';
	ls[1]='s';
	ls[2]=' ';
	strcat (ls, comando);
	ls[i+3]=' ';
	ls[i+3]='>';
	ls[i+3]='f';
	ls[i+3]='i';
	ls[i+3]='l';
	ls[i+3]='e';
	ls[i+3]='.';
	ls[i+3]='t';
	ls[i+3]='m';
	ls[i+3]='p';
	system (ls);

	//funcao abre o arquivo e envia =)
}
