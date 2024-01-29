#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESOS 1000

struct Proceso {
    int id, llegada, prioridad, tiempo_procesador, memoria, impresoras, escaneres, modems, cds;
};

void leerListaPlanificacion(const char *nombreArchivo, struct Proceso listaProcesos[MAX_PROCESOS], int *numProcesos) {
    FILE *archivo = fopen(nombreArchivo, "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    *numProcesos = 0;
    while (fscanf(archivo, "%d,%d,%d,%d,%d,%d,%d,%d,%d", &listaProcesos[*numProcesos].id,
                                                      &listaProcesos[*numProcesos].llegada,
                                                      &listaProcesos[*numProcesos].prioridad,
                                                      &listaProcesos[*numProcesos].tiempo_procesador,
                                                      &listaProcesos[*numProcesos].memoria,
                                                      &listaProcesos[*numProcesos].impresoras,
                                                      &listaProcesos[*numProcesos].escaneres,
                                                      &listaProcesos[*numProcesos].modems,
                                                      &listaProcesos[*numProcesos].cds) == 9) {
        (*numProcesos)++;
    }

    fclose(archivo);
}

void ordenarPorPrioridad(struct Proceso listaProcesos[MAX_PROCESOS], int numProcesos) {
    // Algoritmo simple de burbujeo para ordenar los procesos por prioridad
    for (int i = 0; i < numProcesos - 1; i++) {
        for (int j = 0; j < numProcesos - i - 1; j++) {
            if (listaProcesos[j].prioridad > listaProcesos[j + 1].prioridad) {
                // Intercambiar los procesos
                struct Proceso temp = listaProcesos[j];
                listaProcesos[j] = listaProcesos[j + 1];
                listaProcesos[j + 1] = temp;
            }
        }
    }
}

void ejecutarProceso(struct Proceso *proceso) {
    printf("Proceso [%d] recibido.\n", proceso->id);
    printf("Ejecutando proceso [%d].\n", proceso->id);

    for (int tiempo = 0; tiempo < proceso->tiempo_procesador; tiempo++) {
        printf("Tiempo restante para el proceso [%d]: %d unidades de tiempo.\n", proceso->id, proceso->tiempo_procesador - tiempo);
        // Aquí puedes agregar tu lógica específica para procesar cada unidad de tiempo
        // Por ejemplo, puedes simular la ejecución del proceso o realizar acciones específicas
    }

    printf("Proceso [%d] concluido.\n", proceso->id);
}

void planificar(struct Proceso listaProcesos[MAX_PROCESOS], int numProcesos) {
    printf("Procesos leídos:\n");

    ordenarPorPrioridad(listaProcesos, numProcesos);

    for (int i = 0; i < numProcesos; i++) {
        ejecutarProceso(&listaProcesos[i]);

        // Si encuentras el carácter '*', detén la ejecución de procesos
        if (listaProcesos[i].llegada == '*') {
            printf("Se encontró '*', deteniendo la ejecución de procesos.\n");
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <nombre_archivo>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *nombreArchivo = argv[1];

    struct Proceso listaProcesos[MAX_PROCESOS];
    int numProcesos;

    leerListaPlanificacion(nombreArchivo, listaProcesos, &numProcesos);

    if (numProcesos > 0) {
        planificar(listaProcesos, numProcesos);
    }

    return 0;
}
