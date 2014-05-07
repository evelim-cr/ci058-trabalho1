#include "protocolo.h"

extern int conexao; //armazena o file descriptor do socket
extern int proximo_quadro_enviar;
extern int proximo_quadro_receber;



/******************************************************************************/
void envia_mensagem(mensagem *m){
	int tentativa = 0;
	event_type evento;	
	quadro *q = monta_quadro(m);
	
	do {
		envia_quadro(q);
		espera_por_evento(&evento);
		if (evento==nack_arrival){
			tentativa = 0;
		}
	} while ((evento!=ack_arrival)&&(++tentativa<16));
	

	if (evento==ack_arrival) {
		proximo_quadro_enviar = incrementa(proximo_quadro_enviar);
	} else {
		printf ("\033[31;1mRede sem conexão! (O sistema será fechado automaticamente)\033[m\n");
		exit(-1);
	}
}
/******************************************************************************/






/******************************************************************************/
void espera_por_evento(event_type *evento){
	struct pollfd p;
	p.fd = conexao;
	p.events = POLLIN;

	int resultPoll = poll(&p,1,1000);

	switch(resultPoll){
	
		case 0: { //timeout
			printf("timeout!!\n");
			*evento = timeout;
			break;
		}

		case -1: { //erro
			printf("Erro ao fazer poll\n");
			exit(-1);
			break;
		}

		default: {
			if (p.revents==POLLIN) {
				quadro *q = (quadro *) calloc(1,sizeof(quadro));
				if (recv(conexao,(void *)q,(ssize_t)QUADRO,0) == (ssize_t)QUADRO) {
					if (possui_erro_quadro(q)){
						*evento = cksum_err;
					} else {
						mensagem *m = desmonta_quadro(q);
						if ((m->tipo==ACK)&&(m->dados[0]==incrementa(proximo_quadro_enviar))){
							*evento = ack_arrival;
						} else if ((m->tipo==NACK)&&(m->dados[0]==proximo_quadro_enviar)){
							*evento = nack_arrival;
						} else {
							*evento = lixo;
						}
						free(m);
					}
				} else {
					*evento = lixo;
				}
				free(q);
			}
			break;
		}
	}	
}
/******************************************************************************/







/******************************************************************************/
void envia_quadro(quadro *q){
	send(conexao,(void *)q,(ssize_t)QUADRO,0);
}
/******************************************************************************/






/******************************************************************************/
mensagem *recebe_mensagem(){
	quadro *q = proximo_quadro();	
	mensagem *m = desmonta_quadro(q);
	free(q);
	return m;
}
/******************************************************************************/
quadro *proximo_quadro(){
	quadro *q = (quadro *) malloc(sizeof(quadro));
	while (1){	
		bzero((void *)q,(ssize_t)QUADRO);
		if (recv(conexao,(void *)q,(ssize_t)QUADRO,0)==(ssize_t)QUADRO){
			if (eh_quadro_esperado(q)) {
				if (possui_erro_quadro(q)) {
					envia_nack();
				} else {
					proximo_quadro_receber = incrementa(proximo_quadro_receber);
					envia_ack();
					return q;
				}
			} else {
				//envia ack pois o quadro jah foi recebido
				envia_ack();
			}
		}
	}
}
/******************************************************************************/








//VERIFICA SE A SEQUENCIA DO QUADRO EH IGUAL A SEQUENCIA DO QUADRO ESPERADO
//RETORNA 1 SE VERDADEIRO, 0 SE FALSO
/******************************************************************************/
int eh_quadro_esperado(quadro *q){
	int sequencia = binToint(q->sequencia,SEQUENCIA);
	if (sequencia == proximo_quadro_receber){
		return 1;
	}
	return 0;
}
/******************************************************************************/
int incrementa(int i){
	if (i == MAX_SEQUENCIA){
		return 0;
	} else {
	    return i+1;
	}
}
/******************************************************************************/







/******************************************************************************/
void envia_ack(){
	envia_ack_nack(ACK);
}
/******************************************************************************/
void envia_nack(){
	envia_ack_nack(NACK);
}
/******************************************************************************/
void envia_ack_nack(int tipo){
	quadro *q = (quadro *) calloc(1,sizeof(quadro));
	intTobin(tipo, TIPO, q->tipo);	
	intTobin(1, TAMANHO_DADOS, q->tamanhoDados);
	q->dados[0] = proximo_quadro_receber;
	calcula_crc(q);	
	envia_quadro(q);
	free(q);
}
/******************************************************************************/







/******************************************************************************/
quadro *monta_quadro(mensagem *m){
	int i;
	quadro *q = (quadro *) calloc(1,sizeof(quadro));
	intTobin(m->tipo, TIPO, q->tipo);	
	intTobin(proximo_quadro_enviar, SEQUENCIA, q->sequencia);	
	intTobin(m->tamanhoDados, TAMANHO_DADOS, q->tamanhoDados);
	//copiando dados da mensagem para o quadro
	for (i=0;i<m->tamanhoDados;i++)	q->dados[i] = m->dados[i];
	calcula_crc(q);	
	return q;
}
/******************************************************************************/
mensagem *desmonta_quadro(quadro *q){
	mensagem *m = (mensagem *) malloc(sizeof(mensagem));
	m->tipo = binToint(q->tipo,TIPO);
	m->tamanhoDados = binToint(q->tamanhoDados,TAMANHO_DADOS);
	
	//extraindo dados do quadro
	unsigned char *dados = (unsigned char *) malloc(sizeof(unsigned char) * (m->tamanhoDados+1));
	int i;
	for (i=0;i<m->tamanhoDados;i++) {
		dados[i] = q->dados[i];
	}
	dados[m->tamanhoDados]='\0';	
	m->dados = dados;
	
	return m;
}
/******************************************************************************/







/******************************************************************************/
void calcula_crc(quadro *q){
	int i;
	int qtdBytesDados = binToint(q->tamanhoDados,TAMANHO_DADOS);
	int qtdBitsDados = qtdBytesDados*BYTE;
	int qtdBitsDividendo = qtdBitsDados+GRAU_POLINOMIO_CRC;
	binario *dividendo = calloc(qtdBitsDividendo,sizeof(binario));
	binario *dados = malloc(sizeof(binario)*qtdBitsDados);	
	//convertendo caracteres para binario
	strTobin(q->dados, qtdBytesDados, dados);
	//copiando bits de dados para dividendo
	for (i=0;i<qtdBitsDados;i++) dividendo[i] = dados[i];	
	//calculando o resto da divisao dos dados+zeros pelo polinomio
	binario *resto = resto_crc(dividendo,qtdBitsDividendo);
	//copiando resto para q->crc
	for (i=0;i<GRAU_POLINOMIO_CRC;i++) q->crc[i] = resto[i];
	free(resto);
	free(dividendo);
	free(dados);
}
/******************************************************************************/







/******************************************************************************/
binario *resto_crc(binario *dividendo, int qtdBits){
	//polinomio gerador do crc8
	binario polinomio[GRAU_POLINOMIO_CRC+1] = {1,0,0,0,0,0,1,1,1};
	//fazendo a divisão polinomial
	int i, j;
	for (i=0;i<qtdBits-GRAU_POLINOMIO_CRC;i++) {
		if (dividendo[i] == 1) {
			for (j=0;j<=GRAU_POLINOMIO_CRC;j++) {
				dividendo[j+i] = xorBin(dividendo[j+i],polinomio[j]);
			}
		}
	}
	//pegando o resto da divisão
	binario *resto = (binario *) malloc(sizeof(binario)*GRAU_POLINOMIO_CRC);
	for (i=0;i<GRAU_POLINOMIO_CRC;i++) {
		resto[i] = dividendo[qtdBits-GRAU_POLINOMIO_CRC+i];	
	}
	return resto;
}
/******************************************************************************/








/******************************************************************************/
int possui_erro_quadro(quadro *q){
	int i;
	int qtdBytesDados = binToint(q->tamanhoDados,TAMANHO_DADOS); 
	int qtdBitsDados = qtdBytesDados*BYTE;
	int qtdBitsDividendo = qtdBitsDados+GRAU_POLINOMIO_CRC;
	binario *dividendo = calloc(1,sizeof(binario)*(qtdBitsDividendo));	
	binario *dados = malloc(sizeof(binario)*(qtdBitsDados));
	//convertendo caracteres para binario
	strTobin(q->dados, qtdBytesDados, dados);
	//copiando bits de dados para dividendo
	for (i=0;i<qtdBitsDados;i++) dividendo[i] = dados[i];
	//adicionando crc ao dividendo
	for (i=0;i<GRAU_POLINOMIO_CRC;i++) dividendo[i+qtdBitsDados] = q->crc[i];
	//calculando o resto da divisao dos dados+crc pelo polinomio
	binario *resto = resto_crc(dividendo,qtdBitsDividendo);
	int check = binToint(resto,GRAU_POLINOMIO_CRC);
	free(resto);
	free(dividendo);
	free(dados);
	return check;
}
/******************************************************************************/








/******************************************************************************/
void imprime_quadro(quadro *q){
	printf("CRC:\n");	
	imprimeBinario(q->crc,GRAU_POLINOMIO_CRC);
	printf("TIPO:\n");	
	imprimeBinario(q->tipo,TIPO);
	printf("SEQUENCIA:\n");	
	imprimeBinario(q->sequencia,SEQUENCIA);
	printf("TAMANHO_DADOS:\n");	
	imprimeBinario(q->tamanhoDados,TAMANHO_DADOS);
	printf("DADOS:\n");	
	printf("%s\n\n-------------------------------\n",q->dados);
}
/******************************************************************************/
void imprime_mensagem(mensagem *m){
	printf("TIPO:%d\n",m->tipo);
	printf("TAMANHO:%d\n",m->tamanhoDados);
	printf("DADOS:\n");	
	if ((m->tipo==ACK)||(m->tipo==NACK)){
		printf("%d\n-------------------------------\n",m->dados[0]);
	} else {
		printf("%s\n-------------------------------\n",m->dados);	
	}
}
/******************************************************************************/
