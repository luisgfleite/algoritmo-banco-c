#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include "../include/meu_banco.h"

int AbrirBanco(Banco *banco, size_t quantidade) {
	// so abre se estiver fechado e tiver de 1 a 9000 clientes
	if (banco->carteiras != NULL || quantidade == 0 || quantidade > 9000) {
		return 0;
	}

	CarteiraCliente *carteiras = malloc(quantidade * sizeof(CarteiraCliente));
	if (carteiras == NULL) {
		return 0;
	}

	for (size_t i = 0; i < quantidade; i++) {
		// comeca com saldo, historico e quantidade de titulos zerados
		carteiras[i] = (CarteiraCliente){0};
	}

	banco->carteiras = carteiras;
	banco->quantidade = quantidade;
	return 1;
}

int CadastrarCliente(Banco *banco, size_t posicao, const char nome[], int tipo_conta, int poupanca) {
	CarteiraCliente carteira = {0};

	if (posicao >= banco->quantidade) {
		return 0;
	}

	if (strlen(nome) >= sizeof(carteira.cliente.nome)) {
		return 0;
	}

	carteira.cliente.id = (int)posicao + 1;
	strcpy(carteira.cliente.nome, nome);
	carteira.cliente.tipo_conta = tipo_conta;
	carteira.cliente.poupanca = poupanca;
	carteira.emprestimo = 2;
	// posicao 0 vira 1000, posicao 1 vira 1001 e assim vai
	carteira.numeroConta = 1000 + (int)posicao;

	banco->carteiras[posicao] = carteira;
	return 1;
}

void FecharBanco(Banco *banco) {
	// libera o espaco dos clientes e marca o banco como fechado
	free(banco->carteiras);
	banco->carteiras = NULL;
	banco->quantidade = 0;
}

int PossuiConta(const CarteiraCliente *carteira, int tipo_conta) {
	if (carteira == NULL || tipo_conta < CONTA_CORRENTE || tipo_conta > CONTA_POUPANCA) {
		return 0;
	}

	// pode ser a conta principal ou a poupanca extra (1 = tem poupanca)
	return tipo_conta == carteira->cliente.tipo_conta || (tipo_conta == CONTA_POUPANCA && carteira->cliente.poupanca == 1);
}

void RegistrarTransacao(Historico *historico, float valor, char operacao) {
	// se ja tem 5, puxa tudo uma posicao pra tras e perde a mais antiga
	if (historico->quantidade == TAMANHO_EXTRATO) {
		for (int i = 1; i < TAMANHO_EXTRATO; i++) {
			historico->transacoes[i - 1] = historico->transacoes[i];
		}
		historico->quantidade--;
	}

	// a quantidade indica a proxima posicao livre
	int posicao = historico->quantidade;
	historico->transacoes[posicao].valor = valor;
	historico->transacoes[posicao].operacao = operacao;
	historico->quantidade++;
}

int MovimentarConta(CarteiraCliente *carteira, int tipo_conta, float valor, char operacao) {
	// isfinite recusa infinito e valores que nao sao numeros
	if (!PossuiConta(carteira, tipo_conta) || !isfinite(valor) || valor <= 0) {
		return 0;
	}

	// pega o endereco do saldo e do historico pra mudar a conta certa
	float *saldo = &carteira->saldo;
	Historico *historico = &carteira->historico;
	if (tipo_conta != carteira->cliente.tipo_conta) {
		saldo = &carteira->saldo_poupanca;
		historico = &carteira->historico_poupanca;
	}

	double novo_saldo = *saldo;
	if (operacao == 'S') {
		// so a corrente pode ficar negativa
		if (tipo_conta != CONTA_CORRENTE && valor > *saldo) {
			return 0;
		}
		novo_saldo -= valor;
	} else {
		// D e E entram como dinheiro na conta
		novo_saldo += valor;
	}

	// FLT_MAX e o maior valor que cabe em float
	// se arredondar pro mesmo saldo, recusa antes de registrar
	if (novo_saldo < -FLT_MAX || novo_saldo > FLT_MAX || (float)novo_saldo == *saldo) {
		return 0;
	}

	// so muda o saldo e grava no historico depois das verificacoes
	*saldo = (float)novo_saldo;
	RegistrarTransacao(historico, valor, operacao);
	return 1;
}

int Depositar(CarteiraCliente *carteira, int tipo_conta, float valor) {
	return MovimentarConta(carteira, tipo_conta, valor, 'D');
}

int Sacar(CarteiraCliente *carteira, int tipo_conta, float valor) {
	return MovimentarConta(carteira, tipo_conta, valor, 'S');
}

int ExibirExtrato(const CarteiraCliente *carteira, int tipo_conta) {
	if (!PossuiConta(carteira, tipo_conta)) {
		return 0;
	}

	const Historico *historico = &carteira->historico;
	if (tipo_conta != carteira->cliente.tipo_conta) {
		// a poupanca extra tem o extrato dela
		historico = &carteira->historico_poupanca;
	}

	if (historico->quantidade == 0) {
		printf("Nenhuma transacao.\n");
	}

	// mostra da mais antiga ate a mais nova, com 2 casas decimais
	for (int i = 0; i < historico->quantidade; i++) {
		Transacao transacao = historico->transacoes[i];
		printf("%.2f :%c\n", transacao.valor, transacao.operacao);
	}
	return 1;
}

float CalcularLimiteEmprestimo(const CarteiraCliente *carteira) {
	// precisa das duas contas, saldo na poupanca e nenhum emprestimo anterior
	if (!PossuiConta(carteira, CONTA_CORRENTE) || !PossuiConta(carteira, CONTA_POUPANCA) ||
	    carteira->saldo_poupanca <= 0 || carteira->emprestimo == 1) {
		return 0;
	}

	// 25% de 10 vezes o saldo total da 2,5 vezes a soma dos saldos
	double saldo_total = (double)carteira->saldo + carteira->saldo_poupanca;
	double limite = saldo_total * 2.5;
	if (limite <= 0) {
		return 0;
	}
	if (limite > FLT_MAX) {
		return FLT_MAX;
	}
	return (float)limite;
}

int SolicitarEmprestimo(CarteiraCliente *carteira, int tipo_conta, float valor) {
	if (tipo_conta != CONTA_CORRENTE) {
		return 0;
	}

	float limite = CalcularLimiteEmprestimo(carteira);
	// se passar do limite nem tenta colocar o dinheiro na conta
	if (valor > limite || !MovimentarConta(carteira, tipo_conta, valor, 'E')) {
		return 0;
	}

	// so marca que ja pegou depois que o dinheiro entrou
	carteira->emprestimo = 1;
	carteira->limite_emprestimo = limite;
	return 1;
}

int PodeAdquirirTitulo(const CarteiraCliente *carteira) {
	// so conta salario, de 1000 ate 1500 contando os dois valores
	return PossuiConta(carteira, CONTA_SALARIO) &&
	       carteira->saldo >= 1000 && carteira->saldo <= 1500;
}

int AdquirirTitulo(CarteiraCliente *carteira, int tipo_conta) {
	// UINT_MAX e o maior numero que cabe no contador de titulos
	if (tipo_conta != CONTA_SALARIO || !PodeAdquirirTitulo(carteira) ||
	    carteira->quantidade_titulos == UINT_MAX) {
		return 0;
	}

	// o enunciado nao tem preco, entao so soma um no contador
	carteira->quantidade_titulos++;
	return 1;
}
