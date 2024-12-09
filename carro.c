    #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define N 53

typedef struct carro {
    char placa[8];
    char marca[15];
    char modelo[15];
    char cor[15];
    int status;  // 1 - ativo ou 0 - removido
} CARRO;

typedef struct noTabela {
    char placa[8];
    int posicao;
    struct noTabela* prox;
} NO;

FILE* prepararArquivo(char nome[]);
void fecharArquivo(FILE* arq);
int buscar(NO* tabelaHashing[], char placa[]);
void cadastrar(FILE* arq, NO* tabelaHashing[]);
void consultar(FILE* arq, NO* tabelaHashing[]);
void alterar(FILE* arq, NO* tabelaHashing[]);
void remover(FILE* arq, NO* tabelaHashing[]);
void exibirCadastro(FILE* arq);
void criarIndice(FILE* arq, NO* tabelaHashing[]);
void inserirTabelaHash(NO* tabelaHashing[], char placa[], int pos);
void removerTabelaHash(NO* tabelaHashing[], char placa[], int posTabela);
int hashing(char placa[]);
void exibirOpcoes();
void desalocarIndice(NO* tabelaHashing[]);

int main() {
    char nomeArq[] = "carros.dat";
    int op;
    FILE* cadastro;
    NO* tabelaHashing[N] = {NULL};
    cadastro = prepararArquivo(nomeArq);
    if (cadastro == NULL)
        printf("Erro na abertura do arquivo. Programa encerrado \n");
    else {
        criarIndice(cadastro, tabelaHashing);
        do {
            exibirOpcoes();
            scanf("%d",&op); fflush(stdin);
            switch (op) {
                case 1: cadastrar(cadastro, tabelaHashing);
                    break;
                case 2: consultar(cadastro, tabelaHashing);
                    break;
                case 3: alterar(cadastro, tabelaHashing);
                    break;
                case 4: remover(cadastro, tabelaHashing);
                    break;
                case 5: exibirCadastro(cadastro);
                    break;
                case 0: fecharArquivo(cadastro);
                    desalocarIndice(tabelaHashing);
                    break;
                default: printf("Opcao invalida \n");
            }
        } while (op != 0);
    }
    return 0;
}

FILE* prepararArquivo(char nome[]) {
    FILE* aux;
    aux = fopen(nome, "r+b");
    if (aux == NULL)
        aux = fopen(nome, "w+b");
    return aux;
}

void fecharArquivo(FILE* arq) {
    FILE* novo;
    CARRO carro;

    novo = fopen("novo.dat", "w+b");
    rewind(arq);

    while (fread(&carro, sizeof(CARRO), 1, arq) == 1) {
        if (carro.status == 1) {
            fwrite(&carro, sizeof(CARRO), 1, novo);
        }
    }

    fclose(arq);
    fclose(novo);

    remove("carros.dat");
    rename("novo.dat", "carros.dat");
}

void criarIndice(FILE* arq, NO* tabelaHashing[]) {
    CARRO carro;
    int pos = 0;
    rewind(arq);

    while (fread(&carro, sizeof(CARRO), 1, arq) == 1) {
        if (carro.status == 1) {
            inserirTabelaHash(tabelaHashing, carro.placa, pos);
        }
        pos++;
    }
}

void desalocarIndice(NO* tabelaHashing[]) {
    NO *aux, *temp;
    for (int i = 0; i < N; i++) {
        aux = tabelaHashing[i];
        while (aux != NULL) {
            temp = aux;
            aux = aux->prox;
            free(temp);
        }
        tabelaHashing[i] = NULL;
    }
}

void exibirOpcoes() {
    printf("Opções \n");
    printf("1 - Cadastrar um carro \n");
    printf("2 - Consultar carro \n");
    printf("3 - Alterar dados do carro \n");
    printf("4 - Remover carro \n");
    printf("5 - Exibir carros cadastrados \n");
    printf("0 - Encerrar programa \n");
    printf("Informe a opcao: ");
}

int buscar(NO* tabelaHashing[], char placa[]) {
    int indice = hashing(placa);
    NO* aux = tabelaHashing[indice];
    while (aux != NULL) {
        if (strcmp(aux->placa, placa) == 0) {
            return aux->posicao;
        }
        aux = aux->prox;
    }
    return -1;
}

void inserirTabelaHash(NO* tabelaHashing[], char placa[], int pos) {
    int indice = hashing(placa);
    NO* novo = (NO*)malloc(sizeof(NO));
    strcpy(novo->placa, placa);
    novo->posicao = pos;
    novo->prox = NULL;

    NO* aux = tabelaHashing[indice];
    NO* ant = NULL;
    while (aux != NULL && strcmp(aux->placa, placa) < 0) {
        ant = aux;
        aux = aux->prox;
    }

    if (ant == NULL) {
        novo->prox = tabelaHashing[indice];
        tabelaHashing[indice] = novo;
    } else {
        novo->prox = ant->prox;
        ant->prox = novo;
    }
}

void removerTabelaHash(NO* tabelaHashing[], char placa[], int posTabela) {
    NO *aux = tabelaHashing[posTabela], *ant = NULL;
    while (aux != NULL && strcmp(aux->placa, placa) != 0) {
        ant = aux;
        aux = aux->prox;
    }
    if (aux != NULL) {
        if (ant == NULL) {
            tabelaHashing[posTabela] = aux->prox;
        } else {
            ant->prox = aux->prox;
        }
        free(aux);
    }
}

int hashing(char placa[]) {
    int h = 0;
    for (int i = 0; i < strlen(placa); i++) {
        h = (h * 31 + placa[i]) % N;
    }
    return h;
}

void cadastrar(FILE* arq, NO* tabelaHashing[]) {
    CARRO carro;
    int pos;
    printf("Informe a placa do carro: ");
    scanf("%s", carro.placa);
    fflush(stdin);

    if (buscar(tabelaHashing, carro.placa) != -1) {
        printf("Carro ja cadastrado!\n");
        return;
    }

    printf("Informe a marca do carro: ");
    scanf("%s", carro.marca);
    fflush(stdin);
    printf("Informe o modelo do carro: ");
    scanf("%s", carro.modelo);
    fflush(stdin);
    printf("Informe a cor do carro: ");
    scanf("%s", carro.cor);
    fflush(stdin);

    carro.status = 1;

    fseek(arq, 0, SEEK_END);
    pos = ftell(arq) / sizeof(CARRO);
    fwrite(&carro, sizeof(CARRO), 1, arq);

    inserirTabelaHash(tabelaHashing, carro.placa, pos);
}

void consultar(FILE* arq, NO* tabelaHashing[]) {
    char placa[8];
    int pos;
    CARRO carro;
    printf("Informe a placa do carro: ");
    scanf("%s", placa);
    fflush(stdin);

    pos = buscar(tabelaHashing, placa);
    if (pos == -1) {
        printf("Carro nao cadastrado!\n");
        return;
    }

    fseek(arq, pos * sizeof(CARRO), SEEK_SET);
    fread(&carro, sizeof(CARRO), 1, arq);

    printf("Marca: %s\n", carro.marca);
    printf("Modelo: %s\n", carro.modelo);
    printf("Cor: %s\n", carro.cor);
}

void alterar(FILE* arq, NO* tabelaHashing[]) {
    char placa[8];
    int pos;
    CARRO carro;
    printf("Informe a placa do carro: ");
    scanf("%s", placa);
    fflush(stdin);

    pos = buscar(tabelaHashing, placa);
    if (pos == -1) {
        printf("Carro nao cadastrado!\n");
        return;
    }

    fseek(arq, pos * sizeof(CARRO), SEEK_SET);
    fread(&carro, sizeof(CARRO), 1, arq);

    printf("Marca atual: %s\n", carro.marca);
    printf("Informe a nova marca do carro: ");
    scanf("%s", carro.marca);
    fflush(stdin);
    printf("Modelo atual: %s\n", carro.modelo);
    printf("Informe o novo modelo do carro: ");
    scanf("%s", carro.modelo);
    fflush(stdin);
    printf("Cor atual: %s\n", carro.cor);
    printf("Informe a nova cor do carro: ");
    scanf("%s", carro.cor);
    fflush(stdin);

    fseek(arq, pos * sizeof(CARRO), SEEK_SET);
    fwrite(&carro, sizeof(CARRO), 1, arq);
}

void remover(FILE* arq, NO* tabelaHashing[]) {
    char placa[8];
    int pos;
    CARRO carro;
    printf("Informe a placa do carro: ");
    scanf("%s", placa);
    fflush(stdin);

    pos = buscar(tabelaHashing, placa);
    if (pos == -1) {
        printf("Carro nao cadastrado!\n");
        return;
    }

    fseek(arq, pos * sizeof(CARRO), SEEK_SET);
    fread(&carro, sizeof(CARRO), 1, arq);

    printf("Marca: %s\n", carro.marca);
    printf("Modelo: %s\n", carro.modelo);
    printf("Cor: %s\n", carro.cor);
    printf("Deseja realmente remover este carro? (1-Sim, 0-Nao): ");
    int confirm;
    scanf("%d", &confirm);
    fflush(stdin);

    if (confirm == 1) {
        carro.status = 0;
        fseek(arq, pos * sizeof(CARRO), SEEK_SET);
        fwrite(&carro, sizeof(CARRO), 1, arq);
        removerTabelaHash(tabelaHashing, placa, hashing(placa));
    }
}

void exibirCadastro(FILE* arq) {
    CARRO carro;
    int contador = 1;
    rewind(arq);

    while (fread(&carro, sizeof(CARRO), 1, arq) == 1) {
        if (carro.status == 1) {
            printf("Carro %d:\n", contador);
            printf("Placa: %s\n", carro.placa);
            printf("Marca: %s\n", carro.marca);
            printf("Modelo: %s\n", carro.modelo);
            printf("Cor: %s\n", carro.cor);
            printf("-----------------------\n");
            contador++;
        }
    }
}

//1 fecharArquivo: Esta função cria um novo arquivo, copia todos os registros ativos para ele, remove o arquivo antigo e renomeia o novo arquivo.
//2 criarIndice: Lê o arquivo e insere todas as chaves ativas na tabela de hashing.
//3 desalocarIndice: Libera a memória alocada para a tabela de hashing.
//4 buscar: Procura uma placa na tabela de hashing e retorna a posição no arquivo.
//5 inserirTabelaHash: Insere uma nova chave e posição na tabela de hashing.
//6 removerTabelaHash: Remove uma chave da tabela de hashing.
//7 hashing: Calcula o valor de hash para uma placa usando a permutação de caracteres.
//8 cadastrar: Insere um novo carro no arquivo e na tabela de hashing.
//9 consultar: Exibe os dados de um carro a partir da sua placa.
//10 alterar: Permite alterar os dados de um carro.
//11 remover: Marca um carro como removido no arquivo e remove a chave da tabela de hashing.
//12 exibirCadastro: Exibe todos os carros ativos no arquivo.
