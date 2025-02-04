#include "pico/stdlib.h"
#include "pico/time.h"

// Definir os pinos dos LEDs e do botão
#define LED_BLUE 11
#define LED_RED 12
#define LED_GREEN 13
#define BUTTON_PIN 5  // Pino do botão

// Variáveis globais
int estado_led = 0;
bool sequencia_ativa = false;
absolute_time_t ultimo_press_timestamp;  // Marca o tempo do último pressionamento do botão
#define DEBOUNCE_DELAY 200  // Tempo de debounce em milissegundos

// Função de configuração dos GPIOs
void setup_gpio() {
    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);

    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);

    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);  // Ativa o resistor de pull-up
}

// Callback do temporizador
int64_t timer_callback(alarm_id_t id, void *user_data) {
    if (estado_led == 1) {
        gpio_put(LED_BLUE, 0);  // Desliga o LED azul
        gpio_put(LED_RED, 1);   // Acende o LED vermelho
        estado_led = 2;
        add_alarm_in_ms(3000, timer_callback, NULL, false);
    } else if (estado_led == 2) {
        gpio_put(LED_RED, 0);   // Desliga o LED vermelho
        gpio_put(LED_GREEN, 1); // Acende o LED verde
        estado_led = 3;
        add_alarm_in_ms(3000, timer_callback, NULL, false);
    } else if (estado_led == 3) {
        gpio_put(LED_GREEN, 0); // Desliga o LED verde
        estado_led = 0;
        sequencia_ativa = false; // Desativa a sequência
    }
    return 0;
}

// Função de debounce para o botão
bool button_pressed() {
    // Verifica se o botão foi pressionado e se o tempo de debounce já passou
    if (gpio_get(BUTTON_PIN) == 0 && absolute_time_diff_us(ultimo_press_timestamp, get_absolute_time()) > DEBOUNCE_DELAY * 1000) {
        ultimo_press_timestamp = get_absolute_time();  // Atualiza o tempo do último pressionamento
        return true;
    }
    return false;
}

// Função para acionar a sequência de LEDs ao pressionar o botão
void button_callback() {
    if (!sequencia_ativa && button_pressed()) {  // Verifica o estado do botão com debounce
        sequencia_ativa = true;
        estado_led = 1;  // Começa com o LED azul
        gpio_put(LED_BLUE, 1);  // Acende o LED azul
        gpio_put(LED_RED, 0);   // Garante que o LED vermelho esteja apagado
        gpio_put(LED_GREEN, 0); // Garante que o LED verde esteja apagado

        // Reinicia a sequência com o LED azul aceso
        add_alarm_in_ms(3000, timer_callback, NULL, false);  // Inicia o temporizador
    }
}

int main() {
    // Inicializa os GPIOs
    setup_gpio();
    
    // Inicializa o timestamp do botão
    ultimo_press_timestamp = get_absolute_time();

    while (1) {
        button_callback();  // Verifica o estado do botão e aciona a sequência dos LEDs
        tight_loop_contents();  // Mantém o microcontrolador no loop
    }
}
