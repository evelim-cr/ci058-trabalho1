#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <poll.h>

#define TAMINICIOBIN 8
#define TAMTAMANHOBIN 4
#define TAMSEQUENCIABIN 4
#define TAMTIPOBIN 4
#define TAMERROBIN 4
#define TAMDADOSBIN 120

typedef struct
{
	unsigned char inicio[TAMINICIOBIN];
	unsigned char tamanho[TAMTAMANHOBIN];
	unsigned char sequencia[TAMSEQUENCIABIN];
	unsigned char tipo[TAMTIPOBIN];
	unsigned char erro[TAMERROBIN];
	unsigned char dados[TAMDADOSBIN];
} mensagem_bin;

typedef struct
{
	int tamanho;
	int tipo;
	int sequencia;
	unsigned char dados[15];
} mensagem;

#define TAMMSG 144

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
void recebe_mensagem_bin (int socket, mensagem_bin *msg_bin, int seq);
void EnviaArq(int s, unsigned char * path, int type, int *seq);
void InsereParidade (mensagem_bin *msg_bin);
int TemErro (mensagem_bin msg_bin);
void envia_acknack (int socket, mensagem_bin *acknack);
void recebe_acknack (int socket, int *resposta);
void LimpaSocket (int socket);

#define TIMEOUT 20
#define LIXO	21
#define SYNCSEQ	22