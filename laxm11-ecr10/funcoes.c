#include "funcoes.h"

unsigned char *LerStringDin (int *tam)
{
    int i;
    unsigned char *string;
    unsigned char letra;
    string = (unsigned char *) malloc (sizeof(unsigned char));
    for (i=0; (letra=getchar())!='\n' ; i++)
    {
        string[i] = letra;
        string = (unsigned char *) realloc (string, sizeof(unsigned char)*(i+2));
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
    unsigned char aux[10];
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

unsigned char *ngets (unsigned char *str, int n, FILE *fp)
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


void intTobin(int num, int qtdbits, unsigned char *bin) {
// jef
    int i;
    for(i=qtdbits-1; i>=0; i--) {
        bin[i] = num % 2;
        num = num / 2;
    }
}


int binToint(unsigned char *bin, int qtdbits) {
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

int EhFimTexto (unsigned char *texto, int len)
{
    int i;
    for (i = 0; i < len; i++)
        if (texto[i]==0)
            return 1;
    return 0;
}

int EhImpar (int x)
{
    if (x%2)
        return 1;
    return 0;
}

void incrementa_sequencia(int *seq)
{
    if ((*seq) < 7)
        (*seq)++;
    else
        (*seq) = 0;
}

int EhSequenciaEsperada (unsigned char *seqbin, int seq)
{
   int seqComp = binToint(seqbin,4);
   if (seqComp == seq)
        return 1;
   else
        return 0;      
}
void loadBar(int x, int n, int r, int w)
{
    // Only update r times.
    if ( x % (n/r +1) != 0 ) return;
 
    // Calculuate the ratio of complete-to-incomplete.
    float ratio = x/(float)n;
    int c = ratio * w;
 
    // Show the percentage complete.
    printf("%3d%% [", (int)(ratio*100) );
 
    // Show the load bar.
    for (x=0; x<c; x++)
       printf("=");
 
    for (x=c; x<w; x++)
       printf(" ");
 
    // ANSI Control codes to go back to the
    // previous line and clear it.
    if ((ratio*100)!=100)
        printf("]\n\033[F\033[J");
    else
        printf("]\n");
}