#ifndef MEUBANCO_H
#define MEUBANCO_H

#include <stddef.h>

#define TAMANHO_NUMERO_CONTA 8
#define TAMANHO_EXTRATO 5

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

typedef struct {
	float valor;
	char operacao; // D = deposito, S = saque, E = emprestimo
} Transacao;

typedef struct {
	Transacao transacoes[TAMANHO_EXTRATO];
	int quantidade; // quantas transacoes ja estao guardadas
} Historico;

typedef struct carteira {
	Cliente cliente;
	float saldo; // saldo do tipo de conta escolhido no cadastro
	float saldo_poupanca; // saldo da poupanca extra
	int emprestimo; // 1 para sim ou 2 para nao;
	float limite_emprestimo;
	int numeroConta; // base 4 digitos
	Historico historico; // extrato da conta principal
	Historico historico_poupanca; // extrato da poupanca extra
	unsigned int quantidade_titulos; // 0 = ainda nao comprou titulo

} CarteiraCliente;

typedef struct {
	CarteiraCliente *carteiras;
	size_t quantidade;
} Banco;

int AbrirBanco(Banco *banco, size_t quantidade);
int CadastrarCliente(Banco *banco, size_t posicao, const char nome[], int tipo_conta, int poupanca);
int FormatarNumeroConta(const CarteiraCliente *carteira, int tipo_conta, char numero[TAMANHO_NUMERO_CONTA]);
// nas operacoes: 1 = conseguiu, 0 = recusou
int Depositar(CarteiraCliente *carteira, int tipo_conta, float valor);
int Sacar(CarteiraCliente *carteira, int tipo_conta, float valor);
int ExibirExtrato(const CarteiraCliente *carteira, int tipo_conta);
float CalcularLimiteEmprestimo(const CarteiraCliente *carteira);
int SolicitarEmprestimo(CarteiraCliente *carteira, int tipo_conta, float valor);
int PodeAdquirirTitulo(const CarteiraCliente *carteira);
int AdquirirTitulo(CarteiraCliente *carteira, int tipo_conta);
void FecharBanco(Banco *banco);

#endif
