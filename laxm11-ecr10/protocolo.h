#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

typedef struct
{
	char inicio[8];
	char tamanho[4];
	char sequencia[4];
	char tipo[4];
	char erro[4];
	char dados[2];
} mensagem_bin;

typedef struct
{
	int tamanho;
	int tipo;
	char dados[2];
} mensagem;

#define TAMMSG 26
#define CD 0
#define LS 1
#define PUT 2
#define GET 3
#define CAT 4
#define DIR_ATUAL 5
#define DADOS 7
#define TAMARQ 8
#define MOSTRA 9
#define ACK 10
#define FIMTXT 11
#define SUCESSO 13
#define ERRO 14
#define NACK 15

mensagem_bin MensagemToMensagem_bin (mensagem msg);
mensagem Mensagem_binToMensagem (mensagem_bin msg_bin);
void envia_mensagem_bin (int socket, mensagem_bin *msg_bin);
void recebe_mensagem_bin (int socket, mensagem_bin *msg_bin);
void EnviaArq(int s, char * path, int type);