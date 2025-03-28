/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 28/03/2025 | Document creation		                         |
 *
 * @author Leonel Borgeat (leonelborgeat@gmail.com)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "switch.h"

/*==================[macros and definitions]=================================*/
#define ON 1
#define OFF 0
#define TOGGLE 2
#define CONFIG_BLINK_PERIOD 100  // Definimos 100 ms de periodo

/*==================[struct declaration]=========================*/
	// Estructura definida por la catedra

	struct leds
{
    uint8_t mode;       //ON, OFF, TOGGLE
	uint8_t n_led;      //Indica el número de led a controlar
	uint8_t n_ciclos;   //Indica la cantidad de ciclos de ncendido/apagado
	uint16_t periodo;   //Indica el tiempo de cada ciclo
} 

my_leds; 

/*==================[internal functions declaration]=========================*/
// Funcion que controla los led recibiendo un puntero de struct led

void controlarLED(struct leds *led) {
	switch (led->mode) {
		case ON:
			// Encender el LED correspondiente
			if (led->n_led == 1) {
				LedOn(LED_1);
			} else if (led->n_led == 2) {
				LedOn(LED_2);
			} else if (led->n_led == 3) {
				LedOn(LED_3);
			} else {
				printf("Número de LED no válido: %d\n", led->n_led);
			}
			break;

		case OFF:
			// Apagar el LED correspondiente
			if (led->n_led == 1) {
				LedOff(LED_1);
			} else if (led->n_led == 2) {  // Corregido: antes verificaba `n_led == 1`
				LedOff(LED_2);
			} else if (led->n_led == 3) {  // Corregido: antes verificaba `n_led == 1`
				LedOff(LED_3);
			} else {
				printf("Número de LED no válido: %d\n", led->n_led);
			}
			break;

		case TOGGLE:
			// Alternar el estado del LED según la cantidad de ciclos especificada
			for (int i = 0; i < led->n_ciclos; i++) {
				switch (led->n_led) {
					case 1:
						LedToggle(LED_1);
						break;
					case 2:
						LedToggle(LED_2);
						break;
					case 3:
						LedToggle(LED_3);
						break;
					default:
						printf("Número de LED no válido: %d\n", led->n_led);
						break;  
				}
				vTaskDelay(led->periodo / portTICK_PERIOD_MS); // Espera entre toggles
			}
			break;

		default:
			printf("Modo de LED no válido: %d\n", led->mode);
			break;
	}
}
/*==================[external functions definition]==========================*/
void app_main(void){

	LedsInit();  // Inicializa los LEDs antes de usarlos
 
	// Parametros para el encendido de los leds

	my_leds.n_ciclos = 100;  // Cantidad de veces que se alternará el LED en TOGGLE
	my_leds.periodo = 500;  // Período en milisegundos entre cada toggle
	my_leds.n_led = 3;      // LED que será controlado
	my_leds.mode = TOGGLE;  // Modo de operación On, Off, Toggle

	controlarLED(&my_leds);  // Llamada a la función para ejecutar la acción
	
}
/*==================[end of file]============================================*/