#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "protocolo.h"
#include "conexao.h"

void cdRemotoServer (int socket, mensagem msg);
void lsRemotoServer(int socket, mensagem msg);
void EnviaDirAtual (int socket, mensagem msg);

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
			case DIR_ATUAL:
			{
				EnviaDirAtual(s, msg);
				break;
			}
			case CD:
			{
				cdRemotoServer(s, msg);
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
			comando[i]=msg.dados[j];
		bzero (&msg_bin, TAMMSG);
		if (msg.tipo==FIMTXT)
			acabou=1;
		else
		{
			recebe_mensagem_bin (socket, &msg_bin);
			msg = Mensagem_binToMensagem(msg_bin);
		}
	}
	puts (comando);
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
	system (comando);
	//funcao abre o arquivo e envia =)
	EnviaArq(socket, "file.tmp");
}

void cdRemotoServer (int socket, mensagem msg)
{
	mensagem_bin msg_bin;
	char *dir=NULL;
	int i=0, j, acabou=0;
	while (!acabou)
	{
		dir = realloc(dir,i+msg.tamanho);
		for (j=0; j < msg.tamanho; j++, i++)
			dir[i]=msg.dados[j];
		bzero (&msg_bin, TAMMSG);
		if (msg.tipo==FIMTXT)
			acabou=1;
		else
		{
			recebe_mensagem_bin (socket, &msg_bin);
			msg = Mensagem_binToMensagem(msg_bin);
		}
	}
	printf ("cd "); puts(dir);
	int enviar = chdir(dir);
	if (enviar)
		msg.tipo=ERRO;
	else
		msg.tipo=SUCESSO;
	msg.tamanho=1;
	msg.dados[0]=enviar;
	msg_bin = MensagemToMensagem_bin(msg);
	envia_mensagem_bin (socket, &msg_bin);
}

void EnviaDirAtual (int socket, mensagem msg)
{
	mensagem_bin msg_bin;
	char *dir; char erro[]="Nao foi possivel localizar o diretorio atual.";
    dir = (char *) (long int) get_current_dir_name();
    char *enviar;
    if (dir==0)
        enviar = erro;
    else
    	enviar = dir;
    int i=0, tamenviar = strlen(enviar)+1;
    msg.tipo=MOSTRA;
    msg.tamanho=0;
    while ( tamenviar-i > 0)
    {
        if (msg.tamanho<2)
        {
            msg.dados[(msg.tamanho)++] = enviar[i];
            i++;
        }
        if ((msg.tamanho==2) || (tamenviar-i == 0))
        {
            if (EhFimTexto(msg.dados))
                msg.tipo=FIMTXT;
            msg_bin = MensagemToMensagem_bin(msg);
            envia_mensagem_bin (socket, &msg_bin);
            // recebe mensagem conferindo se tem erro e manda dnovo caso nessecario
            msg.tamanho=0;
            bzero (msg.dados,2);
        }
    }
}