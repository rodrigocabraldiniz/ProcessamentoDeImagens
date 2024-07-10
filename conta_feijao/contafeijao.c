/*=============================================================
 *           UNIFAL = Universidade Federal de Alfenas.
 *             BACHARELADO EM CIENCIA DA COMPUTACAO.
 * Trabalho..: Contagem de feijoes
 * Professor.: Luiz Eduardo da Silva
 * Aluno.....: Rodrigo Cabral Diniz - 2022.1.08.037
 * Data......: 20/05/2024
 *=============================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils/imagelib.h"

// Função para limiarização
image limiarizar(image In)
{
    image Out = img_clone(In);
    int limiar = 95;
    for (int i = 0; i < In->nr * In->nc; i++)
    {
        Out->px[i] = (In->px[i] < limiar) ? 1 : 0;
    }
    return Out;
}

void ordena(int *vet, int tam)
{
    int aux;
    for (int i = 1; i < tam; i++)
    {
        for (int j = 0; j < tam - i; j++)
        {
            if (vet[j] > vet[j + 1])
            {
                aux = vet[j];
                vet[j] = vet[j + 1];
                vet[j + 1] = aux;
            }
        }
    }
}

void excluirRepetidos(int *parent, int numLabel)
{
    int componentesRepetidos[numLabel];
    int componentes[numLabel];
    int atual = 0;
    for (int i = 0; i < numLabel; i++)
    {
        componentesRepetidos[i] = parent[i];
    }
    ordena(componentesRepetidos, numLabel);
    int k = 0;
    for (int i = 0; i < numLabel; i++)
    {
        if (atual != componentesRepetidos[i])
        {
            atual = componentesRepetidos[i];
            componentes[k] = componentesRepetidos[i];
            k++;
        }
    }
    printf("#componentes= %3.0d\n", k);
}

int find(int parent[], int i)
{
    while (parent[i] != i)
        i = parent[i];
    return i;
}

void Union(int parent[], int i, int j)
{
    int x = find(parent, i);
    int y = find(parent, j);
    parent[y] = x;
}

void label(image In)
{
    int nr = In->nr;
    int nc = In->nc;
    int *px = In->px;
    int numLabel = 0;
    int *parent=malloc(nc*nr*sizeof(int));
    for (int i = 0; i < nr * nc; i++)
        parent[i] = i;
    for (int i = 1; i < nr; i++)
        for (int j = 1; j < nc; j++)
        {
            int p = px[i * nc + j];
            int r = px[(i - 1) * nc + j];
            int t = px[i * nc + j - 1];
            if (p != 0)
            {
                if (r == 0 && t == 0)
                    px[i * nc + j] = ++numLabel;
                if (r != 0 && t == 0)
                    px[i * nc + j] = r;
                if (r == 0 && t != 0)
                    px[i * nc + j] = t;
                if (r != 0 && t != 0 && t == r)
                    px[i * nc + j] = r;
                if (r != 0 && t != 0 && t != r)
                {
                    px[i * nc + j] = t;
                    Union(parent, r, t);
                }
            }
        }
    excluirRepetidos(parent, numLabel);
    for (int i = 0; i < nr * nc; i++)
        In->px[i] = find(parent, In->px[i]);
    In->ml = numLabel;
}

void msg(char *s)
{
    printf("\nContagem de feijões");
    printf("\n-------------------------------");
    printf("\nUso:  %s  nome-da-imagem.pgm\n\n", s);
    exit(1);
}

/*-------------------------------------------------------------------------
 * Função principal
 *-------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    char nameIn[100], nameOut[100];
    if (argc < 2)
        msg(argv[0]);

    char *input_filename = argv[1];
    image In, Out;

    //-- ler a imagem
    In = img_get(input_filename, 2);
    img_name(input_filename,nameIn,nameOut,2,1);

    if (!In)
    {
        fprintf(stderr, "Erro ao carregar a imagem: %s\n", input_filename);
        return 1;
    }

    //-- aplicar limiarização
    Out = limiarizar(In);

    //-- rotulação de componentes conexos
    label(Out);

    //-- salvar a imagem resultante da rotulação (opcional)
    img_put(Out, nameOut, 1);

    //-- liberar memória
    img_free(In);
    img_free(Out);

    return 0;
}
