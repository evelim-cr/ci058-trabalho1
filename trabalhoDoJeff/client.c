#include "conexao.h"
#include "protocolo.h"
#include "arquivo.h"
#include <string.h>
#include <stdlib.h>


#define DEFAULT_OPTION 0;
#define LLS 49
#define RLS 50
#define LCD 51
#define RCD 52
#define COMPARE 53


int conexao;
int proximo_quadro_enviar = 0;
int proximo_quadro_receber = 0;
FILE *arquivo_comparado = NULL;

unsigned char *diretorio_local_atual = "";
unsigned char *diretorio_remoto_atual = "";

void rls();
void rcd();
void lls();
void lcd();
void mostra_opcoes();
void compare();
void compara_arquivos();
void abre_arquivo_local(unsigned char *nomeArquivo);
void request_abortar_transmissao();
int le_opcao();
int request_existe_arquivo_remoto(unsigned char *nomearquivo, int tamNomeArq);
int possui_diferenca(unsigned char *arqA, unsigned char *arqB, int tamArqA, int tamArqB);
unsigned char *request_diretorio_remoto_atual();
unsigned char *proxima_parte_arquivo_local(int *tam);
unsigned char *request_proxima_parte_arquivo_remoto(int *tam);



int main(){
	conexao = criaConexao();
	diretorio_local_atual = diretorio_atual();
	diretorio_remoto_atual = request_diretorio_remoto_atual();

	while (1) {
		mostra_opcoes();
		switch (le_opcao()) {
			limpa_tela();
			case LLS: { lls(); break; }				
			case RLS: { rls(); break; }
			case LCD: { lcd(); break; }
			case RCD: { rcd(); break; }
			case COMPARE: { compare(); break; }
			default: { printf("\033[31;1mOpcao Invalida!!\033[m\n"); break; }
		}
	}
}



//LS remoto
void rls(){
	mensagem *m = (mensagem *) malloc(sizeof(mensagem));
	printf("\033[36;1mDigite as opções do LS REMOTO: \033[m");
	m->dados = (unsigned char *) read_keyboard(&m->tamanhoDados);
	m->tipo = LS;
	envia_mensagem(m);
	m = recebe_mensagem();
	while (m->tipo != FIM_TEXTO) {
		printf("%s",m->dados);
		m = recebe_mensagem();
	}
	free(m);
}




//CD remoto
void rcd(){
	mensagem *m = (mensagem *) malloc(sizeof(mensagem));
	printf("\033[36;1mDigite um path para o CD REMOTO: \033[m");
	m->dados = (unsigned char *) read_keyboard(&m->tamanhoDados);
	m->tipo = CD;
	envia_mensagem(m);
	m = recebe_mensagem();
	if (m->tipo == ERRO) {
		trata_erro(m->dados[0]);
	} else {
		diretorio_remoto_atual = request_diretorio_remoto_atual();
	}
	free(m);
}




//CD local
void lcd(){
	int tam;
	printf("\033[36;1mDigite um path para o CD LOCAL: \033[m");
	unsigned char *path = (unsigned char *) read_keyboard(&tam);
	int erro = cd(path);
	if (erro){
		trata_erro(erro);
	} else {
		diretorio_local_atual = diretorio_atual();
	}
	free(path);
}




//LS local
void lls(){
	int tam;
	printf("\033[36;1mDigite as opções do LS LOCAL: \033[m");
	unsigned char *aux = (unsigned char *) read_keyboard(&tam);
	FILE *f = ls(aux,tam);
	if (f != NULL){
		aux = readFile(f,&tam,DADOS);
		while (tam > 0) {
			printf("%s",aux);
			aux = readFile(f,&tam,DADOS);
		}
		
		free(aux);
		fclose(f);
		remove_temp_file();
	}
}




void mostra_opcoes(){
	printf("===========================================================================\n");
	printf("||\033[34;1m Diretório LOCAL atual:\033[m \033[37;1m %s \033[m\n",diretorio_local_atual);  
	printf("||\033[34;1m Diretório REMOTO atual:\033[m \033[37;1m %s \033[m\n",diretorio_remoto_atual);
	printf("---------------------------------------------------------------------------\n");
	printf("||\033[34;1m Escolha uma opção:\033[m\n");
	printf("||\033[37;1m 1 - ls local\033[m\n");
	printf("||\033[37;1m 2 - ls remoto\033[m\n");	
	printf("||\033[37;1m 3 - cd local\033[m\n");
	printf("||\033[37;1m 4 - cd remoto\033[m\n");
	printf("||\033[37;1m 5 - compare\033[m\n");
	printf("===========================================================================\n");
}



int le_opcao(){
	int tam;
	unsigned char *aux = (unsigned char *) read_keyboard(&tam);
	if (tam == 1) {
		return aux[0];
	}
	return DEFAULT_OPTION;
}




void compare(){
	int tamArqLocal, tamArqRemoto;
	printf("\033[36;1mDigite o nome do arquivo LOCAL a ser comparado: \033[m");
	unsigned char *arqLocal = (unsigned char *) read_keyboard(&tamArqLocal);

	if (!arquivo_existe(arqLocal)) {
		trata_erro(ERRO_ARQUIVO_INEXISTENTE);
	} else {
		printf("\033[36;1mDigite o nome do arquivo REMOTO a ser comparado: \033[m");
		unsigned char *arqRemoto = (unsigned char *) read_keyboard(&tamArqRemoto);	
		if (!request_existe_arquivo_remoto(arqRemoto,tamArqRemoto)) {
			trata_erro(ERRO_ARQUIVO_INEXISTENTE);
		} else {
			abre_arquivo_local(arqLocal);
			compara_arquivos();
		}
	}	
}




void abre_arquivo_local(unsigned char *nomeArquivo){
	arquivo_comparado = fopen(nomeArquivo,"r");
}




void compara_arquivos(){
	int tam_parte_local, tam_parte_remota;
	unsigned char *parte_local = proxima_parte_arquivo_local(&tam_parte_local);
	unsigned char *parte_remoto = request_proxima_parte_arquivo_remoto(&tam_parte_remota);
	while ((tam_parte_local>0)&&(tam_parte_remota>0)&&(!possui_diferenca(parte_local,parte_remoto,tam_parte_local,tam_parte_remota))) {
		parte_local = proxima_parte_arquivo_local(&tam_parte_local);
		parte_remoto = request_proxima_parte_arquivo_remoto(&tam_parte_remota);
	}
	
	if ((tam_parte_local==0)&&(tam_parte_remota==0)) { //terminou ambos arquivos e sem diferenca
		printf("\033[39;1mArquivos identicos!\033[m\n");
	} else
	if ((tam_parte_local==0)&&(tam_parte_remota>0)) { //arquivo remoto tem mais dados que arquivo local
		printf("\033[31;1mARQUIVOS DIFERENTES! Arquivo REMOTO tem mais dados que arquivo LOCAL!\033[m\n");
		printf("Diferença na mensagem:\n\033[32;1m%s\033[m\n",parte_remoto);
	} else
	if ((tam_parte_local>0)&&(tam_parte_remota==0)) { //arquivo local tem mais dados que arquivo remoto
		printf("\033[31;1mARQUIVOS DIFERENTES! Arquivo LOCAL tem mais dados que arquivo REMOTO!\033[m\n");
		printf("Diferença no arquivo LOCAL:\n\033[32;1m%s\033[m\n",parte_local);
	} else { //encontrou diferenca no meio dos arquivos
		printf("\033[31;1mARQUIVOS DIFERENTES!\033[m\n");
		printf("Arquivo Remoto:\n\033[32;1m%s\033[m\n----------------------\n",parte_remoto);	
		printf("Arquivo Local:\n\033[32;1m%s\033[m\n----------------------\n",parte_local);	
	}
	
	//fechando arquivo local
	fclose(arquivo_comparado);
	request_abortar_transmissao();
}





int possui_diferenca(unsigned char *arqA, unsigned char *arqB, int tamArqA, int tamArqB){
	if (tamArqA != tamArqB) {
		return 1;
	} else {
		int i;
		for (i=0;i<tamArqA;i++) {
			if (arqA[i] != arqB[i]) {
				return 1;
			}
		}
	}
	return 0;
}



unsigned char *proxima_parte_arquivo_local(int *tam){
	return readFile(arquivo_comparado,tam,DADOS);
}




unsigned char *request_diretorio_remoto_atual(){
	unsigned char *retorno = NULL;
	mensagem *m = (mensagem *) malloc(sizeof(mensagem));
	m->tipo = REQUEST_DIR_ATUAL;
	m->dados = (unsigned char *) malloc(sizeof(unsigned char));
	m->tamanhoDados = 1;
	envia_mensagem(m);
	m = recebe_mensagem();
	if (m->tipo == REQUEST_DIR_ATUAL) {
		retorno = m->dados; 
	}
	free(m);
	return retorno;
}






void request_abortar_transmissao(){
	mensagem *m = (mensagem *) malloc(sizeof(mensagem));
	m->tipo = ABORTAR_TRANSMISSAO;
	m->tamanhoDados = 1;
	m->dados = (unsigned char *) malloc(sizeof(unsigned char));
	envia_mensagem(m);
	free(m->dados);
	free(m);
}






int request_existe_arquivo_remoto(unsigned char *nomearquivo, int tamNomeArq){
	int retorno=0;
	mensagem *m = (mensagem *) malloc(sizeof(mensagem));
	m->tipo = REQUEST_ARQUIVO_EXISTE;
	m->dados = nomearquivo;
	m->tamanhoDados = tamNomeArq;
	envia_mensagem(m);
	m = recebe_mensagem();
	if (m->tipo == REQUEST_ARQUIVO_EXISTE) {
		retorno = m->dados[0];
	} 
	free(m);
	return retorno;
}





unsigned char *request_proxima_parte_arquivo_remoto(int *tam){
	unsigned char *retorno = NULL;
	mensagem *m = (mensagem *) malloc(sizeof(mensagem));
	m->tamanhoDados = 1;
	m->dados = (unsigned char *) malloc(sizeof(unsigned char));
	m->tipo = REQUEST_NEXT_PART_OF_FILE;
	envia_mensagem(m);
	m = recebe_mensagem();
	if (m->tipo == REQUEST_NEXT_PART_OF_FILE) {
		*tam = m->tamanhoDados;
		retorno = m->dados;
	} else if (m->tipo == FIM_ARQUIVO) {
		*tam = 0;
		retorno = NULL;
	}
	free(m);
	return retorno;
}
