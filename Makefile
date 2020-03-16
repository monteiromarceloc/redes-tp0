all:
	gcc -Wall ./src/servidor.c -o servidor
	gcc -Wall ./src/cliente-aluno.c -o cliente-aluno
	gcc -Wall ./src/cliente-prof.c -o cliente-prof
	./servidor

clean:
	rm -f servivor cliente-aluno cliente-prof 
