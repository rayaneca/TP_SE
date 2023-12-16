/*
 bouafia rayane  
*/
//section include..
#include <unistd.h>     /* Symbolic Constants */
#include <stdio.h>      /* Input/Output */
#include <stdlib.h>     /* General Utilities */
#include <pthread.h>    /* POSIX Threads */
#include <semaphore.h>  /* semaphore */

//define
#define N 4 // places dans le buffer

//variable globales 
int n1, m1, n2, m2;
//les matrices
int **B;
int **C;
int **A;
//le tampon

typedef struct T
{
    int *data;
    int *posX;
    int *posY;
    int count;
    int in;
    int out;
    int produced;
    int consumed;
} T;
T buffer;

//pour la synchronisation 
pthread_mutex_t mutex;
sem_t empty;
sem_t full;

// Producer
void *producer(void *arg)
{
    int index = *(int *)arg;
    int item = 0;

    for (int i = 0; i < m2; i++)
    {
        item = 0;
        for (int j = 0; j < m1; j++)
            item += B[index][j] * C[j][i];

        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        buffer.data[buffer.in] = item;
        buffer.posX[buffer.in] = index;
        buffer.posY[buffer.in] = i;
        buffer.in = (buffer.in + 1) % N;
        buffer.count++;
        buffer.produced++;

       

        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }

    // printf("**Producer [%d] Finished.\n", index + 1);
    pthread_exit(NULL);
}

// Consumer
void *consumer(void *arg)
{
    int index = *(int *)arg;
    int item = 0;

    while (!shouldExit)
    {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        if (shouldExit)
        {
            pthread_mutex_unlock(&mutex);
            break;
        }

        item = buffer.data[buffer.out];
        A[buffer.posX[buffer.out]][buffer.posY[buffer.out]] = item;
        buffer.out = (buffer.out + 1) % N;
        buffer.count--;
        buffer.consumed++;

        // printf("Consumer [%d] Consumed item: %d\n", index + 1, item);

        shouldExit = buffer.consumed == n1 * m2;
        if (shouldExit)
            for (int i = 0; i < N; i++)
                sem_post(&full);

        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }

   
    pthread_exit(NULL);
}

int Main ()
{
    printf("Enter number of rows and cols in matrix B: ");
    scanf("%d %d", &n1, &m1);
    printf("Enter number of rows and cols in matrix C: ");
    scanf("%d %d", &n2, &m2);
    if (m1 != n2)
    {
        perror("errur  B*C is not possible .\n");
        exit(EXIT_FAILURE);
    }
     // Allocate memory for matrix B
    B = (int **)malloc(n1 * sizeof(int *));
    for (int i = 0; i < n1; i++) {
        B[i] = (int *)malloc(m1 * sizeof(int));
    }

    // Allocate memory for matrix C
    C = (int **)malloc(n2 * sizeof(int *));
    for (int i = 0; i < n2; i++) {
        C[i] = (int *)malloc(m2 * sizeof(int));
    }
    A = (int **)malloc(n1 * sizeof(int *));
    for (int i = 0; i < n1; i++)
    {
        A[i] = (int *)malloc(m2 * sizeof(int));
    }
        // Fill matrix B
    for (int i = 0; i < n1; i++)
        for (int j = 0; j < m1; j++)
            B[i][j] = rand() % 10;

    // Fill matrix C
    for (int i = 0; i < n2; i++)
        for (int j = 0; j < m2; j++)
            C[i][j] = rand() % 10;
// Initialisation
sem_init(&mutex,0,1);//exclusion mutuelle 
sem_init(&empty, 0 , N);  // buffer vide
sem_init(&full, 0 , 0);   // buffer vide
pthread_mutex_init(&mutex, NULL);
sem_init(&empty, 0, N);
//creation des threads

    // Declare threads
    pthread_t producers_t[n1];
    pthread_t consumer_t[N];

    // Create threads
    for (int i = 0; i < n1; i++)
    {
        int *index = malloc(sizeof(int));
        *index = i;
        if (pthread_create(&producers_t[i], NULL, producer, index) != 0)
        {
            perror("Thread creation failed.\n");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < N; i++)
    {
        int *index = malloc(sizeof(int));
        *index = i;
        if (pthread_create(&consumer_t[i], NULL, consumer, index) != 0)
        {
            perror("Thread creation failed.\n");
            exit(EXIT_FAILURE);
        }
    }


//attente des threads

    // Join threads
    for (int i = 0; i < n1; i++)
        pthread_join(producers_t[i], NULL);

    for (int i = 0; i < N; i++)
        pthread_join(consumer_t[i], NULL);

    // Pretty print matrix A
    printf("\n| Matrix A\n");
    for (int i = 0; i < n1; i++)
    {
        for (int j = 0; j < m2; j++)
            printf("%d  ", A[i][j]);
        printf("\n");
    }



//destruction...
   pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);


return 0;
}
