#include "conexao.h"


int criaConexao(char *device){

	/*cria socket*/
	int soquete = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (soquete == -1) {
		printf("Erro ao abrir o Socket\n");
		exit(-1);
	}

	/*pega ID do dispositivo */
	struct ifreq ir;
	memset(&ir, 0, sizeof(struct ifreq));
	memcpy(ir.ifr_name, device, sizeof(device));
	if (ioctl(soquete, SIOCGIFINDEX, &ir) == -1) {
        printf("Erro ao fazer ioctl\n");
        exit(-1);
	}
	
	/*configura endereco do dispositivo*/
	struct sockaddr_ll endereco;
	memset(&endereco, 0, sizeof(endereco));
	endereco.sll_family = AF_PACKET;
	endereco.sll_protocol = htons(ETH_P_ALL);
	endereco.sll_ifindex = ir.ifr_ifindex;


	/*atribui endereco ao socket*/
	if (bind(soquete, (struct sockaddr *)&endereco, sizeof(endereco)) == -1) {
	    printf("Erro ao fazer o bind\n");
	    exit(-1);
	}

	struct packet_mreq mr;
	memset(&mr, 0, sizeof(mr));
	mr.mr_ifindex = ir.ifr_ifindex;
	mr.mr_type = PACKET_MR_PROMISC;
	if (setsockopt(soquete, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) == -1)	{
	    printf("Erro ao fazer setsockopt\n");
	    exit(-1);
	}

	return soquete;
}
