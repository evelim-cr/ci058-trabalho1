#include "protocolo.h"
#include "funcoes.h"

mensagem_bin MensagemToMensagem_bin (mensagem msg)
{
    int i;
	mensagem_bin mbin;
    memset(mbin.inicio, 1, 8);
    mbin.inicio[3]=0;
    mbin.inicio[7]=0;
	intTobin(msg.tamanho,TAMTAMANHOBIN,mbin.tamanho);
    intTobin(msg.sequencia,TAMSEQUENCIABIN,mbin.sequencia);
	intTobin(msg.tipo, TAMTIPOBIN, mbin.tipo);
	// mbin.dados[0]=msg.dados[0];
	// mbin.dados[1]=msg.dados[1];
    for (i = 0; i < msg.tamanho; i++)
	   intTobin(msg.dados[i], 8, &(mbin.dados[i*8]));
    InsereParidade (&mbin);
	return mbin;
}

mensagem Mensagem_binToMensagem (mensagem_bin msg_bin)
{
	int i;
	mensagem msg;
	msg.tamanho = binToint(msg_bin.tamanho,TAMTAMANHOBIN);
	msg.tipo = binToint(msg_bin.tipo, TAMTIPOBIN);
	for (i = 0; i < msg.tamanho; i++)
	 	msg.dados[i] = binToint(&(msg_bin.dados[i*8]),8);
	// msg.dados[0]=msg_bin.dados[0];
	// msg.dados[1]=msg_bin.dados[1];
	return msg;
}

void envia_mensagem_bin (int socket, mensagem_bin *msg_bin)
{
    mensagem msg;
    char inicio[8];
    memset(inicio, 1, 8);
    inicio[3]=0;
    inicio[7]=0;
    int resposta=NACK, tentativa=0;
    do{
        if ((send (socket, msg_bin, TAMMSG,0)==TAMMSG) && (memcmp(msg_bin->inicio, inicio,8)==0))
            recebe_acknack(socket, &resposta);
        else
            resposta=NACK;

        if (resposta==TIMEOUT)
            printf ("%d\n",tentativa++);
        else if (resposta==SYNCSEQ)
            break;
        else
            tentativa=0;    //LIXO OU NACK

    } while( (tentativa<16) && ((resposta==NACK) || (resposta==TIMEOUT) || resposta==LIXO) );
    if (tentativa >= 16)
    {
        printf("Encerrando...\n"NRM);
        exit(-1);
    }

}

void recebe_mensagem_bin (int socket, mensagem_bin *msg_bin, int seq)
{
    mensagem msg;
    mensagem_bin acknack;
    char inicio[8];
    memset(inicio, 1, 8);
    inicio[3]=0;
    inicio[7]=0;
    while (1)
    {
        if ( (recv (socket, msg_bin, TAMMSG,0)==TAMMSG) && ((memcmp(msg_bin->inicio, inicio,8))==0))
        {
            if (TemErro(*msg_bin))
            {
                msg.tipo=NACK;
                msg.tamanho=0;
                bzero(msg.dados,15);
                acknack = MensagemToMensagem_bin(msg);
                envia_acknack (socket, &acknack);
            }
            else
            {
                if (EhSequenciaEsperada(msg_bin->sequencia,seq))
                {
                    msg.tipo=ACK;
                    msg.tamanho=0;
                    bzero(msg.dados,15);
                    acknack = MensagemToMensagem_bin(msg);
                    envia_acknack (socket, &acknack);
                    break;
                }
                else
                {
                    msg.tipo=NACK;
                    msg.tamanho=sizeof(int);
                    memcpy (msg.dados, &seq, sizeof(int));
                    acknack = MensagemToMensagem_bin(msg);
                    envia_acknack (socket, &acknack);
                }
            }
        }
    }
}

void envia_acknack (int socket, mensagem_bin *acknack)
{
    if (send (socket, acknack, TAMMSG,0)!=TAMMSG)
            puts ("Erro ao enviar ACK ou NACK");
}

void recebe_acknack (int socket, int *resposta)
{
    mensagem msg;
    mensagem_bin msg_bin;

    char inicio[8];
    memset(inicio, 1, 8);
    inicio[3]=0;
    inicio[7]=0;

    struct pollfd p;
    p.fd = socket;
    p.events = POLLIN;

    int rPoll = poll(&p,1,1000);

    switch(rPoll)
    {
        case 0:
        {
            printf(RED"Timeout! "NRM);
            (*resposta)=TIMEOUT;
            break;
        }
        case -1:
        {
            printf("Erro no poll\n");
            exit(-1);
            break;
        }
        default:
        {
            if (p.revents==POLLIN)
            {
                if ((recv (socket, &msg_bin, TAMMSG,0)==TAMMSG) && ((memcmp(msg_bin.inicio, inicio,8))==0))
                {
                    if (TemErro(msg_bin))
                        (*resposta)=NACK;
                    else
                    {
                        msg = Mensagem_binToMensagem(msg_bin);
                        if (msg.tipo==NACK)
                            if (msg.tamanho==sizeof(int))
                                (*resposta)=SYNCSEQ;
                            else
                                (*resposta=NACK);
                        else
                            (*resposta)=ACK;
                    }
                }
                else
                    (*resposta)=LIXO;
            }
            break;
        }
    }
}

void EnviaArq(int s, unsigned char * path, int type, int *seq)
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
        msg.sequencia=*seq;
        bzero (msg.dados,15);
        msg_bin = MensagemToMensagem_bin(msg);
        envia_mensagem_bin (s, &msg_bin);
        return;
    }
    struct stat st;
    if (stat(path, &st))
        printf("\tERRO ao tentar obter stat de %s\n", path);
    int fptam = (int) st.st_size;
    if ( (type==GET) || (type==PUT) )   // se eh GET ou PUT, envia mensagem com tamanho do arquivo.
    {
        msg.tipo = TAMARQ;
        msg.tamanho = 8;
        msg.sequencia = *seq;
        incrementa_sequencia(seq);
        memcpy(msg.dados,&fptam,8);
	    msg_bin = MensagemToMensagem_bin(msg);
	    envia_mensagem_bin(s, &msg_bin);
	    printf("\tTamanho do arquivo: %d Bytes.\n", fptam);
    }

    msg.tipo=MOSTRA;
    msg.tamanho=0;
    bzero (msg.dados,15);
    int i=0;
	puts ("\tInicio da transferencia do arquivo.");	//log
    while (!feof(fp))
    {
        if (msg.tamanho<15)
            fread (&(msg.dados[(msg.tamanho)++]), sizeof(unsigned char), 1, fp);
        if( (msg.tamanho == 15) || (feof(fp)) ){
            loadBar(i,(fptam/15),fptam,50);
            if (feof(fp))
                msg.tipo=FIMTXT;
            msg.sequencia=*seq;
            incrementa_sequencia(seq);
            msg_bin = MensagemToMensagem_bin(msg);
            envia_mensagem_bin (s, &msg_bin);
            i++;
            msg.tamanho=0;
            bzero (msg.dados,15);
        }
    }
    printf ("\t[%d mensagens enviadas]\n\tFim da transferencia do arquivo.\n", i);	//log
    fclose(fp); 
}

void InsereParidade (mensagem_bin *msg_bin)
{
    int i,j;
    for (i = 0; i < 4; i++)
        msg_bin->erro[i] = (msg_bin->tamanho[i]) ^ (msg_bin->tipo[i]) ^ (msg_bin->sequencia[i]);
    unsigned char paridadeDados[4];
    unsigned char temp[4];
    bzero (paridadeDados,4);
    for (i = 0; i < TAMDADOSBIN; i++)
    {
        if ((i%4==0) && (i!=0))
        {
            for (j=0;j<4;j++)
                paridadeDados[j]=paridadeDados[j] ^ temp[j];
            bzero(temp,4);
            temp[i%4] = msg_bin->dados[i];
        }
        else
            temp[i%4] = msg_bin->dados[i];
    }
    for (j=0;j<4;j++)
    {
        paridadeDados[j]=paridadeDados[j] ^ temp[j];
        msg_bin->erro[j]=!(msg_bin->erro[j] ^ paridadeDados[j]);
    }
}

int TemErro (mensagem_bin msg_bin)
{
    int i,j;
    char erro[4];
    for (i = 0; i < 4; i++)
        erro[i] = (msg_bin.tamanho[i]) ^ (msg_bin.tipo[i]) ^ (msg_bin.sequencia[i]);
    unsigned char paridadeDados[4];
    unsigned char temp[4];
    bzero (paridadeDados,4);
    for (i = 0; i < TAMDADOSBIN; i++)
    {
        if ((i%4==0) && (i!=0))
        {
            for (j=0;j<4;j++)
                paridadeDados[j]=paridadeDados[j] ^ temp[j];
            bzero(temp,4);
            temp[i%4] = msg_bin.dados[i];
        }
        else
            temp[i%4] = msg_bin.dados[i];
    }
    for (j=0;j<4;j++)
    {
        paridadeDados[j]=paridadeDados[j] ^ temp[j];
        erro[j]=!(erro[j] ^ paridadeDados[j]) ^ msg_bin.erro[j];
        if (erro[j]!=0)
            return 1;
    }
    return 0;                           //nao tem erro
}

void LimpaSocket (int socket)
{
    struct pollfd p;
    p.fd = socket;
    p.events = POLLIN;
    mensagem_bin lixo;
    int i=0;
    int rPoll;
    rPoll = poll(&p,1,0);
    while ( p.revents==POLLIN )
    {
        recv(socket, &lixo, TAMMSG,0);
        rPoll = poll(&p,1,0);
    }
}