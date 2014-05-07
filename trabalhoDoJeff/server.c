#include "conexao.h"
#include "protocolo.h"
#include "arquivo.h"


int conexao;
int proximo_quadro_enviar = 0;
int proximo_quadro_receber = 0;
FILE *arquivo_comparado = NULL;

void rls(mensagem *m);
void rcd(mensagem *m);
void envia_dir_atual();
void envia_se_arquivo_existe(mensagem *m);
void envia_proxima_parte_arquivo(mensagem *m);
void abortar_transmissao();



int main() {

	conexao = criaConexao();

	mensagem *m;

	while (1){
		m = recebe_mensagem();
		switch (m->tipo) {
			case LS: { rls(m); break; }
			case CD: { rcd(m); break; }
			case REQUEST_DIR_ATUAL: { envia_dir_atual(); break;}
			case REQUEST_ARQUIVO_EXISTE: { envia_se_arquivo_existe(m); break;}
			case REQUEST_NEXT_PART_OF_FILE: { envia_proxima_parte_arquivo(m); break;}
			case ABORTAR_TRANSMISSAO: {abortar_transmissao(); break;}
			default: { printf("Requisição desconhecida!\n"); break; }
		}
		free(m);
	}


}



void envia_dir_atual(){
	unsigned char *dir_atual = diretorio_atual();
	mensagem *m = (mensagem *) malloc(sizeof(mensagem));
	m->tipo = REQUEST_DIR_ATUAL;
	m->tamanhoDados = strlen(dir_atual);
	m->dados = dir_atual;
	envia_mensagem(m);
}




void rls(mensagem *m){
	FILE *f = ls(m->dados,m->tamanhoDados);
	if (f != NULL){
		m->tipo = MOSTRAR_TELA;
		m->dados = readFile(f,&m->tamanhoDados,DADOS);
		while (m->tamanhoDados > 0) {
			envia_mensagem(m);
			m->dados = readFile(f,&m->tamanhoDados,DADOS);
		}
		fclose(f);
		remove_temp_file();
	}	
	m->tipo = FIM_TEXTO;
	envia_mensagem(m);
}



void rcd(mensagem *m){
	int erro = cd(m->dados);
	if (erro){
		m->tipo = ERRO;
		m->tamanhoDados = 1;
		unsigned char *dados = (unsigned char *) malloc(sizeof(unsigned char));
		dados[0] = erro;
		m->dados = dados;
		envia_mensagem(m);
		free(dados);
	} else {
		envia_mensagem(m); //envia mensagem para confirmar que deu certo o CD
	}
}



void envia_se_arquivo_existe(mensagem *m){
	int existe = arquivo_existe(m->dados);
	//se existe entao deixa aberto para futuras leituras remotas
	if (existe) {
		arquivo_comparado = fopen(m->dados,"r");
	}
	//enviando resposta
	unsigned char *dados = (unsigned char *) malloc(sizeof(unsigned char));
	dados[0] = existe;
	m->dados = dados;
	m->tamanhoDados = 1;
	m->tipo = REQUEST_ARQUIVO_EXISTE;
	envia_mensagem(m);
}




void envia_proxima_parte_arquivo(mensagem *m){
	m->tipo = REQUEST_NEXT_PART_OF_FILE;
	m->dados = readFile(arquivo_comparado,&m->tamanhoDados,DADOS);
	if (m->tamanhoDados > 0) {
		envia_mensagem(m);
	} else {
		//envia mensagem que terminou o arquivo
		m->tipo = FIM_ARQUIVO;
		envia_mensagem(m);
	}
}





void abortar_transmissao(){
	if (arquivo_comparado != NULL) {
		fclose(arquivo_comparado);
		arquivo_comparado = NULL;
		printf("Transmissão de arquivo finalizada!\n");
	}
}
