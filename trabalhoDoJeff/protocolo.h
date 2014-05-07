#include "funcoes.h"

#include <poll.h>
#include <strings.h>


typedef enum {ack_arrival,nack_arrival,cksum_err,timeout,lixo}event_type;

#define MAX_SEQUENCIA 15

#define TAMANHO_DADOS 8
#define TIPO 4
#define SEQUENCIA 4
#define GRAU_POLINOMIO_CRC 8
#define DADOS 255
#define QUADRO TAMANHO_DADOS+TIPO+SEQUENCIA+GRAU_POLINOMIO_CRC+DADOS


#define ACK 1
#define NACK 2
#define LS 3
#define CD 4
#define MOSTRAR_TELA 5
#define FIM_TEXTO 6
#define REQUEST_DIR_ATUAL 7
#define REQUEST_ARQUIVO_EXISTE 8
#define REQUEST_NEXT_PART_OF_FILE 9
#define FIM_ARQUIVO 10
#define ABORTAR_TRANSMISSAO 11
#define ERRO 15


typedef struct {
	binario tamanhoDados[TAMANHO_DADOS];
	binario tipo[TIPO];
	binario sequencia[SEQUENCIA];
	binario crc[GRAU_POLINOMIO_CRC];
	unsigned char dados[DADOS];
}quadro;


typedef struct {
	int tipo;
	int tamanhoDados;
	unsigned char *dados;
}mensagem;


void calcula_crc(quadro *q);
void envia_mensagem(mensagem *m);
void envia_quadro(quadro *q);
void envia_ack();
void envia_nack();
void envia_ack_nack(int tipo);
void espera_por_evento(event_type *evento);
void imprime_quadro(quadro *q);
void imprime_mensagem(mensagem *m);

int incrementa(int i);
int possui_erro_quadro(quadro *q);
int eh_quadro_esperado(quadro *q);

mensagem *recebe_mensagem();
mensagem *desmonta_quadro(quadro *q);

quadro *monta_quadro(mensagem *m);
quadro *proximo_quadro();

binario *resto_crc(binario *dividendo, int qtdBits);
