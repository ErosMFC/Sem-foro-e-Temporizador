#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

// Definir os pinos dos LEDs
const uint LED_VERMELHO = 11;
const uint LED_AMARELO  = 12;  // Canal que anteriormente acendia o LED azul (não utilizado na nova lógica)
const uint LED_VERDE    = 13;

// Variável para armazenar o estado atual do semáforo
// 0 = Vermelho, 1 = Amarelo, 2 = Verde
int estado_semaforo = 0;

// Função callback do temporizador com a assinatura correta:
// Retorna bool e recebe um ponteiro para struct repeating_timer.
bool repeating_timer_callback(struct repeating_timer *t) {
    // Apagar todos os LEDs
    gpio_put(LED_VERMELHO, 0);
    gpio_put(LED_AMARELO, 0);
    gpio_put(LED_VERDE, 0);

    // Acender o LED correspondente ao estado atual:
    if (estado_semaforo == 0) {
        // Estado 0: Vermelho
        gpio_put(LED_VERMELHO, 1);
    } else if (estado_semaforo == 1) {
        // Estado 1: Amarelo (formado pela combinação de vermelho e verde)
        gpio_put(LED_VERMELHO, 1);
        gpio_put(LED_VERDE, 1);
    } else if (estado_semaforo == 2) {
        // Estado 2: Verde
        gpio_put(LED_VERDE, 1);
    }

    // Atualizar para o próximo estado
    estado_semaforo = (estado_semaforo + 1) % 3;

    // Retornar true para manter o timer repetindo
    return true;
}

int main() {
    // Inicializar o stdio (necessário para o printf)
    stdio_init_all();

    // Inicializar os pinos como saída
     gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_init(LED_AMARELO);
    gpio_set_dir(LED_AMARELO, GPIO_OUT);
    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);

    // Configurar o temporizador para chamar a função a cada 3000ms (3 segundos)
    struct repeating_timer timer;
    add_repeating_timer_ms(3000, repeating_timer_callback, NULL, &timer);

    // Laço principal para enviar informações a cada 1 segundo
    while (true) {
        printf("Estado atual do semáforo: %d\n", estado_semaforo); // 0: Vermelho, 1: Amarelo, 2: Verde
        sleep_ms(1000); // Esperar 1 segundo
    }

    return 0;
}
