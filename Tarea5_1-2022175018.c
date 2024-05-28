#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

pid_t pid1, pid2; // pids de los procesos par e impar

void handle_signal(int sig) {
    // handler vacío para atrapar señales
}

void child1() {
    signal(SIGUSR1, handle_signal); // handler de señales
    for (int i = 0; i <= 10; i += 2) { // para cada número par
        pause(); // esperar señal
        printf("Par: %d\n", i);
        kill(getppid(), SIGUSR2); // señal al proceso padre para continaur
    }
    exit(0); // salir después de terminar
}

void child2() {
    signal(SIGUSR2, handle_signal); // handler de señales
    for (int i = 1; i <= 10; i += 2) { // para cada número impar
        pause(); // esperar señal
        printf("Impar: %d\n", i); // imprimir número impar
        kill(getppid(), SIGUSR1); // señal al proceso padre para continuar
    }
    exit(0); // salir después de terminar
}

int main() {
    signal(SIGUSR1, handle_signal); // handler de señales
    signal(SIGUSR2, handle_signal); // handler de señales

    pid1 = fork();
    if (pid1 == 0) {
        child1();
    }

    pid2 = fork();
    if (pid2 == 0) {
        child2();
    }

    // enviar señal al primer hijo para comenzar
    kill(pid1, SIGUSR1);

    // el padre se encarga de enviar señales a los hijos
    for (int i = 0; i < 10; i++) {
        pause(); // esperar señal del hijo que termina
        if (i % 2 == 0) {
            kill(pid2, SIGUSR2); // si es par, señal al hijo 2
        } else {
            kill(pid1, SIGUSR1); // si no, señal al hijo 1
        }
    }

    // esperar a que ambos procesos terminen
    wait(NULL);
    wait(NULL);

    return 0;
}
