/*
 bouafia rayane
 */

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

// Global variables
int n1, m1, n2, m2;
int **A;
int **B;
int **C;
int shouldExit = 0;

// Buffer
#define N 5 // Max number of items in buffer
typedef struct T
{
    int *d;
    int *X;
    int *Y;
    int count;
    int in;
    int out;
    int produced;
    int consumed;
} T;
T buffer;

// Semaphores
pthread_mutex_t mutex;
sem_t empty;
sem_t full;

// Producer
void *producer(void *arg)
{
    int fori = *(int *)arg;
    int item = 0;
	printf("((%d",fori);
    for (int i = 0; i < m2; i++)
    {
        item = 0;
        for (int j = 0; j < m1; j++)
            item += B[fori][j] * C[j][i];

        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        buffer.d[buffer.in] = item;
        buffer.X[buffer.in] = fori;
        buffer.Y[buffer.in] = i;
        buffer.in = (buffer.in + 1) % N;
        buffer.count++;
        buffer.produced++;

        // printf("Producer [%d] Produced item: %d\n", index + 1, item);

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

        item = buffer.d[buffer.out];
        A[buffer.X[buffer.out]][buffer.Y[buffer.out]] = item;
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

    // printf("**Consumer [%d] Finished.\n", index + 1);
    pthread_exit(NULL);
}



int main(int argc, char *argv[])
{
    // Seed the random number generator
    srand(time(NULL));

    // Ask user for n1,m1 for B[n1][m1] and n2,m2 for C[n2][m2]
    printf("Enter number of rows and cols in matrix B: ");
    n1=3;
    n2=3;
    m1=3;
    m2=3;

    // Make sure that multiplication is possible
    if (m1 != n2)
    {
        perror("errur of B*C is not possible.\n");
        exit(EXIT_FAILURE);
    }

     // Generate matrix B
    B = (int **)malloc(n1 * sizeof(int *));
    for (int i = 0; i < n1; i++)
        B[i] = (int *)malloc(m1 * sizeof(int));

   
    // Generate matrix C
    C = (int **)malloc(n2 * sizeof(int *));
    for (int i = 0; i < n2; i++)
        C[i] = (int *)malloc(m2 * sizeof(int));

    
    // Fill matrix B with random numbers
    for (int i = 0; i < n1; i++)
        for (int j = 0; j < m1; j++)
            B[i][j] = rand() % 10;

    // Fill matrix C with random numbers
    for (int i = 0; i < n2; i++)
        for (int j = 0; j < m2; j++)
            C[i][j] = rand() % 10;

    // Pretty print both matrices
    printf("\n| Matrix B\n");
    for (int i = 0; i < n1; i++)
    {
        for (int j = 0; j < m1; j++)
            printf("%d  ", B[i][j]);
        printf("\n");
    }
    printf("\n| Matrix C\n");
    for (int i = 0; i < n2; i++)
    {
        for (int j = 0; j < m2; j++)
            printf("%d  ", C[i][j]);
        printf("\n");
    }
    printf("\n\n");

    buffer.d = (int *)malloc(N * sizeof(int));
    buffer.X = (int *)malloc(N * sizeof(int));
    buffer.Y = (int *)malloc(N * sizeof(int));
    buffer.count = 0;
    buffer.in = 0;
    buffer.out = 0;
    buffer.produced = 0;
    buffer.consumed = 0;

    // Init matrix A
    A = (int **)malloc(n1 * sizeof(int *));
    for (int i = 0; i < n1; i++)
    {
        A[i] = (int *)malloc(m2 * sizeof(int));
    }
  
    // Init semaphores
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, N);
    sem_init(&full, 0, 0) ;
   

    // Declare threads
    pthread_t producers_t[n1];
    pthread_t consumer_t[N];

    // Create threads
    for (int i = 0; i < n1; i++)
    {
        int *index = malloc(sizeof(int));
        *index = i;
        pthread_create(&producers_t[i], NULL, producer, index);
      
    }

    for (int i = 0; i < N; i++)
    {
        int *index = malloc(sizeof(int));
        *index = i;
        pthread_create(&consumer_t[i], NULL, consumer, index) ;
       
    }

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

    // Cleanup
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);
   
    return 0;
}
