#include "protocolo.h"

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