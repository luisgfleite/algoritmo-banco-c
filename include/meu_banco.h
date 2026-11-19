#ifndef MEUBANCO_H
#define MEUBANCO_H

#include <stddef.h>

#define TAMANHO_NUMERO_CONTA 8

enum TipoConta {
	CONTA_CORRENTE = 1,
	CONTA_SALARIO = 2,
	CONTA_POUPANCA = 3
};

typedef struct cliente {
	int id;
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
	int numeroConta; // Base de quatro dígitos, antes do código do tipo.
	int historicoValor[10];
	char historicoOperacao[10];

} CarteiraCliente;

typedef struct {
	CarteiraCliente *carteiras;
	size_t quantidade;
} Banco;

int AbrirBanco(Banco *banco, size_t quantidade);
int CadastrarCliente(Banco *banco, size_t posicao, const char nome[],
                     int tipo_conta, int poupanca);
int FormatarNumeroConta(const CarteiraCliente *carteira, int tipo_conta,
                        char numero[TAMANHO_NUMERO_CONTA]);
void FecharBanco(Banco *banco);

#endif
