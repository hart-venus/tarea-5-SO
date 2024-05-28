#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>

#define NUM_CHILDREN 10

pid_t children[NUM_CHILDREN]; // arreglo para contener los pids de los hijos

void handle_signal(int sig) {
    if (sig == SIGTERM) {
        printf("bye %d\n", getpid()); // handler para la señal de terminación
        exit(0);
    }
}

void child_process() {
    signal(SIGTERM, handle_signal); // handler de señales
    while (1) {
        // hasta que se reciba la señal de terminación, imprimir el pid cada segundo
        printf("PID: %d\n", getpid());
        sleep(1);
    }
}

int main() {
    srand(time(NULL)); // semilla para números aleatorios

    for (int i = 0; i < NUM_CHILDREN; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            child_process(); // crea 10 procesos hijos
        } else {
            children[i] = pid; // el padre continuamente crea hijos y los pone en el arreglo
        }
    }

    sleep(0.2); // dejar que los hijos impriman su pid
    for (int i = 0; i < NUM_CHILDREN; i++) {
        int index = rand() % (NUM_CHILDREN - i); // cortar el rango para que no se repitan
        kill(children[index], SIGTERM);
        waitpid(children[index], NULL, 0); // esperar a que el hijo termine
        // mover el último hijo al lugar del hijo que terminó para no dejar huecos
        children[index] = children[NUM_CHILDREN - i - 1];
        sleep(1);
    }

    // esperar la señal de terminación de todos los hijos
    for (int i = 0; i < NUM_CHILDREN; i++) {
        wait(NULL);
    }

    return 0;
}
