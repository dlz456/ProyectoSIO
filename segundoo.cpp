#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define Limite_Entrada  1024 // Cantidad límite de caracteres de la entrada
#define Limite_Tokens  64 // Cantidad límite de tokens aceptados

// Función para leer comandos y ejecutarlos
int leer_comandos(char **tokens, int ejecucion_en_segundo_plano){
    pid_t pid, wpid;
    int status;

    pid = fork(); // Crear un nuevo proceso

    if(pid==0){ // Proceso Hijo
        // Código de redirección y ejecución sigue igual...

        // Ejecutar el comando
        if (execvp(tokens[0], tokens) == -1) {
            perror("Ese comando no es válido");
            exit(EXIT_FAILURE);
        }
    } else if(pid<0){ // Directiva cuando no se crea el proceso hijo
        perror("No se pudo crear el proceso Hijo");
        exit(EXIT_FAILURE);
    } else { // Proceso Padre
        if (!ejecucion_en_segundo_plano) {
            do {
                // Esperar a que el proceso hijo termine
                wpid = waitpid(pid, &status, WUNTRACED);
            } while(!WIFEXITED(status) && !WIFSIGNALED(status));  // Bucle que mantiene al proceso padre en espera mientras se ejecuta el proceso hijo
        } else {
            // Proceso en segundo plano, imprimir PID y continuar
            printf("Proceso en segundo plano ejecutándose con PID: %d...\n", pid);
        }
    }

    return 1;    
}

int main() {
    
    char entrada[Limite_Entrada]; // Variable que guarda la entrada
    char *Tokens[Limite_Tokens]; // Variable que guarda los tokens

    // Bucle principal de la consola
    while (1){
        printf("Consola Creada> "); // Imprimir mensaje de la consola

        fgets(entrada, sizeof(entrada), stdin); // Leer comandos ingresados

        // Salir del bucle si se ingresa "salir"
        if(strcmp(entrada, "salir\n")==0 ){ // Instrucción para romper el bucle
            break;
        }

        int i =0;
        // Tokenizar la entrada
        Tokens[i] = strtok(entrada, " \t\n");
        while(Tokens[i] != NULL){ // Bucle para leer espacios en blanco
            i++;
            Tokens[i] = strtok(NULL, " \t\n");
        }

        // Verificar si el último token es "&" para ejecución en segundo plano
        int ejecucion_en_segundo_plano = 0;
        if (i > 0 && strcmp(Tokens[i - 1], "&") == 0) {
            ejecucion_en_segundo_plano = 1;
            Tokens[i - 1] = NULL; // Eliminar el token "&"
        }

        if (Tokens[0] != NULL) { // Bloque que pasa los tokens al intérprete de comandos
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

