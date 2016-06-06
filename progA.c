#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <unistd.h>

int main(int argc, char** argv){
	int i,j,ji,jj,id1,id2,m,n,rc,status,pid;
	if(argc != 3)
	{
		printf("Formato errado, por favor use./nome_do_arquivo m n\n");
		exit(-1);
	}
	m = atoi(argv[1]);
	n = atoi(argv[2]);

	pid = getpid();
	printf("-------------------------------\nPid do processo principal = %d\n-------------------------------\n",pid);

	ji = 0;
	for (i = 0; i<=m ; i++){
		id1 = fork();
		if(id1){
			pid = getpid();
			printf("Pai %d teve filho %d\n", pid, id1);
			ji = ji + 1;
			jj = 0;
			for (j = 0; j<=n; j++){
				id2 = fork();

				if(id2){
					pid = getpid();
					printf("Pai %d teve filho %d\n", pid, id2);
					jj = jj + 1;
				}else{
					if(ji > 0) ji = 0;
					if(jj > 0) jj = 0;
				}
				//ji = ji + jj;
			}
			ji = ji + jj;
		}else{
			if(ji > 0) ji = 0;
			if(jj > 0) jj = 0;
		}
	}
	if ((id1 || id2) != 0){
		pid = getpid();
		printf("Total de Filhos do Processo %d: %d\n",pid, ji);
		for(i=1;i<=ji;i++){
			rc = wait(&status);
			if (rc > 0){
				printf("%d, Processo Filho do Pai %d terminou OK\n", rc, pid);
			}
			else{
				printf("%d, Processo Filho do Pai %d NAO terminou OK\n", rc, pid);
			}
		}
	}
	exit(0);
}
