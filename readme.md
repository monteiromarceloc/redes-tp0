## Descrição

## Get Started
    gcc servidor.c -o servidor
    ./servidor

## Function Flow
    
    Server          Client

    socket()        socket()
    bind()
    listen()
    accept() <----- connect()
    recv() <------- send()
    send() -------> recv()
    close() <------ close()

**OBS:**
send() ~ print file
recv() ~ scan (trava e aguarda chegada de dado na rede)

## Usage (POSIX SOCKET)
```
int socket(int domain, int type, int protocol);
  domain:
    AF_INET : internet (usaremos esse)
    AF_UNIX : entre processos do PC
    AF_OSI
  type:
    SOCK_STREAM : facilita o tratamento de erros
    SOCK_DGRAM : 
  protocol: 0
```

**Abertura passiva - servidor**
```
int bind(int scky, struct sockaddr *addr, socklen_t addr_len);
int listen(int scky, int backlog);
int accept(int scky, struct sockaddr *addr, socklen_t addr_len);
  sckt : int da função socket
  struct sockaddr : endereço em cada familia
  {
    sa_family_t     sin_family     // AF_INET
    unsigned shot   int sin_port   // port: 51511 (16 bits, até 60k) | 0 (escolhe aleatório)
    struct in_addr  sin_addr       // ip: 127.0.0.1 | INADDR_ANY
  }
  backlog : numero de conexões pendentes suportadas // 10
```

**Abertura ativa - cliente**

**Especiais**

    inet_pton
    inet_ntop
    getaddrinfo
    getpeername

**Comunicação**
```
int send (int sckt, char *msg, int mlen, int flags)
  mlen : tamanho da mensagem
  flag // 0
  (o char \0 é enviado?)

int recv (int sckt, char *buf, int blen, int flags)
  blen : tamanho do buffer ( >= quantidade de bytes pra receber)
  flag // MSG_WAITALL : espera receber blen bytes
  (buffer overflow)
```

**Tratamento de erros**

    if ( (code=some_sunction()) < 0 ) {
      perror("some_function"); exit(1);
    }