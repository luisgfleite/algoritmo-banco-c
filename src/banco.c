#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/meu_banco.h"
/*
meu_banco.h

#ifndef MEUBANCO_H
#define MEUBANCO_H

typedef struct cliente {
	char nome[50];
	int tipo_conta;
	int poupanca; // 1 para sim - 2 para nao;
} Cliente;

typedef struct carteira {
	Cliente cliente;
	float saldo;
	float saldo_poupanca;
	int emprestimo; // 1 para sim s ou 2 para nao;
	float limite_emprestimo;
	int numeroConta;
	int historicoValor[10];
	char historicoOperacao[10];

} CarteiraCliente;

#endif
*/


int CadastrarCliente(char[] nome, int tipo_conta, int poupanca, float saldo, float saldo_poupanca) {
    CarteiraCliente novoCliente;
    strcpy(novoCliente.cliente.nome, nome);
    novoCliente.cliente.tipo_conta = tipo_conta;

    if (tipo_conta ==)

    return 0;
}