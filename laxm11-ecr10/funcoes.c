#include "funcoes.h"

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


void intTobin(int num, int qtdbits, char *bin) {
// jef
    int i;
    for(i=qtdbits-1; i>=0; i--) {
        bin[i] = num % 2;
        num = num / 2;
    }
}


int binToint(char *bin, int qtdbits) {
//jef
    int i,j,r=0;
    for(i=0;i<qtdbits;i++) {
        if(bin[i] == 1)
            r += pot(2, (i-(qtdbits-1))*(-1));
    }
    return r;
}


int pot(int base, int exp) {
//jef
    int i,p=1;
    for(i=0; i<exp; i++)
        p *= base;
    return p;
}