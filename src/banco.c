#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/meu_banco.h"

int AbrirBanco(Banco *banco, size_t quantidade) {
	if (banco->carteiras != NULL || quantidade == 0 || quantidade > 9000) {
		return 0;
	}

	CarteiraCliente *carteiras = malloc(quantidade * sizeof(CarteiraCliente));
	if (carteiras == NULL) {
		return 0;
	}

	for (size_t i = 0; i < quantidade; i++) {
		carteiras[i] = (CarteiraCliente){0};
	}

	banco->carteiras = carteiras;
	banco->quantidade = quantidade;
	return 1;
}

int CadastrarCliente(Banco *banco, size_t posicao, const char nome[],
                     int tipo_conta, int poupanca) {
	CarteiraCliente carteira = {0};

	if (posicao >= banco->quantidade) {
		return 0;
	}

	if (strlen(nome) >= sizeof(carteira.cliente.nome)) {
		return 0;
	}

	strcpy(carteira.cliente.nome, nome);
	carteira.cliente.tipo_conta = tipo_conta;
	carteira.cliente.poupanca = poupanca;
	carteira.emprestimo = 2;
	carteira.numeroConta = 1000 + (int)posicao;

	banco->carteiras[posicao] = carteira;
	return 1;
}

void FecharBanco(Banco *banco) {
	free(banco->carteiras);
	banco->carteiras = NULL;
	banco->quantidade = 0;
}
