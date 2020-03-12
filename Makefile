all:
	gcc -Wall servivor.c -o server
	gcc -Wall cliente-aluno.c -o client
	gcc -Wall cliente-prof.c -o client

clean:
	rm -f servivor cliente-aluno cliente-prof 
