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
	if (send (socket, msg_bin, TAMMSG,0)!=TAMMSG)
		puts ("Erro ao enviar mensagem");
}

void recebe_mensagem_bin (int socket, mensagem_bin *msg_bin)
{

	while ( (recv (socket, msg_bin, TAMMSG,0)!=TAMMSG) && (strcmp(msg_bin->inicio, "11101110")!=0) );
	//puts ("Mensagem recebida.");
}

void EnviaArq(int s, char * path, int type)
{
    FILE *fp;
    mensagem msg;
    mensagem_bin msg_bin;
    puts ("\tAbrindo arquivo para envio.");	//log
    if ((fp = fopen (path,"rb"))==NULL)
    {
        printf ("\tArquivo nao existe.\n");	//log
        msg.tipo=ERRO;
        msg.tamanho=0;
        bzero (msg.dados,2);
        msg_bin = MensagemToMensagem_bin(msg);
        envia_mensagem_bin (s, &msg_bin);
        return;
    }

    if ( (type==GET) || (type==PUT) )
    {
	    fseek(fp, 0L, SEEK_END);
	    unsigned int tamfp = ftell(fp);	// fazer barra de progresso
	    int i=0;
	    rewind (fp);
	    msg.tipo = TAMARQ;
	    msg.tamanho = 1;
	    msg.dados[0] = tamfp;
	    msg_bin = MensagemToMensagem_bin(msg);
	    envia_mensagem_bin(s, &msg_bin);
	    printf("\tTamanho do arquivo: %uB.\n", tamfp);
    }

    msg.tipo=MOSTRA;
    msg.tamanho=0;
    bzero (msg.dados,2);
    int i=0;
	puts ("\tInicio da transferencia do arquivo.");	//log
    while (!feof(fp))
    {
        if (msg.tamanho<2)
            fread (&(msg.dados[(msg.tamanho)++]), sizeof(char), 1, fp);
        if( (msg.tamanho == 2) || (feof(fp)) ){
            if (feof(fp))
                msg.tipo=FIMTXT;
            msg_bin = MensagemToMensagem_bin(msg);
            envia_mensagem_bin (s, &msg_bin);
            i++;
            msg.tamanho=0;
            bzero (msg.dados,2);
        }
    }
    printf ("\t[%d mensagens enviadas]\n\tFim da transferencia do arquivo.\n", i);	//log
    fclose(fp); 
}