all:
	gcc -g -c conexao.c
	gcc -g -c protocolo.c
	gcc -g -c funcoes.c
	gcc -g conexao.o protocolo.o funcoes.o -o client client.c
	gcc -g conexao.o protocolo.o funcoes.o -o server server.c
	rm *.o

client:
	gcc -g -c conexao.c
	gcc -g -c protocolo.c
	gcc -g -c funcoes.c
	gcc -g conexao.o protocolo.o funcoes.o -o client client.c
	rm *.o

server:
	gcc -g -c conexao.c
	gcc -g -c protocolo.c
	gcc -g -c funcoes.c
	gcc -g conexao.o protocolo.o funcoes.o -o server server.c
	rm *.o