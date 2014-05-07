redes:
	gcc -c funcoes.c
	gcc -c conexao.c
	gcc -c protocolo.c
	gcc -c arquivo.c
	gcc funcoes.o arquivo.o protocolo.o conexao.o -o server server.c
	gcc funcoes.o arquivo.o protocolo.o conexao.o -o client client.c
	rm *.o
