#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

/*******************************************************************************************

    A atual abordagem vem a implementar uma combinação do algoritmo do KnapSack com o
    conhecimento que os profits, nesta questão serão dinâmicos. Ou seja, há que haver
    um novo calculo após a seleção de um ponto para um dado subset. Assim, teremos o
    novo ganho da área com um ponto j = {1,2,...,n}.

    Inicialmente, quando são dados os pontos, estes são ordenados por abcissa ascendente
    e ordenada descendente. Isto vai implicar que vamos percorrer o array de pontos tal
    como se fossemos percorrer no referencial cartesiano, isto é, da esquerda para a
    direita.Isto implica também que para a mesma abcissa, o ponto seguinte é menor, pelo
    que pode ser descartado.

    O principio para esta abordagem tem por base uma matriz n x k, em que n é o número
    de pontos dados em input e k o limite de combinações para o cálculo. Ou seja, para
    1200 pontos, se quisermos saber quais os 58 pontos que denotam a maior área então
    k = 58.

    A matriz é preenchida por linha, com um primeiro ciclo de i até n. Dentro, teremos o
    segundo ciclo que irá percorrer todas as colunas de 0 a k.

    A ideia é preencher da combinação mais baixa até k. Com isto quero dizer que, preenchendo
    de k=0 até k=kf (kf sendo o limite de de combinações). Para cada par x,y de cada linha
    o valor a preencher na matriz será sempre o que este ponto consegue maximizar à área
    combinada pelos outros pontos, pelo que para i=1, só se testa área desse ponto, pelo que
    em k = 0, estarão todas as áreas cujos pontos (sozinhos) consegues criar. Assim, a
    cada iteração a variável maximilian irá guardar sempre o valor máximo da coluna,
    dando-nos a área máxima para combinações de k.

    De modo a establecer melhores tempos, e sabendo que anteriormente já foram testadas algumas
    combinações, não há necessidade de realizá-las novamente, pelo que só devo estar preocupado
    em minimizar x e y de i, de forma a encontrar o limite superior ou inferior das áreas
    calculadas anteriormente. Deste modo conseguimos sempre, para k, saber como maximizamos as áreas
    anteriores e assim, arranjar os novos valores maximos. Isto é feito a ir buscar os valores da coluna
    anterior, terminando na diagonal anterior subset[p][j-1]. Neste valor subset[i][j] só é colocada
    a área máxima max_cover, e isto é que permite saber á proxima coluna que ali está presente a melhor
    combinação k-1 para aquele ponto. É importante referir que em somente na ultima coluna é que são testadas
    todas as possibilidades, mediante todo o calculo anterior, é garantido que em kf (k final) estaremos
    presentes com todas as combinações, pelo que teremos de percorrer todo n, para encontrar o máximo.
    Noutro ponto, existem ciclos que iriam ser custosos de maneiras a que são renegados,colocando
    os valores dessas celulas a 0. Isto permite criar uma matriz diagonal, sendo que só vamos precisar
    das áreas calculadas até [p][j-1]. Assim, i-j >= n-k+1, determina a triangular inferior e,
    i-(j) < 0 a triangular superior. Isto permite diminuiar drásticamente o tempo de execução.

    O facto de ser bottom-up tem exigencias em termos de memória, sendo que há que verificar sempre
    as combinações anteriores. Assim como, temos de começar da combinação mais primitiva de modo
    a evoluir até combinação k. Contudo, estes valores calculados não são irrelevantes. São
    necessários para o calculo da combinação k+1.
                                                                  03-05-2020.

                            André Loras Leite - 2015250489
                              uc2015250489@student.uc.pt
*******************************************************************************************/

/*******************************************************************************************
*
*                              Caracteristica do Ponto
*
********************************************************************************************/

typedef struct node{
    double x; /* coordenada x, calculada nas interseções*/
    double y; /* coordenada y, calculada nas interseções*/
    double x_f; /* coordenada x, sem calculos efetuados (estática)*/
    double y_f; /* coordenada y, sem calculos efetuados (estática)*/
}Node;

/*******************************************************************************************
*
*                                    QUICKSORT
*                            x ascendente & y descendente
*
********************************************************************************************/

void swap(Node* a, Node* b){
    Node t = *a;
    *a = *b;
    *b = t;
}

int subset(Node arr[], int low, int high){

    int j;
    Node pivot = arr[high];
    int i = (low - 1);

    for (j = low; j <= high- 1; j++){

        if (arr[j].x < pivot.x){
            i++;
            swap(&arr[i], &arr[j]);
        }else if(arr[j].x == pivot.x){
            if (arr[j].y > pivot.y){
                i++;
                swap(&arr[i], &arr[j]);
            }
        }

    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quickSort(Node arr[], int low, int high){

    if (low < high){
        int pi = subset(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

/*******************************************************************************************
*
*                                BOTTOM-UP APPROACH
*
********************************************************************************************/

void best_area(Node *points, int n, int k){

    int i,j,p,fac;
    double max_cover = 0.0;
    double maximilian = 0.0;
    double subset[n][k];

    for(i = 0; i < n; i++){

        for(j = 0; j < k; j++){

            if(j == 0 && i-j < n-k+1){
                subset[i][j] = points[i].x * points[i].y;
                if(subset[i][j] > maximilian){
                    maximilian = subset[i][j];
                }
            }else{

                if(i-j >= n-k+1){ /* triangular inferior*/  /* minimizar calculo repetido */
                    subset[i][j] = 0; /* minimizar calculo irrelevante */
                }else if(i-(j) < 0){ /* triangular superior*/
                    subset[i][j] = 0; /* (R2) - estes valores têm de ser zero, de forma a não corromper o calculo em k, pois, se permanecer um valor aqui, há a possibilidade de somado a esse valor, a minimização considera este o melhor caso como combinações anteriores, o que é errado*/
                }else{ /* diagonal necessária para o cálculo de máximos até i-1*/
                    fac = 0;
                    for(p = 0; p < i; p++){ /*só verfica até i-1, pois queremos combinações com i*/

                        /*em  k-1 estão sempre as melhores combinações possiveis anteriores*/
                        if(fac==0){
                            max_cover = subset[p][j-1]; /*seleciona um máximo inicial para comparação*/
                        }

                        if(points[i].x_f > points[p].x_f){ /* minimizar x de modo a ter x no limiar da abcissa da área*/
                            if((points[i].x_f - points[p].x_f) < points[i].x){
                                points[i].x = points[i].x_f - points[p].x_f;
                            }
                        }else if(points[i].y_f > points[p].y_f){/* minimizar y de modo a ter y no limiar da ordenada da área*/
                            if((points[i].y_f - points[p].y_f) < points[i].y){
                                points[i].y = points[i].y_f - points[p].y_f;
                            }
                        }

                        if(subset[p][j-1] + (points[i].x * points[i].y) > max_cover){
                            max_cover = subset[p][j-1] +(points[i].x * points[i].y);
                        }

                        fac++;
                    }
                    if(max_cover > maximilian && j == k-1){
                        maximilian = max_cover; /* máximo na coluna */
                    }
                    subset[i][j] = max_cover;
                    points[i].x = points[i].x_f; /*restaura x para próximos cálculos*/
                    points[i].y = points[i].y_f; /*restaura y para próximos cálculos*/
                }
            }
        }
    }

    printf("%.12f\n",maximilian);

}

/*******************************************************************************************
*
*                                   INITIALIZER
*
********************************************************************************************/
void read_input(){

    int i = 0;
    int k,n;
    Node point;
    Node *points;
    scanf("%d %d",&n,&k);
    points = (Node *) malloc(sizeof(Node)*n);
    while(i < n){
        scanf("%lf %lf",&(point.x),&(point.y));
        point.x_f = point.x; point.y_f = point.y;
        points[i] = point;
        i++;
    }
    quickSort(points,0,n-1);
    best_area(points,n,k);
}


int main()
{
    read_input();
    return 0;

}
