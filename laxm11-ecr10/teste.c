#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main(int argc, char const *argv[])
{
	int tam;
	char *s=LerStringDin(&tam);
	printf("strlen(s)= %d\n", (int) strlen(s));
	printf("tam= %d\n", tam);
	return 0;
}