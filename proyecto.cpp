#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>


#define Limite_Entrada  1024 // Cantidad limite de caracteres de la entrada
#define Limite_Tokens  64 // cantidad limite de tokens aceptados

// Función para leer comandos y ejecutarlos

int leer_comandos(char **tokens, int ejecucion_en_segundo_plano) {
    pid_t pid, wpid;
    int status;

    int pipe_pos = -1;
    for (int i = 0; tokens[i] != NULL; i++) {
        if (strcmp(tokens[i], "|") == 0) {
            pipe_pos = i;
            break;
        }
    }

    if (pipe_pos >= 0) {
        // Tubería detectada
        char **tokens_left = tokens;
        char **tokens_right = &tokens[pipe_pos + 1];
        tokens[pipe_pos] = NULL;

        int pipe_fd[2];
        if (pipe(pipe_fd) == -1) {
            perror("Error al crear la tubería");
            exit(EXIT_FAILURE);
        }

        pid_t child_pid = fork();

        if (child_pid == 0) {
            // Proceso hijo (izquierda de la tubería)
            close(pipe_fd[0]); // Cerrar extremo de lectura no utilizado

            // Redirigir la salida estándar al extremo de escritura de la tubería
            dup2(pipe_fd[1], STDOUT_FILENO);
            close(pipe_fd[1]);

            // Ejecutar el comando de la izquierda
            if (execvp(tokens_left[0], tokens_left) == -1) {
                perror("Error al ejecutar el comando de la izquierda");
                exit(EXIT_FAILURE);
            }
        } else if (child_pid > 0) {
            // Proceso padre
            close(pipe_fd[1]); // Cerrar extremo de escritura no utilizado

            // Redirigir la entrada estándar al extremo de lectura de la tubería
            dup2(pipe_fd[0], STDIN_FILENO);
            close(pipe_fd[0]);

            // Ejecutar el comando de la derecha
            if (execvp(tokens_right[0], tokens_right) == -1) {
                perror("Error al ejecutar el comando de la derecha");
                exit(EXIT_FAILURE);
            }
        } else {
            perror("Error al crear el proceso hijo para la tubería");
            exit(EXIT_FAILURE);
        }
    } else {
		
		pid = fork();

		if(pid==0){ //Proceso Hijo
		int i, redireccion_entrada = -1, redireccion_salida = -1;
	  	
		// Buscar el operador de redirección "<"
	  	 	 
	   		for (i = 0; tokens[i] != NULL; i++) {
			if (strcmp(tokens[i], "<") == 0) {
		    	redireccion_entrada = i;
		    	break;
	       		 }
	   		 }
	   	// Buscar el operador de redirección ">"
	   	
	   		for ( i = 0; tokens[i] != NULL; i++){
	     		 if (strcmp(tokens[i], ">") == 0) {
	       		 redireccion_salida = i;
	       		 break;
	     			 }
	   		 }
	 		 
	 	// Bloque para manejar la redirección de entrada
	 		if (redireccion_entrada >= 0 && tokens[redireccion_entrada + 1] != NULL) {
	   	 	FILE *archivo_seleccionado = fopen(tokens[redireccion_entrada + 1], "r");
	   	 	if (archivo_seleccionado == NULL) {
	   	     	perror("No se puede abrir el archivo seleccionado");
	   	     	exit(EXIT_FAILURE);
	   	 	}
	   	 	// Redirigir la entrada estándar al archivo seleccionado
	   	 	dup2(fileno(archivo_seleccionado), STDIN_FILENO);
	   	 	fclose(archivo_seleccionado);
	   	 	// Eliminar el símbolo de redirección y el nombre del archivo del array de tokens
	   	 	tokens[redireccion_entrada] = NULL;
	   	 	tokens[redireccion_entrada + 1] = NULL;
	   	 	}

	   	 // Bloque para manejar la redirección de salida
	   	 
	  		 if (redireccion_salida >= 0 && tokens[i + 1] != NULL) { // bloque para leer el comando o archivo a ejecutar
	   		 FILE *archivo_seleccionado = fopen(tokens[i + 1], "w"); //variable para seleccionar el archivo
	   		 if (archivo_seleccionado  == NULL) {
	       		 perror("No se puede abrir el archivo seleccionado");
	       		 exit(EXIT_FAILURE);
	   		 }
	   		 // Redirigir la salida estándar al archivo seleccionado
	   		 dup2(fileno(archivo_seleccionado), STDOUT_FILENO); // directiva para duplicar la el parametro pasado despues de > para poder utilizarlo en el comando respectivo
	   		 fclose(archivo_seleccionado);
	   		 // Eliminar el símbolo de redirección del array de tokens
	   		 tokens[i] = NULL; // Eliminar el símbolo de redirección
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
		    printf("Proceso en segundo plano ejecut�ndose con PID: %d...\n", pid);
		}
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

    // Verificar si el �ltimo token es "&" para ejecuci�n en segundo plano
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
