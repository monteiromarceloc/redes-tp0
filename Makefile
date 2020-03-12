all:
	gcc -Wall servidor.c -o servidor
	gcc -Wall cliente-aluno.c -o cliente-aluno
	gcc -Wall cliente-prof.c -o cliente-prof
	./servidor

clean:
	rm -f servivor cliente-aluno cliente-prof 
