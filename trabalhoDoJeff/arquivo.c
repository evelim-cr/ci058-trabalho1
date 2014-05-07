#include "arquivo.h"

#define TEMP_FILE "temp.txt"



FILE *openFile(char *fileName){
	char mode = 'r';
	return fopen(fileName,&mode);
}



unsigned char *readFile(FILE *file, int *sizeRead, int maxSizeRead){
	unsigned char c;
	unsigned char *bytesRead = NULL;
	int length = 0;	
	*sizeRead = 0;
	c = fgetc(file);
	while (!feof(file)&&(length<maxSizeRead)){
		bytesRead = (unsigned char *) realloc(bytesRead,sizeof(unsigned char)*(++length));
		bytesRead[length-1] = c;
		c = fgetc(file);
	}
	*sizeRead = length;
	return bytesRead;
}




FILE *ls(unsigned char *opcoes, int tamOpcoes){
	int i;
	int tamcmd = tamOpcoes+14;
	unsigned char *cmd = (unsigned char *) malloc(sizeof(unsigned char)*tamcmd);

	cmd[0] = 'l';
	cmd[1] = 's';
	cmd[2] = ' ';
	for (i=0;i<tamOpcoes;i++) cmd[i+3] = opcoes[i]; 	//copiando opcoes para cmd
	cmd[tamOpcoes+3] = ' ';
	cmd[tamOpcoes+4] = '>';
	cmd[tamOpcoes+5] = 't';
	cmd[tamOpcoes+6] = 'e';
	cmd[tamOpcoes+7] = 'm';
	cmd[tamOpcoes+8] = 'p';
	cmd[tamOpcoes+9] = '.';
	cmd[tamOpcoes+10] = 't';
	cmd[tamOpcoes+11] = 'x';
	cmd[tamOpcoes+12] = 't';
	cmd[tamOpcoes+13] = '\0';

	system(cmd);
	free(cmd);
	FILE *f = openFile(TEMP_FILE); 
}




unsigned char *read_keyboard(int *size_read){
	int tam = 0;
	unsigned char *bytes_read = NULL; 
	unsigned char c = getc(stdin);
	while (c!='\n') {
		bytes_read = (unsigned char *)realloc(bytes_read,sizeof(unsigned char)*(++tam));
		bytes_read[tam-1] = c;
		c = getc(stdin);
	}
	*size_read = tam;
	
	if (tam>0) { //colocando "\0" no final da sequencia de char
		bytes_read = (unsigned char *)realloc(bytes_read,sizeof(unsigned char)*(++tam));
		bytes_read[tam-1] = '\0';
	}
	
	return bytes_read;
}




int cd(unsigned char *diretorio){
	if (chdir(diretorio) == 0){ //conseguiu alterar o diretório
		return 0;
	} else {
		//ocorreu erro ao alterar o diretório
		switch(errno) {
			case ENOENT:{return ERRO_DIRETORIO_INEXISTENTE;break;}
			case EACCES:{return ERRO_FALTA_PERMISSAO;break;}
			default: { printf("\033[31;1mErro desconhecido ao fazer cd!\033[m\n");break;}
		}
	}
}


void trata_erro(int numErro){
	switch(numErro) {
		case ERRO_DIRETORIO_INEXISTENTE: {
			printf("\033[31;1mErro: Diretório Inexistente!\033[m\n");
			break;
		}
		case ERRO_FALTA_PERMISSAO: {
			printf("\033[31;1mErro: Sem permissão de acesso!\033[m\n");
			break;
		}
		case ERRO_ARQUIVO_INEXISTENTE: {
			printf("\033[31;1mErro: Arquivo Inexistente!\033[m\n");
			break;
		}
	}
}



void remove_temp_file(){
	remove(TEMP_FILE);
}


void limpa_tela(){
	system("clear");
}


unsigned char *diretorio_atual(){
	return (unsigned char *) get_current_dir_name();
}


int arquivo_existe(unsigned char *nome){
    FILE *arquivo = fopen(nome,"r");
    if (arquivo != NULL){
		fclose(arquivo);
        return 1;
    } else {
        return 0;
    }
}
