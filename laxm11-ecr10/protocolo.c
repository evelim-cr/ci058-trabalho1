#include "protocolo.h"
#include "funcoes.h"

mensagem_bin MensagemToMensagem_bin (mensagem msg)
{
	mensagem_bin mbin;
	strcpy (mbin.inicio,"1110");
	intTobin(msg.tamanho,4,mbin.tamanho);
	strcpy (mbin.sequencia,"0000");
	intTobin(msg.tipo, 4, mbin.tipo);
	strcpy (mbin.erro,"0000");
	intTobin(msg.dados[0], 8, mbin.dados);
	intTobin(msg.dados[1], 8, &(mbin.dados[8]));
	return mbin;
}

mensagem Mensagem_binToMensagem (mensagem_bin msg_bin)
{
	int i;
	mensagem msg;
	msg.tamanho = binToint(msg_bin.tamanho,4);
	msg.tipo = binToint(msg_bin.tipo, 4);
	for (i = 0; i < msg.tamanho; i++)
		msg.dados[i] = binToint(&(msg_bin.dados[i*8]),8);
	return msg;
}

void envia_string (int socket, char *string)
{
	if (write (socket, string, strlen(string)+1)==(strlen(string)+1))
		puts ("Acho que deu certo...");
	else
		puts ("Erro ao enviar string");
}

void recebe_string (int socket, char *string)
{
	if (recv (socket, string, (strlen(string)+1),0)==(strlen(string)+1))
		puts ("Acho que deu certo...");
	else
		puts ("Erro ao receber string");
}

void envia_mensagem_bin (int socket, mensagem_bin *msg_bin)
{
	if (write (socket, msg_bin, TAMMSG)==TAMMSG)
		puts ("Acho que deu certo...");
	else
		puts ("Erro ao enviar mensagem");
}

void recebe_mensagem_bin (int socket, mensagem_bin *msg_bin)
{

	while ( (recv (socket, msg_bin, TAMMSG,0)!=TAMMSG) && (strcmp(msg_bin->inicio, "1110")!=0) );
}