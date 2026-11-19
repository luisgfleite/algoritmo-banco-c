#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/meu_banco.h"

/* ---------- LEITURA COM VALIDACAO ---------- */
void lerLinha(const char *msg, char *dest, int tam) {
    do {
        printf("%s", msg);
        if (fgets(dest, tam, stdin) == NULL) { dest[0] = '\0'; return; }
        dest[strcspn(dest, "\n")] = '\0';
        if (dest[0] == '\0') printf("Entrada vazia!\n");
    } while (dest[0] == '\0');
}

int lerInt(const char *msg, int min, int max) {
    char buf[50];
    int v;
    while (1) {
        printf("%s", msg);
        if (fgets(buf, sizeof(buf), stdin) != NULL &&
            sscanf(buf, "%d", &v) == 1 && v >= min && v <= max)
            return v;
        printf("Valor invalido! Digite de %d a %d.\n", min, max);
    }
}

float lerValor(const char *msg) {
    char buf[50];
    float v;
    while (1) {
        printf("%s", msg);
        if (fgets(buf, sizeof(buf), stdin) != NULL &&
            sscanf(buf, "%f", &v) == 1 && v > 0)
            return v;
        printf("Valor invalido! Digite um numero maior que zero.\n");
    }
}

/* ---------- HISTORICO (ultimas 10 operacoes; vazio = '\0') ---------- */
void registrarHistorico(CarteiraCliente *c, float valor, char op) {
    int i;
    for (i = 0; i < 9; i++) {
        c->historicoValor[i] = c->historicoValor[i + 1];
        c->historicoOperacao[i] = c->historicoOperacao[i + 1];
    }
    c->historicoValor[9] = (int)valor;
    c->historicoOperacao[9] = op;
}

void imprimirExtrato(const CarteiraCliente *c) {
    int i;
    int mostrados = 0;
    printf("\n--- EXTRATO (ultimas 5) ---\n");
    for (i = 9; i >= 0 && mostrados < 5; i--) {
        if (c->historicoOperacao[i] != '\0') {
            printf("%d : %c\n", c->historicoValor[i], c->historicoOperacao[i]);
            mostrados++;
        }
    }
    if (mostrados == 0) printf("Nenhuma transacao.\n");
}

/* ---------- REGRAS ---------- */
float saldoTotal(const CarteiraCliente *c) {
    return c->saldo + (c->cliente.poupanca == 1 ? c->saldo_poupanca : 0);
}

int aptoEmprestimo(const CarteiraCliente *c) {
    return c->cliente.tipo_conta == CONTA_CORRENTE &&
           c->cliente.poupanca == 1 &&
           c->saldo_poupanca > 0 &&
           c->emprestimo != 1;
}

int corrente_negativa(const CarteiraCliente *c) {
    return c->cliente.tipo_conta == CONTA_CORRENTE && c->saldo < 0;
}

int aptoTitulo(const CarteiraCliente *c) {
    return c->cliente.tipo_conta == CONTA_SALARIO &&
           c->saldo >= 1000 && c->saldo <= 1500;
}

/* ---------- CADASTRO ---------- */
int cadastrarClientes(Banco *banco) {
    char numero[TAMANHO_NUMERO_CONTA];
    size_t i;

    for (i = 0; i < banco->quantidade; i++) {
        char nome[50];
        int tipo, poup, ok = 0;

        printf("\n--- Cliente %lu de %lu ---\n",
               (unsigned long)(i + 1), (unsigned long)banco->quantidade);
        while (!ok) {
            lerLinha("Nome completo: ", nome, sizeof(nome));
            tipo = lerInt("Tipo de conta (1-corrente, 2-salario): ", 1, 2);
            poup = lerInt("Tambem tem poupanca? (1-sim, 2-nao): ", 1, 2);

            if (CadastrarCliente(banco, i, nome, tipo, poup) == 0)
                ok = 1;
            else
                printf("Cadastro recusado. Tente de novo.\n");
        }

        FormatarNumeroConta(&banco->carteiras[i], tipo, numero);
        printf("Conta criada! Numero: %s | saldo R$ 0.00\n", numero);
        if (poup == 1) {
            FormatarNumeroConta(&banco->carteiras[i], CONTA_POUPANCA, numero);
            printf("Poupanca criada! Numero: %s | saldo R$ 0.00\n", numero);
        }
    }
    return 0;
}

/* ---------- MENU DO CLIENTE ---------- */
/* tipo = qual conta foi acessada (CONTA_CORRENTE, CONTA_SALARIO ou CONTA_POUPANCA) */
int menuCliente(CarteiraCliente *c, int tipo) {
    int op = -1;
    float *saldo;

    if (tipo == CONTA_POUPANCA)
        saldo = &c->saldo_poupanca;
    else
        saldo = &c->saldo;

    if (corrente_negativa(c))
        printf("\n*** ALERTA: sua conta corrente esta com saldo NEGATIVO! ***\n");

    while (op != 0) {
        printf("\n=== MENU DO CLIENTE: %s ===\n", c->cliente.nome);
        printf("1 - Depositar\n");
        printf("2 - Sacar\n");
        printf("3 - Solicitar emprestimo\n");
        printf("4 - Adquirir titulos de capitalizacao\n");
        printf("5 - Solicitar extrato\n");
        printf("0 - Voltar ao menu principal\n");
        op = lerInt("Opcao: ", 0, 5);

        switch (op) {
            case 1: {
                float v = lerValor("Valor do deposito: ");
                *saldo += v;
                registrarHistorico(c, v, 'D');
                printf("Deposito realizado! Saldo: R$ %.2f\n", *saldo);
                break;
            }
            case 2: {
                float v = lerValor("Valor do saque: ");
                if (v > *saldo && tipo != CONTA_CORRENTE) {
                    printf("Saque negado: saldo insuficiente.\n");
                } else {
                    *saldo -= v;
                    registrarHistorico(c, v, 'S');
                    printf("Saque realizado! Saldo: R$ %.2f\n", *saldo);
                }
                break;
            }
            case 3:
                if (!aptoEmprestimo(c)) {
                    printf("Emprestimo negado: precisa de corrente + poupanca com saldo, e so 1 por cliente.\n");
                } else {
                    c->limite_emprestimo = 0.25f * 10.0f * saldoTotal(c);
                    c->saldo += c->limite_emprestimo;
                    c->emprestimo = 1;
                    registrarHistorico(c, c->limite_emprestimo, 'E');
                    printf("Emprestimo de R$ %.2f creditado na corrente! Saldo corrente: R$ %.2f\n",
                           c->limite_emprestimo, c->saldo);
                }
                break;
            case 4:
                if (!aptoTitulo(c))
                    printf("Titulo negado: precisa de conta-salario com saldo entre 1000 e 1500.\n");
                else
                    printf("Voce esta apto ao titulo! (registro depende de um campo no .h)\n");
                break;
            case 5:
                imprimirExtrato(c);
                break;
            case 0:
                break;
        }
    }
    return 0;
}

/* ---------- ACESSAR CONTA ---------- */
int acessarConta(Banco *banco) {
    char digitado[50], numero[TAMANHO_NUMERO_CONTA];
    size_t i;

    lerLinha("Numero da conta completo (ex: 1000-01): ", digitado, sizeof(digitado));

    for (i = 0; i < banco->quantidade; i++) {
        CarteiraCliente *c = &banco->carteiras[i];

        FormatarNumeroConta(c, c->cliente.tipo_conta, numero);
        if (strcmp(digitado, numero) == 0)
            return menuCliente(c, c->cliente.tipo_conta);

        if (c->cliente.poupanca == 1) {
            FormatarNumeroConta(c, CONTA_POUPANCA, numero);
            if (strcmp(digitado, numero) == 0)
                return menuCliente(c, CONTA_POUPANCA);
        }
    }
    printf("Conta nao encontrada!\n");
    return -1;
}

/* ---------- HISTORICO DO BANCO ---------- */
void historicoBanco(const Banco *banco) {
    char numero[TAMANHO_NUMERO_CONTA];
    float total = 0;
    size_t i;

    for (i = 0; i < banco->quantidade; i++)
        total += saldoTotal(&banco->carteiras[i]);

    printf("\n== Saldo Total do banco ==\n");
    printf("%lu | R$ %.2f\n", (unsigned long)banco->quantidade, total);

    printf("\n== Clientes Aptos a Emprestimos ==\n");
    for (i = 0; i < banco->quantidade; i++) {
        const CarteiraCliente *c = &banco->carteiras[i];
        if (aptoEmprestimo(c)) {
            FormatarNumeroConta(c, c->cliente.tipo_conta, numero);
            printf("%s|%s|%.2f\n", c->cliente.nome, numero, 0.25f * 10.0f * saldoTotal(c));
        }
    }

    printf("\n== Clientes com Saldo Negativo na Conta Corrente ==\n");
    for (i = 0; i < banco->quantidade; i++) {
        const CarteiraCliente *c = &banco->carteiras[i];
        if (corrente_negativa(c)) {
            FormatarNumeroConta(c, c->cliente.tipo_conta, numero);
            printf("%s|%s|%.2f\n", c->cliente.nome, numero, c->saldo);
        }
    }

    printf("\n== Clientes Aptos a Compra de Titulos ==\n");
    for (i = 0; i < banco->quantidade; i++) {
        const CarteiraCliente *c = &banco->carteiras[i];
        if (aptoTitulo(c)) {
            FormatarNumeroConta(c, c->cliente.tipo_conta, numero);
            printf("%s|%s|%.2f\n", c->cliente.nome, numero, c->saldo);
        }
    }

    printf("\n== Clientes Contemplado no Sorteio ==\n");
    printf("(nenhum sorteio realizado)\n");
}

/* ---------- MENU PRINCIPAL ---------- */
int main() {
    Banco banco;
    int banco_aberto = 0;
    char op = ' ';
    char buf[20];

    while (op != 's') {
        printf("\n=== MENU PRINCIPAL ===\n");
        printf("1 - Abrir o banco\n");
        printf("2 - Consultar historico\n");
        printf("3 - Efetuar sorteio\n");
        printf("4 - Acessar conta\n");
        printf("s - Sair\n");
        printf("Opcao: ");
        if (fgets(buf, sizeof(buf), stdin) == NULL) break;
        op = buf[0];

        switch (op) {
            case '1': {
                int n;
                if (banco_aberto) { printf("O banco ja esta aberto!\n"); break; }
                n = lerInt("Quantos clientes? ", 1, 1000);
                if (AbrirBanco(&banco, (size_t)n) != 0) {
                    printf("Erro ao abrir o banco!\n");
                    break;
                }
                banco_aberto = 1;
                printf("Banco aberto!\n");
                cadastrarClientes(&banco);
                break;
            }
            case '2':
                if (!banco_aberto) { printf("Abra o banco primeiro!\n"); break; }
                historicoBanco(&banco);
                break;
            case '3':
                if (!banco_aberto) { printf("Abra o banco primeiro!\n"); break; }
                printf("Sorteio ainda nao implementado (precisa do Fibonacci de sala e do campo titulo).\n");
                break;
            case '4':
                if (!banco_aberto) { printf("Abra o banco primeiro!\n"); break; }
                acessarConta(&banco);
                break;
            case 's':
            case 'S':
                op = 's';
                if (banco_aberto) FecharBanco(&banco);
                printf("Programa encerrado.\n");
                break;
            default:
                printf("Opcao invalida!\n");
                break;
        }
    }
    return 0;
}