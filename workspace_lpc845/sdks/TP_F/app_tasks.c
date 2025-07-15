
#include "app_tasks.h"

// Cola para datos del ADC
xQueueHandle queue_adc;
// Cola para datos del display
xQueueHandle queue_display;
// Cola para selecion de valor para el display
xQueueHandle queue_display_variable;
// Cola para datos de luminosidad
xQueueHandle queue_lux;
// cola para datos de lux, pero en crudo
xQueueHandle queue_lux_data;
// Cola para setpoint
xQueueHandle queue_setpoint;
// Semáforo para interrupción del touch
xSemaphoreHandle semphr_s1;
// Semáforo para interrupción del touch
xSemaphoreHandle semphr_s2;

// Semáforo para interrupción del infrarojo
xSemaphoreHandle semphr_buzz;
// Semáforo para interrupción del user button
xSemaphoreHandle semphr_usr;

// es un Handler para la tarea de display 
TaskHandle_t handle_display;

/**
 * Iniciamos los periféricos
 */
void task_init(void *params) {
	// Inicio semáforos
	semphr_buzz = xSemaphoreCreateBinary();
	semphr_usr = xSemaphoreCreateBinary();
	semphr_touch = xSemaphoreCreateBinary();
	semphr_counter = xSemaphoreCreateCounting(99, 30);
	semphr_mutex = xSemaphoreCreateMutex();

	// Inicio colas
	queue_adc = xQueueCreate(1, sizeof(adc_data_t));
	queue_display_variable = xQueueCreate(1, sizeof(display_variable_t));
	queue_lux = xQueueCreate(1, sizeof(uint16_t));
	queue_display = xQueueCreate(1, sizeof(uint16_t));
	
	// Inicializacion de GPIO
	wrapper_gpio_init(0);
	wrapper_gpio_init(1);
	// Inicialización del LED
	wrapper_output_init((gpio_t){LED}, true);
	// Inicialización del buzzer
	wrapper_output_init((gpio_t){BUZZER}, false);
	// Inicialización del enable del CNY70
	wrapper_output_init((gpio_t){CNY70_EN}, true);
	// Configuro el ADC
	wrapper_adc_init();
	// Configuro el display
	wrapper_display_init();
	// Configuro botones
	wrapper_btn_init();
	// Configuro interrupción por flancos para el infrarojo y para el botón del user
	wrapper_gpio_enable_irq((gpio_t){CNY70}, kPINT_PinIntEnableBothEdges, cny70_callback);
	wrapper_gpio_enable_irq((gpio_t){USR_BTN}, kPINT_PinIntEnableFallEdge, usr_callback);
	// Inicializo el PWM
	wrapper_pwm_init();
	// Inicializo I2C y Bh1750
	wrapper_i2c_init();
	wrapper_bh1750_init();
	// Inicializo el pulsador capacitivo
	wrapper_touch_init();

	// Elimino tarea para liberar recursos
	vTaskDelete(NULL);
}

/**
 * Tarea del ADC (1)
 */
void task_adc(void *params) {

	while(1) {
		// Inicio una conversion
		ADC_DoSoftwareTriggerConvSeqA(ADC0);
		// Bloqueo la tarea por 250 ms
		vTaskDelay(pdMS_TO_TICKS(250));
	}
}

/**
 * Tarea Control del display (2)
 */
void task_display_change(void *params) {
	// Dato para pasar
	display_variable_t variable = kDISPLAY_TEMP;

	while(1) {
		// Escribe el dato en la cola
		xQueueOverwrite(queue_display_variable, &variable);
		// Intenta tomar el semáforo
		xSemaphoreTake(semphr_usr, portMAX_DELAY);
		// Si se presionó, cambio la variable
		variable = (variable == kDISPLAY_TEMP)? kDISPLAY_REF : kDISPLAY_TEMP;
	}
}

/**
 *  Tarea Escribir en el display (3)
 */
void task_control(void *params) {
	// Variable a mostrar
	display_variable_t variable = kDISPLAY_TEMP;
	// Valores de ADC
	adc_data_t data = {0};
	// Valor a mostrar
	uint16_t val = 0;

	while(1) {
		// Veo que variable hay que mostrar
		xQueuePeek(queue_display_variable, &variable, portMAX_DELAY);
		// Leo los datos del ADC
		xQueuePeek(queue_adc, &data, portMAX_DELAY);
		// Veo cual tengo que mostrar
		val = (variable == kDISPLAY_TEMP)? data.temp_raw : data.ref_raw;
		val = 30 * val / 4095;
		// Escribo en la cola del display si puedo tomar el mutex
		xSemaphoreTake(semphr_mutex, portMAX_DELAY);
		xQueueOverwrite(queue_display, &val);
		xSemaphoreGive(semphr_mutex);

		vTaskDelay(pdMS_TO_TICKS(50));
	}
}

/**
 *  Tarea Número en el display (4)
 */
void task_display(void *params) {
    uint8_t data;
    display_variable_t variable;

    while(1) {
        if(!xQueuePeek(queue_display, &data, pdMS_TO_TICKS(100))) { continue; }
        xQueuePeek(queue_display_variable, &variable, 0);

        wrapper_display_off();
        wrapper_display_write((uint8_t)(data / 10));
        wrapper_display_on((gpio_t){COM_1});
        vTaskDelay(pdMS_TO_TICKS(10));
        wrapper_display_off();
        wrapper_display_write((uint8_t)(data % 10));
        wrapper_display_on((gpio_t){COM_2});

        // Enciende punto decimal si muestra setpoint
        if (variable == kDISPLAY_REF) {
            wrapper_display_decimal(true);
        } else {
            wrapper_display_decimal(false);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/**
 *  Tarea para BH1750 (5)
 */
void task_BH1750(void *params) {
    //Parámetros de luminosidad
    uint16_t lux = 0 ;
    float lux_p = 0;

    while (1){
        // Bloqueo
        vTaskDelay(pdMS_TO_TICKS(200));

        // Leo el valor de lux
        lux = wrapper_bh1750_read();
        if (lux > 30000)
            lux = 30000;

        lux_p = (lux / 30000.0f) * 100.0f;

        // Muestrar en la consola
        xQueueOverwrite(queue_lux, &lux_p);
        xQueueOverwrite(queue_lux_raw, &lux);
    }
}


/**
 *  Tarea LED Azul y RV22 (6)
 */
void task_led_azul_rv22(void *params) {
    uint16_t rv22_val = 0;
    uint8_t intensidad = 0;

    while(1) {
        rv22_val = wrapper_adc_read(RV22_CHANNEL); // Lee el canal del RV22
        intensidad = (uint8_t)((rv22_val * 100) / 4095); // Escala a %
        wrapper_pwm_set((gpio_t){LED_AZUL}, intensidad);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}


/
/**
 *  Tarea para Buzzer (7)
 */
void task_buzzer(void *params) {

	while(1) {
		// Intenta tomar el semáforo
		xSemaphoreTake(semphr_buzz, portMAX_DELAY);
		// Conmuto el buzzer
		wrapper_output_toggle((gpio_t){BUZZER});
	}
}

/**
 *  Tarea Setpoint (8)
 */

void task_setpoint(void *params) {
    uint8_t setpoint = 25; // Valor inicial en %
    while (1) {
        // Espera por S1 (incrementa)
        if (xSemaphoreTake(semphr_s1, pdMS_TO_TICKS(10)) == pdTRUE) {
            if (setpoint < 75) setpoint++;
        }
        // Espera por S2 (decrementa)
        if (xSemaphoreTake(semphr_s2, pdMS_TO_TICKS(10)) == pdTRUE) {
            if (setpoint > 25) setpoint--;
        }
        // Envía el setpoint por la cola
        xQueueOverwrite(queue_setpoint, &setpoint);
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

/**
 *  Tarea LED Tricolor (9)
 */
void task_tricolor(void *params) {
    uint8_t lux = 0;
    uint8_t setpoint = 0;
    uint8_t intensidad = 0;

    while (1) {
        // Obtiene luminosidad y setpoint
        xQueuePeek(queue_lux, &lux, portMAX_DELAY);
        xQueuePeek(queue_setpoint, &setpoint, portMAX_DELAY);

        if (lux > setpoint) {
            intensidad = (lux - setpoint); // Diferencia proporcional
            wrapper_pwm_set((gpio_t){LED_ROJO}, intensidad); // Ajusta LED rojo
            wrapper_pwm_set((gpio_t){LED_AZUL}, 0);          // Apaga azul
        } else {
            intensidad = (setpoint - lux); // Diferencia proporcional
            wrapper_pwm_set((gpio_t){LED_AZUL}, intensidad); // Ajusta LED azul
            wrapper_pwm_set((gpio_t){LED_ROJO}, 0);          // Apaga rojo
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

/**
 *  Tarea Monitoreo (10)
 */

 void task_logger(void *params) {
    uint32_t tiempo = 0;
    uint8_t lux = 0, setpoint = 0, led_rojo = 0, led_azul = 0;

    while(1) {
        tiempo += 1000;
        xQueuePeek(queue_lux, &lux, 0);
        xQueuePeek(queue_setpoint, &setpoint, 0);
        led_rojo = wrapper_pwm_get((gpio_t){LED_ROJO});
        led_azul = wrapper_pwm_get((gpio_t){LED_AZUL});

        printf("Tiempo: %lu ms | Lux: %u%% | Setpoint: %u%% | LED Rojo: %u%% | LED Azul: %u%%\n",
            tiempo, lux, setpoint, led_rojo, led_azul);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}