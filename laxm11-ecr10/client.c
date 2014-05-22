#include <stdio.h>
#include "conexao.h"
// #include "protocolo.h"
#include <unistd.h>
#include <strings.h>

int main(int argc, char const *argv[])
{
	int s = criaConexao();
	// FILE *infile = fopen ("teste.txt","r");
	unsigned char *diretorioAtualLocal = (unsigned char *) get_current_dir_name();
	printf("Diretório Local: "); puts (diretorioAtualLocal);
	chdir("/home/lucas");
	diretorioAtualLocal = (unsigned char *) get_current_dir_name();
	printf("Diretório Local: "); puts (diretorioAtualLocal);

	// fclose (infile);
	return 0;
}