#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NRM  "\x1B[0m"
#define RED  "\x1B[31m"
#define GRN  "\x1B[32m"
#define YEL  "\x1B[33m"
#define BLU  "\x1B[34m"
#define MAG  "\x1B[35m"
#define CYN  "\x1B[36m"
#define WHT  "\x1B[37m"

char *LerStringDin(int *tam);
void LimpaBuffer ();
int LeOpcao (int min, int max);
char *ngets (char *str, int n, FILE *fp);
void PressioneEnter ();
void intTobin(int num, int qtdbits, char *bin);
int binToint(char *bin, int qtdbits);
int pot(int base, int exp);
int EhFimTexto (char *texto);