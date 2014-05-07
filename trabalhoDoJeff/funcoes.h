#include <stdio.h>
#include <stdlib.h>

//valores em bits
#define BYTE 8

typedef unsigned char binario;


void imprimeBinario(binario *bin,int qtdbits);
void intTobin(int num, int qtdbits, binario *bin);
void strTobin(unsigned char *str, int qtdchar, binario *bin);

int binToint(binario *bin, int qtdbits);
int pot(int base, int exp);
int xorBin(int a, int b);

unsigned char *binTostr(binario *bin, int qtdbits);
