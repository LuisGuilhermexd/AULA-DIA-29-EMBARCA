#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

// Definição dos pinos dos LEDs e botão
#define LED_BLUE_PIN 11    // led azul
#define LED_RED_PIN 12     // led Vermelho
#define LED_GREEN_PIN 13   // led Verde
#define BUTTON_PIN 5       // Botão A

// Estados dos LEDs
typedef enum {
    STATE_ALL_ON,   // Todos os LEDs ligados
    STATE_BLUE_OFF, // Azul desligado
    STATE_RED_OFF,  // Vermelho desligado
    STATE_GREEN_OFF // Verde desligado
} led_state_t;


volatile led_state_t current_state = STATE_ALL_ON;
volatile bool button_pressed = false;
volatile bool timer_running = false;

// Função para ligar/desligar os LEDs
void set_leds(bool blue, bool red, bool green) {
    gpio_put(LED_BLUE_PIN, blue);
    gpio_put(LED_RED_PIN, red);
    gpio_put(LED_GREEN_PIN, green);
}

// Callback do temporizador
int64_t timer_callback(alarm_id_t id, void *user_data) {
    switch (current_state) {
        case STATE_ALL_ON:
            current_state = STATE_BLUE_OFF;
            set_leds(0, 1, 1); // Azul desligado, Vermelho e Verde ligados
            break;
        case STATE_BLUE_OFF:
            current_state = STATE_RED_OFF;
            set_leds(0, 0, 1); // Azul e Vermelho desligados, Verde ligado
            break;
        case STATE_RED_OFF:
            current_state = STATE_GREEN_OFF;
            set_leds(0, 0, 0); // Todos os LEDs desligados
            timer_running = false; // Finaliza a temporização
            return 0; // Não repete o alarme
        default:
            break;
    }
    return 3000000; // Repete o alarme após 3 segundos (3.000.000 µs)
}

// Callback do botão
void button_callback(uint gpio, uint32_t events) {
    if (gpio == BUTTON_PIN && !timer_running) {
        button_pressed = true;
    }
}

int main() {
    // Inicializa as entradas e saídas
    stdio_init_all();
    gpio_init(LED_BLUE_PIN);
    gpio_init(LED_RED_PIN);
    gpio_init(LED_GREEN_PIN);
    gpio_init(BUTTON_PIN);

    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    // Configura a interrupção do botão
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &button_callback);

    while (true) {
        if (button_pressed && !timer_running) {
            button_pressed = false;
            timer_running = true;

            // Inicia a sequência de temporização
            current_state = STATE_ALL_ON;
            set_leds(1, 1, 1); // Todos os LEDs ligados
            add_alarm_in_ms(3000, timer_callback, NULL, false); // Primeiro atraso de 3 segundos
        }
        sleep_ms(100); // Evita uso excessivo da CPU
    }

    return 0;
}