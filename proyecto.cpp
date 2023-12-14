#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define Limite_Entrada  1024 // Cantidad limite de caracteres de la entrada
#define Limite_Tokens  64 // cantidad limite de tokens aceptados

int leer_comandos(char **tokens){
    pid_t pid, wpid;
    int status;
    
    pid = fork();
    
    if(pid==0){ //Proceso Hijo
    

   	int i,redireccion = -1;
       	for ( i = 0; tokens[i] != NULL; i++){//Directiva para aceptar el caracter > en comandos
          	if (strcmp(tokens[i], ">") == 0) {
            	redireccion = i;
            	break;
          	}
       	}
      	 
       	if (redireccion >= 0 && tokens[i + 1] != NULL) { // bloque para leer el comando o archivo a ejecutar
        	FILE *archivo_seleccionado = fopen(tokens[i + 1], "w"); //variable para seleccionar el archivo
        	if (archivo_seleccionado  == NULL) {
            	perror("No se puede abrir el archivo seleccionado");
            	exit(EXIT_FAILURE);
        	}
        	dup2(fileno(archivo_seleccionado), STDOUT_FILENO); // directiva para duplicar la el parametro pasado despues de > para poder utilizarlo en el comando respectivo
        	fclose(archivo_seleccionado);
        	tokens[i] = NULL; // Eliminar el símbolo de redirección
    	}
   	 
    	if (execvp(tokens[0], tokens) == -1) {
        	perror("Ese comando no es valido");
        	exit(EXIT_FAILURE);
    	}
      	 
    }else if(pid<0){ // Directiva cuando no se crea el proceso hijo
   	perror("No se pudo crear el proceso Hijo");
    } else{ // Proceso Padre
   	do{
    	wpid = waitpid(pid, &status, WUNTRACED);
   	} while(!WIFEXITED(status) && !WIFSIGNALED(status));     //bucle que mantiene al proceso padre en espera mientras se ejecuta el proceso hijo
       
    }
    
   return 1;    
}


int main() {
    
	char entrada[Limite_Entrada]; //Variable que guarda la entrada
	char *Tokens[Limite_Tokens]; //Variable que guarda los tokens
 	//Variable bandera para controlar la ejecucion de la consola
    
	while (1){
   	 printf("Consola Creada> "); //Imprimir mensaje de la consola
   	 
   	 fgets(entrada, sizeof(entrada), stdin); //leer comandos ingresados
   	 
   	 if(strcmp(entrada, "salir\n")==0 ){ //instruccion para romper el bucle
   	   break;
   	 }
   	 
   	 int i =0;
   	 Tokens[i] = strtok(entrada, " \t\n");
   	 while(Tokens[i] != NULL){ // bucle para leer espacios en blanco
   		i++;
   		Tokens[i] = strtok(NULL, " \t\n");
   	 }
   	 
   	 if (Tokens[0] != NULL) { //bloque que pasa los tokens al interprete de comandos
        	if (strcmp(Tokens[0], "salir") == 0) {
            	break;
        	} else {
            	leer_comandos(Tokens);
        	}
    	}
   	 
	}

	return 0;
}
