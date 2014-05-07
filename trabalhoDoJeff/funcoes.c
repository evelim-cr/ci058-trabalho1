#include "funcoes.h"


void imprimeBinario(binario *bin,int qtdbits){
	int i;
	for (i=0; i<qtdbits; i++){
		if (((i%8) == 0)&&(i>0)){
			printf(".");		
		}
		printf("%d",bin[i]);
	}
	printf("\n");
}


void intTobin(int num, int qtdbits, binario *bin) {
	int i;
	for(i=qtdbits-1; i>=0; i--) {
		bin[i] = num % 2;
		num = num / 2;
	}
}


int binToint(binario *bin, int qtdbits) {
	int i,j,r=0;
	for(i=0;i<qtdbits;i++) {
		if(bin[i] == 1)
			r += pot(2, (i-(qtdbits-1))*(-1));
	}
	return r;
}


int pot(int base, int exp) {
	int i,p=1;
	for(i=0; i<exp; i++)
		p *= base;
	return p;
}


void strTobin(unsigned char *str, int qtdchar, binario *bin){
	int i;	
	for (i=0; i<qtdchar; i++) {
		intTobin(str[i],BYTE,&bin[i*BYTE]);	
	}
}


unsigned char *binTostr(binario *bin, int qtdbits){
	int tamStr = qtdbits/BYTE;
	unsigned char *str = (unsigned char *) malloc(sizeof(unsigned char)*tamStr);	
	int i;	
	for (i=0; i<tamStr; i++) {
		str[i] = binToint(&bin[i*BYTE], BYTE);
	}
	return str;
}


int xorBin(int a, int b){
	return (!(a&&b))&&(a||b);	
}
