#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include "conexao.h"
#include "protocolo.h"
// #include "protocolo.h"

#define NRM  "\x1B[0m"
#define RED  "\x1B[31m"
#define GRN  "\x1B[32m"
#define YEL  "\x1B[33m"
#define BLU  "\x1B[34m"
#define MAG  "\x1B[35m"
#define CYN  "\x1B[36m"
#define WHT  "\x1B[37m"

void cdLocal (char *path);
void lsLocal ();
void catLocal();
char *DirAtualLocal(void);
char *LerStringDin(int *tam);
void exibeMenu();
void LimpaBuffer ();
int LeOpcao (int min, int max);
char *ngets (char *str, int n, FILE *fp);
void PressioneEnter ();


int main(int argc, char const *argv[])
{
    int s = criaConexao();
    int tamStr;
    // FILE *infile = fopen ("teste.txt","r");
    exibeMenu();
    int op=LeOpcao(1,9);
    while (op!=9)
    {
        switch (op)
        {
            case 1: // LS Local
            {
                lsLocal();
                PressioneEnter();
                break;
            }
            case 2: // LS Remoto
            {
                printf(NRM "Testando cor NRM\n");
                printf(RED "Testando cor RED\n");
                printf(GRN "Testando cor GRN\n");
                printf(YEL "Testando cor YEL\n");
                printf(BLU "Testando cor BLU\n");
                printf(MAG "Testando cor MAG\n");
                printf(CYN "Testando cor CYN\n");
                printf(WHT "Testando cor WHT\n");
                PressioneEnter();
                break;
            }
            case 3: // CD Local
            {
                printf ("Digite o caminho para o diretorio:\n? cd ");
                char *path=LerStringDin(&tamStr);
                cdLocal (path);
                break;
            }
            case 4: // CD Remoto
            {
                char *str = LerStringDin(&tamStr);
                envia_string(s, str);
                free(str);
                PressioneEnter();
                break;
            }
            case 5: // CAT Local
            {
                catLocal();
                PressioneEnter();
                break;
            }
            case 6: // CAT Remoto
            {
                PressioneEnter();
                break;
            }
            case 7: // GET
            {
                PressioneEnter();
                break;
            }
            case 8: // PUT
            {
                PressioneEnter();
                break;
            }
        }
        exibeMenu();
        op=LeOpcao(1,9);
    }
    // fclose (infile);
    return 0;
}

void cdLocal (char *path)
{
    if (chdir(path))
    {
        printf(RED "Mudanca de diretorio falhou!\n" NRM);
        PressioneEnter();
    }
}

char *DirAtualLocal(void)
{
    char *dir;
    dir = (char *) (long int) get_current_dir_name();
    if (dir==0)
        return ("Nao foi possivel localizar o diretorio atual.");
    return dir;
}

void lsLocal ()
{
    printf ("Digite os argumentos do ls:\n? ls ");
    int tamargs;
    char *lsArgs=LerStringDin(&tamargs);
    int lsTam = 3+strlen(lsArgs)+1;
    char *ls = (char *) malloc (sizeof(char)*lsTam);
    ls[0] = 'l';
    ls[1] = 's';
    ls[2] = ' ';
    ls[3] = '\0';
    ls = strcat(ls, lsArgs);
    if (system (ls)==-1)
        puts ("ls falhou!");
    free (lsArgs);
    free (ls);
}

void catLocal()
{
    printf("Digite os argumentos do cat:\n? cat ");
    int tamargs;
    char *catArgs=LerStringDin(&tamargs);
    int catTam = 4+strlen(catArgs)+1;
    char *cat = (char *) malloc (sizeof(char)*catTam);
    cat[0] = 'c';
    cat[1] = 'a';
    cat[2] = 't';
    cat[3] = ' ';
    cat[4] = '\0';
    cat = strcat(cat, catArgs);
    if (system (cat)==-1)
        puts ("cat falhou!");
    free (catArgs);
    free (cat);
}

char *LerStringDin (int *tam)
{
    int i;
    char *string;
    char letra;
    string = (char *) malloc (sizeof(char));
    for (i=0; (letra=getchar())!='\n' ; i++)
    {
        string[i] = letra;
        string = (char *) realloc (string, sizeof(char)*(i+2));
    }
    string[i] = 0;
    (*tam)=i;
    return string;
}

void exibeMenu()
{
    system ("clear");
    printf(YEL "Diretório Local: "); puts (DirAtualLocal()); printf(NRM);
    printf
        (BLU "\t+-----------"YEL"Client"BLU"----------+\n"
         BLU "\t|                           |\n" NRM
         BLU "\t|     "CYN"(1) LS Local."BLU"         |\n" NRM
         BLU "\t|     "CYN"(2) LS Remoto."BLU"        |\n" NRM
         BLU "\t|     "CYN"(3) CD Local."BLU"         |\n" NRM
         BLU "\t|     "CYN"(4) CD Remoto."BLU"        |\n" NRM
         BLU "\t|     "CYN"(5) CAT Local."BLU"        |\n" NRM
         BLU "\t|     "CYN"(6) CAT Remoto."BLU"       |\n" NRM
         BLU "\t|     "CYN"(7) GET."BLU"              |\n" NRM
         BLU "\t|     "CYN"(8) PUT."BLU"              |\n" NRM
         BLU "\t|     "CYN"(9) Encerrar."BLU"         |\n" NRM
         BLU "\t|                           |\n" NRM
         BLU "\t+---------------------------+\n" NRM
         "? ");
}

void LimpaBuffer ()
/*  Descarta caracteres que estão no buffer de entrada até que encontre um '\n'.
 */
{
    while (getchar()!='\n');
}

int LeOpcao (int min, int max)
{
/*  Prende o usuario em um loop até que ele digite um  valor entre 'min' e 'max'. O Valor digitado é retornado pela função.
 */
    char aux[10];
    int n;
    ngets(aux,6,stdin);
    n = atoi(aux);
    while ( (n<min) || (n>max) )
    {
        printf ("Valor Invalido, Tente novamente.\n? ");
        ngets(aux,6,stdin);
        n = atoi(aux);
    }
    return n;
}

char *ngets (char *str, int n, FILE *fp)
{
    str = fgets (str, n, stdin);
    if (str[strlen(str)-1]=='\n')
        str[strlen(str)-1]='\0';
    else
        LimpaBuffer();
    return str;
}

void PressioneEnter ()
/*  Mostra na tela a mensagem para o usuario pressionar ENTER aguarda até que ele faça isso.
 */
{
    printf (YEL "\nPressione <ENTER> para continuar...\n" NRM);
    getchar();
}