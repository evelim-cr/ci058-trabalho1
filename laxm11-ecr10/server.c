#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "protocolo.h"
#include "conexao.h"

void cdRemotoServer (int socket, mensagem msg);
void lsRemotoServer(int socket, mensagem msg);
void catRemotoServer(int socket, mensagem msg);
void getServer(int socket, mensagem msg);
void EnviaDirAtual (int socket, mensagem msg);

int main(int argc, char const *argv[])
{
	int s = criaConexao();
	mensagem msg;
	bzero (&msg,sizeof(mensagem));
	mensagem_bin msg_bin;
	bzero (&msg_bin, TAMMSG);
	system("clear");
	while(1)
	{
		puts ("----------------------");
		puts ("Aguardando mensagem...");	//log
		recebe_mensagem_bin(s, &msg_bin);
		msg = Mensagem_binToMensagem(msg_bin);
		puts ("----------------------");
		puts ("Mensagem Recebida!");	//log
		switch (msg.tipo)
		{
			case LS:
			{
				puts ("\tTipo: LS");	//log
				lsRemotoServer(s, msg);
				break;
			}
			case DIR_ATUAL:
			{
				puts ("\tTipo: DIR_ATUAL");	//log
				EnviaDirAtual(s, msg);
				break;
			}
			case CD:
			{
				puts ("\tTipo: CD");	//log
				cdRemotoServer(s, msg);
				break;
			}
			case CAT:
			{
				puts ("\tTipo: CAT");	//log
				catRemotoServer(s, msg);
				break;
			}
			case GET:
			{
				puts ("\tTipo: GET");	//log
				getServer(s, msg);
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
	int i=0, j, acabou=0, countmsg=1;
	puts ("\tRecebendo comando 'ls'.");	//log
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
			countmsg++;
		}
	}
	printf ("\t[%d mensagens recebidas]\n\tComando recebido: ", countmsg); puts (comando);	//log
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
	puts ("\tResposta armazenada no arquivo 'file.tmp'.");	//log
	//funcao abre o arquivo e envia =)
	EnviaArq(socket, "file.tmp", LS);
	system ("rm -f file.tmp");
}

void catRemotoServer(int socket, mensagem msg)
{
	mensagem_bin msg_bin;
	char *comando=NULL;
	int i=0, j, acabou=0, countmsg=1;
	puts ("\tRecebendo comando 'cat'.");	//log
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
			countmsg++;
		}
	}
	printf ("\t[%d mensagens recebidas]\n\tComando recebido: ", countmsg); puts (comando);	//log
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
	puts ("\tResposta armazenada no arquivo 'file.tmp'.");	//log
	EnviaArq(socket, "file.tmp", CAT);
	system ("rm -f file.tmp");
}

void getServer (int socket, mensagem msg)
{
	mensagem_bin msg_bin;
	char *filename=NULL;
	FILE *fp;
	int i=0, j, acabou=0, countmsg=0;
	puts ("\tRecebendo nome do arquivo.");	//log
	while (!acabou)
	{
		filename = realloc(filename,i+msg.tamanho);
		for (j=0; j < msg.tamanho; j++, i++)
			filename[i]=msg.dados[j];
		bzero (&msg_bin, TAMMSG);
		if (msg.tipo==FIMTXT)
			acabou=1;
		else
		{
			recebe_mensagem_bin (socket, &msg_bin);
			msg = Mensagem_binToMensagem(msg_bin);
			countmsg++;
		}
	}
	printf ("\t[%d mensagens recebidas]\n\tArquivo requisitado: ", countmsg); puts (filename);	//log
	EnviaArq (socket, filename, GET);
}

void cdRemotoServer (int socket, mensagem msg)
{
	mensagem_bin msg_bin;
	char *dir=NULL;
	int i=0, j, acabou=0, countmsg=1;
	puts ("\tRecebendo comando 'cd'.");	//log
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
			countmsg++;
		}
	}
	printf ("\t[%d mensagens recebidas]\n\tComando recebido: cd ", countmsg); puts(dir);	//log
	int enviar = chdir(dir);
	if (enviar)
	{
    	puts ("\tEnviando mensagem de erro.");	//log
		msg.tipo=ERRO;
	}
	else
	{
    	puts ("\tEnviando mensagem de sucesso.");	//log
		msg.tipo=SUCESSO;
	}
	msg.tamanho=1;
	msg.dados[0]=enviar;
	msg_bin = MensagemToMensagem_bin(msg);
	envia_mensagem_bin (socket, &msg_bin);
	puts ("\tPronto!");	//log
}

void EnviaDirAtual (int socket, mensagem msg)
{
	mensagem_bin msg_bin;
	char *dir; char erro[]="Nao foi possivel localizar o diretorio atual.";
    dir = (char *) (long int) get_current_dir_name();
    char *enviar;
    if (dir==0)
    {
    	puts ("\tEnviando mensagem de erro.");	//log
        enviar = erro;
    }
    else
    {
    	puts ("\tEnviando Diretorio Atual.");	//log
    	enviar = dir;
    }
    int i=0, tamenviar = strlen(enviar)+1, countmsg=1;
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
            countmsg++;
            // recebe mensagem conferindo se tem erro e manda dnovo caso nessecario
            msg.tamanho=0;
            bzero (msg.dados,2);
        }
    }
    if (dir==0)
    	printf("\t[%d mensagens enviadas]\n\tMensagem de erro enviada.\n", countmsg);	//log
    else
    	printf("\t[%d mensagens enviadas]\n\tDiretorio atual enviado.\n", countmsg);	//log
}