#include "protocolo.h"
#include "funcoes.h"

mensagem_bin MensagemToMensagem_bin (mensagem msg)
{
	mensagem_bin mbin;
	strcpy (mbin.inicio,"11101110");
	intTobin(msg.tamanho,4,mbin.tamanho);
	strcpy (mbin.sequencia,"0000");
	intTobin(msg.tipo, 4, mbin.tipo);
	strcpy (mbin.erro,"0000");
	mbin.dados[0]=msg.dados[0];
	mbin.dados[1]=msg.dados[1];
	// intTobin(msg.dados[0], 8, mbin.dados);
	// intTobin(msg.dados[1], 8, &(mbin.dados[8]));
	return mbin;
}

mensagem Mensagem_binToMensagem (mensagem_bin msg_bin)
{
	int i;
	mensagem msg;
	msg.tamanho = binToint(msg_bin.tamanho,4);
	msg.tipo = binToint(msg_bin.tipo, 4);
	// for (i = 0; i < msg.tamanho; i++)
	// 	msg.dados[i] = binToint(&(msg_bin.dados[i*8]),8);
	msg.dados[0]=msg_bin.dados[0];
	msg.dados[1]=msg_bin.dados[1];
	return msg;
}

void envia_mensagem_bin (int socket, mensagem_bin *msg_bin)
{
	if (write (socket, msg_bin, TAMMSG)==TAMMSG)
		puts ("Mensagem enviada.");
	else
		puts ("Erro ao enviar mensagem");
}

void recebe_mensagem_bin (int socket, mensagem_bin *msg_bin)
{

	while ( (recv (socket, msg_bin, TAMMSG,0)!=TAMMSG) && (strcmp(msg_bin->inicio, "11101110")!=0) );
	//puts ("Mensagem recebida.");
}

void EnviaArq(int s, char * path)
{ 
    FILE *fp;
    mensagem msg;
    mensagem_bin msg_bin; 
    fp = fopen(path,"rb");// Abre o arquivo para leitura
    msg.tamanho=0;
    msg.tipo=MOSTRA;
    bzero (msg.dados,2);
    if (fp == NULL) // se o arquivo nao existir exibe a mensagem de erro.
    {
        printf("Houve um erro ao abrir o arquivo\n");
        exit(1);
    }
    else
    {
        while (!feof(fp))
        {
            if (msg.tamanho<2)
                fread (&(msg.dados[(msg.tamanho)++]), sizeof(char), 1, fp);
            if( (msg.tamanho == 2) || (feof(fp)) ){
                if (feof(fp))
                    msg.tipo=FIMTXT;
                msg_bin = MensagemToMensagem_bin(msg);
                envia_mensagem_bin (s, &msg_bin);
                msg.tamanho=0;
                bzero (msg.dados,2);
            }
        }
    }
    fclose(fp); 
}