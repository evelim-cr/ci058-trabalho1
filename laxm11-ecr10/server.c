#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "protocolo.h"
#include "conexao.h"

void lsRemotoServer(int socket, mensagem msg);
void cdRemotoServer (int socket, mensagem msg);
void catRemotoServer(int socket, mensagem msg);
void getServer(int socket, mensagem msg);
void putServer (int s, mensagem msg);
void EnviaDirAtual (int socket, mensagem msg);

int main(int argc, unsigned char const *argv[])
{
	int s = criaConexao();
	mensagem msg;
	system("clear");
	while(1)
	{
		bzero (&msg,sizeof(mensagem));
		mensagem_bin msg_bin;
		bzero (&msg_bin, TAMMSG);
		puts ("----------------------");
		puts ("Aguardando mensagem...");	//log
		do{
			recebe_mensagem_bin(s, &msg_bin);
			msg = Mensagem_binToMensagem(msg_bin);
		}while( (msg.tipo==ACK) || (msg.tipo==NACK) );	
		switch (msg.tipo)
		{
			case LS:
			{
				puts ("----------------------");
				puts ("Mensagem Recebida!");	//log
				puts ("\tTipo: LS");	//log
				lsRemotoServer(s, msg);
				break;
			}
			case DIR_ATUAL:
			{
				puts ("----------------------");
				puts ("Mensagem Recebida!");	//log
				puts ("\tTipo: DIR_ATUAL");	//log
				EnviaDirAtual(s, msg);
				break;
			}
			case CD:
			{
				puts ("----------------------");
				puts ("Mensagem Recebida!");	//log
				puts ("\tTipo: CD");	//log
				cdRemotoServer(s, msg);
				break;
			}
			case CAT:
			{
				puts ("----------------------");
				puts ("Mensagem Recebida!");	//log
				puts ("\tTipo: CAT");	//log
				catRemotoServer(s, msg);
				break;
			}
			case GET:
			{
				puts ("----------------------");
				puts ("Mensagem Recebida!");	//log
				puts ("\tTipo: GET");	//log
				getServer(s, msg);
				break;
			}
			case PUT:
			{
				puts ("----------------------");
				puts ("Mensagem Recebida!");	//log
				puts ("\tTipo: PUT");	//log
				putServer (s, msg);
				break;
			}
			default:
			{
				//altus lixu
				break;
			}
		}
	}
	return 0;
}

void lsRemotoServer(int socket, mensagem msg)
{
	mensagem_bin msg_bin;
	unsigned char *comando=NULL;
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
	comando = realloc (comando, i+11);
	i--;
	comando[i++]=' ';
	comando[i++]='>';
	comando[i++]='.';
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
	puts ("\tResposta armazenada no arquivo '.file.tmp'.");	//log
	//funcao abre o arquivo e envia =)
	EnviaArq(socket, ".file.tmp", LS);
	system ("rm -f .file.tmp");
}

void cdRemotoServer (int socket, mensagem msg)
{
	mensagem_bin msg_bin;
	unsigned char *dir=NULL;
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

void catRemotoServer(int socket, mensagem msg)
{
	mensagem_bin msg_bin;
	unsigned char *comando=NULL;
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
	comando = realloc (comando, i+11);
	i--;
	comando[i++]=' ';
	comando[i++]='>';
	comando[i++]='.';
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
	puts ("\tResposta armazenada no arquivo '.file.tmp'.");	//log
	EnviaArq(socket, ".file.tmp", CAT);
	system ("rm -f .file.tmp");
}

void getServer (int socket, mensagem msg)
{
	mensagem_bin msg_bin;
	unsigned char *filename=NULL;
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

void putServer (int s, mensagem msg)
{
	mensagem_bin msg_bin;
	unsigned char *filename=NULL;
	FILE *fp;
	int i=0, j, acabou=0, countmsg=0;
	puts ("\tRecebendo nome do arquivo.");	//log
	while (!acabou)
	{
		filename = realloc(filename,i+msg.tamanho);
		for (j=0; j < msg.tamanho; j++, i++)
			filename[i]=msg.dados[j];
		bzero (&msg_bin, TAMMSG);
		if (msg.tipo==FIMTXT){ //recebeu o nome do arquivo com sucesso, envia uma msg com o tipo sucesso para 
			acabou=1;
			msg.tipo==SUCESSO;
			msg_bin = MensagemToMensagem_bin(msg);
            envia_mensagem_bin (s, &msg_bin);
		}
		else
		{
			recebe_mensagem_bin (s, &msg_bin);
			msg = Mensagem_binToMensagem(msg_bin);
			countmsg++;
		}
	}
	recebe_mensagem_bin(s, &msg_bin);
    msg = Mensagem_binToMensagem(msg_bin);
    FILE *dest;
	if (msg.tipo!=ERRO)
    {
        if (msg.tipo==TAMARQ)
            printf("Tamanho do arquivo: %uB.\n", msg.dados[0]);
        
        recebe_mensagem_bin(s, &msg_bin);
        msg = Mensagem_binToMensagem(msg_bin);
        if ((dest = fopen (filename,"w+b"))==NULL)
        {
            printf ("Erro ao abrir ou criar o arquivo "); puts (filename);
        }
        while (msg.tipo!=FIMTXT)
        {
            if (fwrite (msg.dados, 1, msg.tamanho, dest)!=msg.tamanho)
                puts ("Erro na escrita em arquivo.");
            recebe_mensagem_bin(s, &msg_bin);
            msg = Mensagem_binToMensagem(msg_bin);
        }
        puts ("Arquivo copiado com sucesso.");
        unsigned char mode[] = "0777";
        chmod (filename,strtol(mode, 0, 8));
        fclose (dest);
    }
    else
        puts ("Erro ao receber arquivo.");
    free (filename);
}

void EnviaDirAtual (int socket, mensagem msg)
{
	mensagem_bin msg_bin;
	unsigned char *dir; unsigned char erro[]="Nao foi possivel localizar o diretorio atual.";
    dir = (unsigned char *) (long int) get_current_dir_name();
    unsigned char *enviar;
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