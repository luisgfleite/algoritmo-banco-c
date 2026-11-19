#include <stdio.h>
#include <string.h>
int cadastro_cliente(){
    char continuar = "s";
    while (continuar == "s"){
        char nome[50];
        int tipo;

        printf("\nNome do cliente: ");
        scanf("%s", &nome);
        printf("tipo de conta(01-corrente, 02-salario, 03-poupança): ");
        scanf("%d", &tipo);

        printf("cadastrar outro? (s/n): ");
        scanf(" %c",&continuar);
    }
    return 0;
}

int menucliente (int indice){
    int op = -1;
    float valor;

    while(op != 0) {
        printf("\n===MENU DO CLIENTE\n");
        printf("1-Depositar\n");
        printf("2-sacar\n");
        printf("3- solicitar emprestimo\n");
        printf("4-adquiri titulos de capitazação\n");
        printf("5- solicitar extrato\n");
        printf("0- voltar\n");
        printf("opção: ");
        scanf("%d", &op);


        switch (op){
            case 1:
            printf("valor: ");
            scanf("%f", &valor);
            break;
            case 2:
            printf("valor: ");
            scanf("%f", &valor);
            break;
            case 3:
              //banco_emprestimo(indcie)
            break;
            case 4:
            //bcomprar titulo(indicie)
            break;
            case 5:
            //b_extrato(indice)
            break;
            case 0:
                break;
        default:
        printf("opcao invalida!\n");
            break;
        }
    }
}