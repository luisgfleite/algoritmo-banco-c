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