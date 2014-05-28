#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include "conexao.h"
#include "protocolo.h"
#include "funcoes.h"

void cdLocal (char *path);
void cdRemoto (int socket, char *path);
void lsLocal ();
void lsRemoto (int s);
void catLocal();
void catRemoto(int s);
char *DirAtualLocal(void);
void exibeMenu(int s);



int main(int argc, char const *argv[])
{
    int s = criaConexao();
    int tamStr;
    // FILE *infile = fopen ("teste.txt","r");
    exibeMenu(s);
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
                lsRemoto(s);
                PressioneEnter();
                break;
            }
            case 3: // CD Local
            {
                printf ("Digite o caminho para o diretorio:\n? cd ");
                char *path=LerStringDin(&tamStr);
                cdLocal (path);
                free (path);
                break;
            }
            case 4: // CD Remoto
            {
                printf ("Digite o caminho para o diretorio:\n? cd ");
                char *path=LerStringDin(&tamStr);
                cdRemoto (s, path);
                free (path);
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
                catRemoto(s);
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
        exibeMenu(s);
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

void cdRemoto (int socket, char *path)
{
    mensagem msg;
    mensagem_bin msg_bin;
    int i=0, j;
    msg.tipo = CD;
    msg.tamanho = 0;
    int tampath = strlen(path)+1;
    while ( tampath-i > 0)
    {
        if (msg.tamanho<2)
        {
            msg.dados[(msg.tamanho)++] = path[i];
            i++;
        }
        if ((msg.tamanho==2) || (tampath-i == 0))
        {
            if (EhFimTexto(msg.dados))
                msg.tipo=FIMTXT;
            msg_bin = MensagemToMensagem_bin(msg);
            envia_mensagem_bin (socket, &msg_bin);
            // recebe mensagem conferindo se tem erro e manda dnovo caso nessecario
            msg.tamanho=0;
            bzero (msg.dados,2);
        }
    }
    recebe_mensagem_bin(socket, &msg_bin);
    msg = Mensagem_binToMensagem(msg_bin);
    int resposta = (int) msg.dados[0];  //ou atoi
    if (resposta)
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

char *DirAtualRemoto(int socket)
{
    int i=0, j;
    char *dir=NULL;
    mensagem msg;
    mensagem_bin msg_bin;
    msg.tipo = DIR_ATUAL;
    msg.tamanho = 0;
    bzero (msg.dados, 2);
    msg_bin = MensagemToMensagem_bin(msg);
    envia_mensagem_bin (socket, &msg_bin);
    do
    {
        recebe_mensagem_bin(socket, &msg_bin);
        msg = Mensagem_binToMensagem(msg_bin);
        dir = realloc(dir, i+msg.tamanho);
        for (j = 0; j < msg.tamanho; j++, i++)
            dir[i]= msg.dados[j];
    } while (msg.tipo!=FIMTXT);
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

void lsRemoto (int s)
{
    mensagem msg;
    mensagem_bin msg_bin;
    msg.tipo = LS;
    printf ("Digite os argumentos do ls remoto:\n? ls ");
    int tamargs, i=0, j;
    char *ls=NULL, *lsArgs=LerStringDin(&tamargs);
    int tamls=4+tamargs;
    ls = malloc (4+tamargs);
    ls[0]='l';
    ls[1]='s';
    ls[2]=' ';
    ls[3]='\0';
    strcat (ls, lsArgs);
    char *resposta=NULL;
    msg.tamanho=0;
    while ( tamls-i > 0)
    {
        if (msg.tamanho<2)
        {
            msg.dados[(msg.tamanho)++] = ls[i];
            i++;
        }
        if ((msg.tamanho==2) || (tamls-i == 0))
        {
            if (EhFimTexto(msg.dados))
                msg.tipo=FIMTXT;
            msg_bin = MensagemToMensagem_bin(msg);
            envia_mensagem_bin (s, &msg_bin);
            // recebe mensagem conferindo se tem erro e manda dnovo caso nessecario
            msg.tamanho=0;
            bzero (msg.dados,2);
        }
    }
    i=0;
    j=0;
    do
    {
        recebe_mensagem_bin(s, &msg_bin);
        msg = Mensagem_binToMensagem(msg_bin);
        resposta = realloc(resposta, i+msg.tamanho);
        for (j = 0; j < msg.tamanho; j++, i++)
            resposta[i]= msg.dados[j];
    } while (msg.tipo!=FIMTXT);
    puts (resposta);
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

void catRemoto(int s)
{
    mensagem msg;
    mensagem_bin msg_bin;
    printf("Digite os argumentos do cat:\n? cat ");
    int tamargs;
    char *catArgs=LerStringDin(&tamargs), *resposta=NULL;
    int i=0, j=0, tamcat = 4+strlen(catArgs)+1;
    char *cat = (char *) malloc (sizeof(char)*tamcat);
    cat[0] = 'c';
    cat[1] = 'a';
    cat[2] = 't';
    cat[3] = ' ';
    cat[4] = '\0';
    cat = strcat(cat, catArgs);
    msg.tipo = CAT;
    msg.tamanho = 0;
    while ( tamcat-i > 0)
    {
        if (msg.tamanho<2)
        {
            msg.dados[(msg.tamanho)++] = cat[i];
            i++;
        }
        if ((msg.tamanho==2) || (tamcat-i == 0))
        {
            if (EhFimTexto(msg.dados))
                msg.tipo=FIMTXT;
            msg_bin = MensagemToMensagem_bin(msg);
            envia_mensagem_bin (s, &msg_bin);
            // recebe mensagem conferindo se tem erro e manda dnovo caso nessecario
            msg.tamanho=0;
            bzero (msg.dados,2);
        }
    }
    i=0;
    j=0;
    do
    {
        recebe_mensagem_bin(s, &msg_bin);
        msg = Mensagem_binToMensagem(msg_bin);
        resposta = realloc(resposta, i+msg.tamanho);
        for (j = 0; j < msg.tamanho; j++, i++)
            resposta[i]= msg.dados[j];
    } while (msg.tipo!=FIMTXT);
    puts (resposta);
    free (catArgs);
    free (cat);
    free (resposta);
}

void get(int s)
{
    mensagem msg;
    mensagem_bin msg_bin;
    msg.tipo = GET;
    msg.tamanho = 0;
    bzero (msg.dados, 2);
    msg_bin = MensagemToMensagem_bin(msg);
    envia_mensagem_bin (s, &msg_bin);
    printf("Digite o nome do arquivo:\n? ");
    int tamfilename;
    char *filename=LerStringDin(&tamfilename);
    int i=0;
    msg.tipo = GET;
    msg.tamanho = 0;
    while ( tamfilename-i > 0)
    {
        if (msg.tamanho<2)
        {
            msg.dados[(msg.tamanho)++] = filename[i];
            i++;
        }
        if ((msg.tamanho==2) || (tamfilename-i == 0))
        {
            if (EhFimTexto(msg.dados))
                msg.tipo=FIMTXT;
            msg_bin = MensagemToMensagem_bin(msg);
            envia_mensagem_bin (s, &msg_bin);
            // recebe mensagem conferindo se tem erro e manda dnovo caso nessecario
            msg.tamanho=0;
            bzero (msg.dados,2);
        }
    }
    recebe_mensagem_bin(s, &msg_bin);
    msg = Mensagem_binToMensagem(msg_bin);
    FILE *dest;
    if (msg.tipo!=ERRO)
    {
        if ((dest = fopen (filename,"w+b"))==NULL)
        {
            printf ("Erro ao abrir ou criar o arquivo "); puts (filename);
        }
        do
        {
            recebe_mensagem_bin(s, &msg_bin);
            msg = Mensagem_binToMensagem(msg_bin);
            if (fwrite (msg.dados, 1, msg.tamanho, dest)!=msg.tamanho)
                puts ("Erro na escrita em arquivo.");
        } while (msg.tipo!=FIMTXT);
        puts ("Arquivo copiado com sucesso.");
        fclose (dest);
    }
    else
        puts ("Erro ao receber arquivo.");
    free (filename);
    // free (cat);
}

void exibeMenu(int socket)
{
    system ("clear");
    printf(YEL "Diretório Local: "); puts (DirAtualLocal()); printf(NRM);
    printf(YEL "Diretório Remoto: "); puts (DirAtualRemoto(socket)); printf(NRM);
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