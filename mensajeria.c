// Redes de Computadoras - Curso 1er Semestre 2018
// Tecnologo en Informatica FIng - CETP
//
// Obligatorio  - Programacion con Sockets
// Sistema basico de Mensajeria 

// Integrantes:
//	Detallar aqui, nombre, apellido y documento de todos los integrantes del grupo.

#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sys/socket.h>

#define MAX_LARGO_MENSAJE 255
#define MAX_NOMBRE 25
#define MAX_COMANDO 65
#define MAX_LARGO_ARCHIVO 65535

using namespace std;

void manejadorSenhales (int signal)
// Manejador de las senhales.
// Aca se debe implementar la accion a realizar cuando se recibe la senhal
// Deberia haber un manejador de senhales para cada hijo si hacen cosas distintas
{
	if (signal == SIGINT)
	{
		cout << "\33[46m\33[31m[" << getpid() << "]" << " SIGINT CTRL+C recibido\33[00m\n"; 
	}
	if (signal == SIGTERM)
	{
		cout << "\33[46m\33[31m[" << getpid() << "]" << " SIGTERM Terminacion de programa\33[00m\n";
	}
	if (signal == SIGSEGV)
	{
		cout << "\33[46m\33[31m[" << getpid() << "]" << " SIGSEGV violacion de segmento\33[00m\n";
	}
	if (signal == SIGCHLD)
	{
		cout << "\33[46m\33[31m[" << "]" << " SIGCHLD \33[00m\n";
	}
	if (signal == SIGPIPE)
	{
		cout << "\33[46m\33[31m[" << getpid() << "]" << " SIGPIPE \33[00m\n";
	}
	if (signal == SIGKILL)
	{
		cout << "\33[46m\33[31m[" << getpid() << "]" << " SIGKILL \33[00m\n";
	}
	if (signal == SIGSEGV)
	{
		cout << "\33[46m\33[31m[" << getpid() << "]" << " SIGSEGV violacion de segmento\33[00m\n";
	}
	exit(1);
}

int main(int argc, char * argv[])
// En argc viene la cantidad de argumentos que se pasan,
// si se llama solo al programa, el nombre es el argumento 0
// En nuestro caso:
//      - argv[0] es el string "mensajeria", puede cambiar si se llama con otro path.
//      - argv[1] es el puerto de escucha de mensajeria.
//      - argv[2] es el ip del servidor de autenticacion.
//      - argv[3] es el puerto del servidor de autenticacion.
{

	if (argc < 4)
	{
			cout << "\33[46m\33[31m[ERROR]:" << " Faltan argumentos: port, ipAuth, portAuth.\33[00m\n";
			exit (1);
	}



//	cout << "PUERTO MENSAJERIA: " << argv[1] << endl;
//	cout << "IP AUTENTICACION: " << argv[2] << endl;
//	cout << "PUERTO AUTENTICACION: " << argv[3] << endl;

	// Estructuras para el manejo de Senhales
	// Deberia haber un manejador de senhales para cada hijo si hacen cosas distintas
	// *********************************
	struct sigaction sa;
	memset (&sa, 0, sizeof (sa));
	sa.sa_handler = &manejadorSenhales;

	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGPIPE, &sa, NULL);
	sigaction(SIGSEGV, &sa, NULL);
	sigaction(SIGKILL, &sa, NULL);
	signal(SIGALRM, SIG_IGN);
	// **********************************

	char usuario[MAX_NOMBRE];
	char clave[MAX_NOMBRE];
	char comando[MAX_COMANDO];
	char uspw[MAX_COMANDO];
		
	


	cout << "Usuario: ";
	cin >> usuario;
	cout << "Clave: ";
	cin >> clave;


	strcpy (comando, "echo -n \"");
	strcat (comando, clave);
	strcat (comando, "\" | md5sum > md5");

//	cout << "COMANDO A EJECUTAR:" << comando << endl;
	
	system(comando);

	FILE * fp;
	
	fp = fopen ( "md5", "r");
	fgets(comando, 33, fp);

	fclose(fp);

//	cout << "MD5 LEIDO DESDE ARCHIVO: " << comando << endl;

	strcpy (uspw, usuario);
	strcat (uspw, "-");
	strcat (uspw, comando);
	strcat (uspw, "\r\n");

//	cout << "MENSAJE A ENVIAR: " << uspw << endl;



	int fd, numbytes;       
	/* ficheros descriptores */

	char buf[MAX_LARGO_MENSAJE];  
   /* en donde es almacenará el texto recibido */

	struct hostent *he;         
   /* estructura que recibirá información sobre el nodo remoto */

	struct sockaddr_in server;  
   /* información sobre la dirección del servidor */


	if ((he=gethostbyname(argv[2]))==NULL){       
   /* llamada a gethostbyname() */
		cout << "\33[46m\33[31m[ERROR]:" << " gethostbyname()\33[00m\n";
		exit(-1);
	}

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1 ){  
      /* llamada a socket() */
		cout << "\33[46m\33[31m[ERROR]:" << " socket()\33[00m\n";
		exit(-1);
	}

	server.sin_family = AF_INET;
   	server.sin_port = htons(atoi(argv[3]));
   /* htons() es necesaria nuevamente ;-o */
	server.sin_addr = *((struct in_addr *)he->h_addr);  
   /*he->h_addr pasa la información de ``*he'' a "h_addr" */
	bzero(&(server.sin_zero),8);

	
	int error = connect(fd, (struct sockaddr *)&server, sizeof(struct sockaddr));
	if(error ==-1){ 
      /* llamada a connect() */
		cout << "\33[46m\33[31m[ERROR]:" << " connect()\33[00m\n";
		exit(-1);
	}
	
	numbytes = recv(fd, buf, MAX_LARGO_MENSAJE,0);

	if (numbytes == -1){  
      /* llamada a recv()  */
		cout << "\33[46m\33[31m[ERROR]:" << " recv()\33[00m\n";
		exit(-1);
	}
	
	buf[numbytes - 2] = '\0';

//	cout << "MENSAJE RECIBIDO: " << buf << endl;

	if (strcmp(buf, "Redes 2018 - Obligatorio 2 - Autenticacion de Usuarios") != 0){
		cout << "\33[46m\33[31m[ERROR]:" << " Servidor de autenticacion incorrecto.\33[00m\n";
		exit(-1);
	}


	send(fd, uspw, strlen(uspw), 0); 

	numbytes = recv(fd, buf, MAX_LARGO_MENSAJE,0);

	if (numbytes == -1){  
      /* llamada a recv()  */
		cout << "\33[46m\33[31m[ERROR]: recv()\33[00m\n";
		exit(-1);
	}
	
	buf[numbytes - 2] = '\0';

	if (strcmp (buf,"NO") == 0){
		cout << "\33[46m\33[31m[ERROR]: Imposible autenticar, usuario no valido.\33[00m\n";
		exit(-1);
	}else if (strcmp (buf,"SI") == 0){
		numbytes = recv(fd, buf, MAX_LARGO_MENSAJE,0);
		if (numbytes == -1){
	      /* llamada a recv()  */
			cout << "\33[46m\33[31m[ERROR]: connect()\33[00m\n";
			exit(-1);
		}
		buf[numbytes - 2] = '\0';
		cout << "Bienvenid@ " << buf << endl;
	}else{
		cout << "\33[46m\33[31m[ERROR]: Error en protocolo de autenticacion.\33[00m\n";
		exit(-1);
	}


	close(fd);








	
	cout << "\33[34mRedes de Computadoras 2018\33[39m: Sistema de Mensajeria.\nEscuchando en el puerto " << argv[1] << ".\nProceso de pid: " << getpid() << ".\n";


	unsigned int pid = fork();

	if (pid < 0){
		cout << "\33[46m\33[31m[ERROR]:" << " Imposible Bifurcar.\33[00m\n";
		exit (1);
	}

	if (pid == 0){
		printf("\33[34mRx\33[39m: Iniciada parte que recepciona mensajes. Pid %d\n", getpid());  
		while (true){

		}

	}

	if (pid > 0){
		printf("\33[34mTx\33[39m: Iniciada parte que transmite mensajes. Pid %d\n", getpid());
		while (true){

		}

	}
	
	// Se debe Bifurcar el Programa
	// Al iniciar la parte que recibe imprimir lo siguiente descomentandolo

	// Al iniciar la parte que transmite imprimir lo siguiente descomentandolo

	
	// Es indistinto si el padre transmite y el hijo recibe, o viceversa, lo que si al ser distintos porcesos, van a tener distinto pid.

	// Si se crean nuevos procesos para distintas funcionalidades, agregar un mensaje como el de arriba al principio de su ejecucion
	// describiendo su funcionalidad.
	
	// Familiarizarse con los comandos de UNIX ps, ps -as, kill, etc.
	
}
