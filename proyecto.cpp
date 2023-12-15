#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>


#define Limite_Entrada  1024 // Cantidad limite de caracteres de la entrada
#define Limite_Tokens  64 // cantidad limite de tokens aceptados

// Funci贸n para leer comandos y ejecutarlos

int leer_comandos(char **tokens, int ejecucion_en_segundo_plano){
    
    pid_t pid, wpid;
	int status;

	pid = fork();

	if(pid==0){ //Proceso Hijo
	int i, redireccion_entrada = -1, redireccion_salida = -1;
  	
	// Buscar el operador de redirecci贸n "<"
  	 	 
   		for (i = 0; tokens[i] != NULL; i++) {
        	if (strcmp(tokens[i], "<") == 0) {
            	redireccion_entrada = i;
            	break;
       		 }
   		 }
   	// Buscar el operador de redirecci贸n ">"
   	
   		for ( i = 0; tokens[i] != NULL; i++){
     		 if (strcmp(tokens[i], ">") == 0) {
       		 redireccion_salida = i;
       		 break;
     			 }
   		 }
 		 
 	// Bloque para manejar la redirecci贸n de entrada
 		if (redireccion_entrada >= 0 && tokens[redireccion_entrada + 1] != NULL) {
   	 	FILE *archivo_seleccionado = fopen(tokens[redireccion_entrada + 1], "r");
   	 	if (archivo_seleccionado == NULL) {
   	     	perror("No se puede abrir el archivo seleccionado");
   	     	exit(EXIT_FAILURE);
   	 	}
   	 	// Redirigir la entrada est谩ndar al archivo seleccionado
   	 	dup2(fileno(archivo_seleccionado), STDIN_FILENO);
   	 	fclose(archivo_seleccionado);
   	 	// Eliminar el s铆mbolo de redirecci贸n y el nombre del archivo del array de tokens
   	 	tokens[redireccion_entrada] = NULL;
   	 	tokens[redireccion_entrada + 1] = NULL;
   	 	}

   	 // Bloque para manejar la redirecci贸n de salida
   	 
  		 if (redireccion_salida >= 0 && tokens[i + 1] != NULL) { // bloque para leer el comando o archivo a ejecutar
   		 FILE *archivo_seleccionado = fopen(tokens[i + 1], "w"); //variable para seleccionar el archivo
   		 if (archivo_seleccionado  == NULL) {
       		 perror("No se puede abrir el archivo seleccionado");
       		 exit(EXIT_FAILURE);
   		 }
   		 // Redirigir la salida est谩ndar al archivo seleccionado
   		 dup2(fileno(archivo_seleccionado), STDOUT_FILENO); // directiva para duplicar la el parametro pasado despues de > para poder utilizarlo en el comando respectivo
   		 fclose(archivo_seleccionado);
   		 // Eliminar el s铆mbolo de redirecci贸n del array de tokens
   		 tokens[i] = NULL; // Eliminar el s铆mbolo de redirecci贸n
   	 }
   	 
   	 
  	// Ejecutar el comando
   	 if (execvp(tokens[0], tokens) == -1) {
   		 perror("Ese comando no es valido");
   		 exit(EXIT_FAILURE);
   	 }
 	
	}else if(pid<0){ // Directiva cuando no se crea el proceso hijo
  		 perror("No se pudo crear el proceso Hijo");
  		 exit(EXIT_FAILURE);
  	 
	} else{ // Proceso Padre
        if (!ejecucion_en_segundo_plano) {
            do {
                // Esperar a que el proceso hijo termine
                wpid = waitpid(pid, &status, WUNTRACED);
            } while(!WIFEXITED(status) && !WIFSIGNALED(status));  // Bucle que mantiene al proceso padre en espera mientras se ejecuta el proceso hijo
        } else {
            // Proceso en segundo plano, imprimir PID y continuar
            printf("Proceso en segundo plano ejecut醤dose con PID: %d...\n", pid);
        }
    }
    
   return 1;    
}


int main() {
    
    char entrada[Limite_Entrada]; //Variable que guarda la entrada
    char *Tokens[Limite_Tokens]; //Variable que guarda los tokens

    // Bucle principal de la consola
    while (1){
  	 printf("Consola Creada> "); //Imprimir mensaje de la consola
  	 // Leer comandos ingresados
  	 fgets(entrada, sizeof(entrada), stdin); //leer comandos ingresados
  	 
    /*
  	 if (fgets(entrada, sizeof(entrada), stdin) == NULL) {
        	perror("Error al leer la entrada");
        	exit(EXIT_FAILURE);
	}
  	 */
  	 
  	 // Salir del bucle si se ingresa "salir"
  	 if(strcmp(entrada, "salir\n")==0 ){ //instruccion para romper el bucle
  		break;
  	 }
  	 
  	 int i =0;
  	 // Tokenizar la entrada
  	 Tokens[i] = strtok(entrada, " \t\n");
  	 while(Tokens[i] != NULL){ // bucle para leer espacios en blanco
  		 i++;
  		 Tokens[i] = strtok(NULL, " \t\n");
  	 }

    // Verificar si el 鷏timo token es "&" para ejecuci髇 en segundo plano
    int ejecucion_en_segundo_plano = 0;
    if (i > 0 && strcmp(Tokens[i - 1], "&") == 0) {
        ejecucion_en_segundo_plano = 1;
        Tokens[i - 1] = NULL; // Eliminar el token "&"
    }
  	 
  	 if (Tokens[0] != NULL) { //bloque que pasa los tokens al interprete de comandos
  			 // Ejecutar comandos o salir del programa
   		 if (strcmp(Tokens[0], "salir") == 0) {
       		 break;
   		 } else {
       		 leer_comandos(Tokens, ejecucion_en_segundo_plano);
   		 }
   	 }
  	 
    }

    return 0;
}
