#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

sem_t *fila,*atendimento;
sem_t mutex_clientes;

int m,n,senha=0;
int *cliente_atual;

void *vendedor (void * arg)
{
	int tid = *((int *) arg);
	int atendidos=0,cota,client;
	free(arg);
	cota=(n/m);
	printf("Padeiro %d chegou no caixa\n",tid);
	while(atendidos<cota)
	{
		printf("Padeiro %d chamou um cliente\n",tid);
		sem_post(&fila[tid]);
		sem_wait(&atendimento[tid]);
		sem_wait(&mutex_clientes);
		client=cliente_atual[tid];
		sem_post(&mutex_clientes);
		printf("Padeiro %d esta atendendo o cliente %d\n", tid, client);
		sem_wait(&atendimento[tid]);
		atendidos++;
		printf("Padeiro %d ira chamar o proximo cliente\n",tid);
	}
	printf("Padeiro %d cumpriu a cota e esta indo pra casa \n",tid);
	pthread_exit(NULL);
}

void *cliente (void * arg)
{
	int tid = *((int *) arg);
	int minha_fila;
	free(arg);
	printf("Cliente %d chegou na fila\n",tid);
	sem_wait(&mutex_clientes);
	minha_fila=senha;
	senha=(senha+1)%m;
	sem_post(&mutex_clientes);
	printf("Cliente %d vai esperar na fila do padeiro %d\n",tid, minha_fila);
	sem_wait(&fila[minha_fila]);
	sem_wait(&mutex_clientes);
	cliente_atual[minha_fila]=tid;
	sem_post(&mutex_clientes);
	sem_post(&atendimento[minha_fila]);
	printf("Cliente %d sendo atendido pelo Padeiro %d\n",tid,minha_fila);
	sleep(3);
	sem_post(&atendimento[minha_fila]);
	printf("Cliente %d indo embora\n",tid);
	pthread_exit(NULL);
}

int main(int argc, char** argv) {
	int i;
	int *arg;

	if (argc != 3)
	{
		printf("Formato incorreto, por favor utilize ./nome_do_arquivo numero_de_vendedores numero_de_clientes\n");
		exit(-1);
	}
	m=atoi(argv[1]);
	n=atoi(argv[2]);
	pthread_t *vendedores = (pthread_t*) malloc(m * sizeof(pthread_t));
	pthread_t *clientes = (pthread_t*) malloc(n * sizeof(pthread_t));
	fila=malloc(m*sizeof(sem_t));
	atendimento=malloc(m*sizeof(sem_t));
	cliente_atual=malloc(m*sizeof(int));
	sem_init(&mutex_clientes,0,1);
	for(i=0;i<m;i++)
	{
		sem_init(&atendimento[i],0,0);
		sem_init(&fila[i],0,0);
	}
	if(clientes==NULL || vendedores==NULL)
	{
		printf("Erro de malloc\n");
		exit(-1);
	}
	for (i = 0; i < m; i++)
	{
		arg = malloc(sizeof(int));
		if (arg == NULL)
		{
			printf("Erro de malloc\n");
			exit(-1);
		}
		*arg = i;
		if (pthread_create(&vendedores[i], NULL, vendedor, (void*) arg))
		{
			printf("Erro: pthread_create dos atendentes\n");
			exit(-1);
		}
	}
	for (i = 0; i < n; i++)
	{
		arg = malloc(sizeof(int));
		if(arg == NULL)
		{
			printf("Erro de malloc\n");
			exit(-1);
		}
		*arg = i;
		if (pthread_create(&clientes[i], NULL, cliente, (void*) arg))
		{
			printf("Erro: pthread_create dos clientes\n");
			exit(-1);
		}
	}
	for (i = 0; i < m; i++)
	{
		if (pthread_join(vendedores[i], NULL))
		{
			printf("Erro: pthread_join\n");
			exit(-1);
		}
	}

	for (i = 0; i < n; i++)
	{
		if (pthread_join(clientes[i], NULL))
		{
			printf("Erro: pthread_join\n");
			exit(-1);
		}
	}
	pthread_exit(NULL);
}
