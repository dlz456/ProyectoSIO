#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define Limite_Entrada  1024 // Cantidad límite de caracteres de la entrada
#define Limite_Tokens  64 // Cantidad límite de tokens aceptados

void ejecutar_hijo(char **tokens) {
    int i, redireccion = -1;

    for (i = 0; tokens[i] != NULL; i++) {
        // Directiva para aceptar el caracter > en comandos
        if (strcmp(tokens[i], ">") == 0) {
            redireccion = i;
            break;
        }
    }

    if (redireccion >= 0 && tokens[i + 1] != NULL) {
        // Bloque para leer el comando o archivo a ejecutar
        FILE *archivo_seleccionado = fopen(tokens[i + 1], "w");

        if (archivo_seleccionado == NULL) {
            perror("No se puede abrir el archivo seleccionado");
            exit(EXIT_FAILURE);
        }

        dup2(fileno(archivo_seleccionado), STDOUT_FILENO);
        fclose(archivo_seleccionado);
        tokens[i] = NULL; // Eliminar el símbolo de redirección
    }

    if (execvp(tokens[0], tokens) == -1) {
        perror("Ese comando no es válido");
        exit(EXIT_FAILURE);
    }
}

int main() {
    char entrada[Limite_Entrada]; // Variable que guarda la entrada
    char *Tokens[Limite_Tokens]; // Variable que guarda los tokens

    while (1) {
        printf("Consola Creada> "); // Imprimir mensaje de la consola

        fgets(entrada, sizeof(entrada), stdin); // Leer comandos ingresados

        if (strcmp(entrada, "salir\n") == 0) { // Instrucción para romper el bucle
            break;
        }

        int i = 0;
        Tokens[i] = strtok(entrada, " \t\n");
        while (Tokens[i] != NULL) { // Bucle para leer espacios en blanco
            i++;
            Tokens[i] = strtok(NULL, " \t\n");
        }

        if (Tokens[0] != NULL) { // Bloque que pasa los tokens al interprete de comandos
            if (strcmp(Tokens[0], "salir") == 0) {
                break;
            } else {
                leer_comandos(Tokens);
            }
        }
    }

    return 0;
}

