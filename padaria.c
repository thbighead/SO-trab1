#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

sem_t mutex_vend,mutex_client;
sem_t *vende;

int n=0, m=0;
int prox_senha_cliente=0;
int prox_senha_padaria=0;
int atendidos = 0;
int *senhas_chamadas;
int *senhas_clientes;


void *vendedor (void * arg)
{
	int tid = *((int *) arg);
	int i, cliente;
	free(arg);
	printf("Padeiro %d chegou a padaria\n",tid);
	sem_wait(&mutex_vend);
	while(atendidos<n)
	{
		senhas_chamadas[tid]=prox_senha_padaria++;
		sem_post(&mutex_vend);
		printf("Padeiro %d chamou a senha %d e espera o cliente\n",tid,senhas_chamadas[tid]);
		sem_wait(&vende[tid]);
		if(atendidos>=n)
		{
			printf("Padeiro %d terminou a venda e está indo embora\n",tid);
			break;
		}
		printf("Padeiro %d está identificando o cliente\n",tid);
		for(i=0;i<n;i++)
		{
			if(senhas_clientes[i]==senhas_chamadas[tid])
			{
				cliente=i;
				break;
			}
		}
		printf("Padeiro %d atendendo Cliente %d\n",tid,cliente);
		sem_wait(&vende[tid]);
		sem_wait(&mutex_vend);
		printf("Padeiro %d terminou a venda, iniciando outra\n",tid);
	}
	sem_post(&mutex_vend);
	for(i=0;i<m;i++)
	{
		sem_post(&vende[i]);
	}
	pthread_exit(NULL);
}

void *cliente (void * arg)
{
	int tid = *((int *) arg);
	int i,padeiro,minha_senha,atendido=0;
	free(arg);
	printf("Cliente %d chegou a padaria\n",tid);
	sem_wait(&mutex_client);
	minha_senha=prox_senha_cliente++;
	printf("Cliente %d pegou a senha %d\n",tid,minha_senha);
	sem_post(&mutex_client);
	printf("Cliente %d está procurando o padeiro\n",tid);
	while(!atendido)
	{
		for(i=0;i<m;i++)
		{
			if(senhas_chamadas[i]==minha_senha)
			{
				padeiro=i;
				atendido=1;
				break;
			}
		}
	}
	printf("Cliente %d achou o Padeiro %d\n",tid, padeiro);
	sem_post(&vende[padeiro]);
	sleep(3);
	sem_post(&vende[padeiro]);
	sem_wait(&mutex_vend);
	atendidos++;
	sem_post(&mutex_vend);
	printf("Cliente %d foi atendido e está indo embora\n",tid);
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
	senhas_chamadas=malloc(m*sizeof(int));
	senhas_clientes=malloc(n*sizeof(int));
	pthread_t *vendedores = (pthread_t*) malloc(m * sizeof(pthread_t));
	pthread_t *clientes = (pthread_t*) malloc(n * sizeof(pthread_t));
	vende=malloc(m*sizeof(sem_t));
	sem_init(&mutex_vend,0,1);
	sem_init(&mutex_client,0,1);
	for(i=0;i<m;i++)
	{
		sem_init(&vende[i],0,0);
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
