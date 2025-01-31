#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

#define RED_LED 11     // GPIO para o LED vermelho (ou BitDogLab LED R)
#define YELLOW_LED 12  // GPIO para o LED amarelo (ou BitDogLab LED G)
#define GREEN_LED 13   // GPIO para o LED verde (ou BitDogLab LED B)

volatile int state = 1; // Estado do semáforo (começa no amarelo para alternar corretamente)

// Callback do temporizador
bool repeating_timer_callback(struct repeating_timer *t) {
    // Alternar entre os estados do semáforo
    switch (state) {
        case 0:
            gpio_put(RED_LED, 1);
            gpio_put(YELLOW_LED, 0);
            gpio_put(GREEN_LED, 0);
            state = 1;
            break;
        case 1:
            gpio_put(RED_LED, 0);
            gpio_put(YELLOW_LED, 1);
            gpio_put(GREEN_LED, 0);
            state = 2;
            break;
        case 2:
            gpio_put(RED_LED, 0);
            gpio_put(YELLOW_LED, 0);
            gpio_put(GREEN_LED, 1);
            state = 0;
            break;
    }
    return true; // Mantém o timer ativo
}

int main() {
    stdio_init_all(); // Inicializa a comunicação serial
    sleep_ms(2000); // Aguarda a inicialização da porta serial
    
    // Configura os pinos dos LEDs como saída
    gpio_init(RED_LED);
    gpio_set_dir(RED_LED, GPIO_OUT);
    gpio_init(YELLOW_LED);
    gpio_set_dir(YELLOW_LED, GPIO_OUT);
    gpio_init(GREEN_LED);
    gpio_set_dir(GREEN_LED, GPIO_OUT);
    
    // Inicializa o estado do semáforo (vermelho ligado primeiro)
    gpio_put(RED_LED, 1);
    gpio_put(YELLOW_LED, 0);
    gpio_put(GREEN_LED, 0);
    
    struct repeating_timer timer;
    add_repeating_timer_ms(-3000, repeating_timer_callback, NULL, &timer); // Chama a função a cada 3s
    repeating_timer_callback(NULL); // Chama imediatamente para garantir que o tempo inicial seja 3s
    
    while (1) {
        printf("Semáforo operando...\n");
        sleep_ms(1000); // Mensagem a cada 1s
    }
}
