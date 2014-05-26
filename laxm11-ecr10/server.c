#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "protocolo.h"
#include "conexao.h"

void lsRemotoServer(int socket, mensagem msg);
void abreArqEnviaMsgCliente(int s);

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
	mensagem_bin msg_bin;
	char *comando=NULL;
	int i=0, j, acabou=0;
	while (!acabou)
	{
		comando = realloc(comando,i+msg.tamanho);
		for (j=0; j < msg.tamanho; j++, i++)
		{
			//printf("[%d]%c", msg.dados[j],msg.dados[j]);
			comando[i]=msg.dados[j];
		}
		bzero (&msg_bin, TAMMSG);
		if (msg.tipo==FIMTXT)
			acabou=1;
		else
		{
			recebe_mensagem_bin (socket, &msg_bin);
			msg = Mensagem_binToMensagem(msg_bin);
		}
	}
	comando = realloc (comando, i+10);
	i--;
	comando[i++]=' ';
	comando[i++]='>';
	comando[i++]='f';
	comando[i++]='i';
	comando[i++]='l';
	comando[i++]='e';
	comando[i++]='.';
	comando[i++]='t';
	comando[i++]='m';
	comando[i++]='p';
	comando[i]='\0';
	puts (comando);
	system (comando);
	//funcao abre o arquivo e envia =)
	abreArqEnviaMsgCliente(socket);
}

void abreArqEnviaMsgCliente(int s)
{ 
	FILE *fp;
	mensagem msg;
	mensagem_bin msg_bin; 
	fp = fopen("file.tmp","r");// Abre o arquivo para leitura
	if (fp == NULL) // se o arquivo nao existir exibe a mensagem de erro.
	{
		printf("Houve um erro ao abrir o arquivo\n");
		exit(1);
	}
	else{
		while (!feof(fp))
		{
			if (msg.tamanho<2){
				fread (&(msg.dados[(msg.tamanho)++]), sizeof(char), 1, fp);
			}
			else if(msg.tamanho == 2){
				msg_bin = MensagemToMensagem_bin(msg);
	            envia_mensagem_bin (s, &msg_bin);
	            msg.tamanho=0;
            	bzero (msg.dados,2);
        	}
		}
	}
	fclose(fp);	
}