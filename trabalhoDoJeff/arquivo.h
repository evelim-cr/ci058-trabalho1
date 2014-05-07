#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>


#define ERRO_DIRETORIO_INEXISTENTE 1
#define ERRO_FALTA_PERMISSAO 2
#define ERRO_ARQUIVO_INEXISTENTE 3


FILE *openFile(char *fileName);
FILE *ls(unsigned char *opcoes, int tamOpcoes);

unsigned char *readFile(FILE *file, int *sizeRead, int maxSizeRead);
unsigned char *diretorio_atual();

int cd(unsigned char *diretorio);
int arquivo_existe(unsigned char *nome);

void trata_erro(int numErro);
void removeTempFile();
