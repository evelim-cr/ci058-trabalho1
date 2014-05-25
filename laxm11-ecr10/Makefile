main:
	gcc -c conexao.c
	gcc -c protocolo.c
	gcc conexao.o protocolo.o -o client client.c
	gcc conexao.o protocolo.o -o server server.c
	rm *.o
	