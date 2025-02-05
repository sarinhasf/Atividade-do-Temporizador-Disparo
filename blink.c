// Projeto Embarcatech
// Aluna: Sara Souza Ferreira
// Data de envio: 04/02/2025


#include <stdio.h>
#include "pico/stdlib.h"

#define LED_PIN_GREEN 13 
#define LED_PIN_BLUE 12
#define LED_PIN_RED 11
#define BUTTON_PIN 5
#define DEBOUNCE_DELAY 50  // 50ms de debounce

bool led_active = false;  // Indica se os LEDs estão em uma sequência ativa

// Callback para desligar o LED verde (última etapa)
int64_t turn_off_green(alarm_id_t id, void *user_data) {
    gpio_put(LED_PIN_GREEN, 0);
    led_active = false; // Agora o botão pode ativar novamente
    return 0;
}

// Callback para desligar o LED azul (segunda etapa)
int64_t turn_off_blue(alarm_id_t id, void *user_data) {
    gpio_put(LED_PIN_BLUE, 0);
    // Agenda o desligamento do LED verde após 3 segundos
    add_alarm_in_ms(3000, turn_off_green, NULL, false);
    return 0;
}

// Callback para desligar o LED vermelho (primeira etapa)
int64_t turn_off_red(alarm_id_t id, void *user_data) {
    gpio_put(LED_PIN_RED, 0);
    // Agenda o desligamento do LED azul após 3 segundos
    add_alarm_in_ms(3000, turn_off_blue, NULL, false);
    return 0;
}

// Inicia a sequência de LEDs
void start_led_sequence() {

    if (!led_active) {  // Apenas inicia se não estiver em execução
    
        led_active = true;

        // Liga todos os LEDs
        gpio_put(LED_PIN_RED, 1);
        gpio_put(LED_PIN_BLUE, 1);
        gpio_put(LED_PIN_GREEN, 1);

        // Agenda o desligamento do primeiro LED após 3 segundos
        add_alarm_in_ms(3000, turn_off_red, NULL, false);
    }
}

int main() {
    stdio_init_all();
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);  

    gpio_init(LED_PIN_RED);
    gpio_set_dir(LED_PIN_RED, GPIO_OUT);
    
    gpio_init(LED_PIN_BLUE);
    gpio_set_dir(LED_PIN_BLUE, GPIO_OUT);
    
    gpio_init(LED_PIN_GREEN);
    gpio_set_dir(LED_PIN_GREEN, GPIO_OUT);

    while (true) {
        // Verifica se o botão foi pressionado e os LEDs não estão ativos
        if (gpio_get(BUTTON_PIN) == 0 && !led_active) {
            sleep_ms(DEBOUNCE_DELAY);  // Debounce

            // Verifica se ainda está pressionado após o debounce
            if (gpio_get(BUTTON_PIN) == 0) {
                start_led_sequence();  // Inicia a sequência de LEDs
            }
        }
        sleep_ms(10);  // Pequeno atraso para eficiência
    }
}
