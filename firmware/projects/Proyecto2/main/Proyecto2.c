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
 * |    HC-SR04     |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	ECHO	 	| 	GPIO_3		|
 * | 	TRIGGER	 	| 	GPIO_2		|
 * | 	+5V  	 	| 	+5V	    	|
 * | 	GND 	 	| 	GND	    	|
 *
 * |    display LCD |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	D1	     	| 	GPIO_20		|
 * | 	D2	     	| 	GPIO_21  	|
 * | 	D3 	     	| 	GPIO_22  	|
 * | 	D4	     	| 	GPIO_23  	|
 * | 	SEL_1	    | 	GPIO_19  	|
 * | 	SEL_2	    | 	GPIO_18  	|
 * | 	SEL_3     	|  	GPIO_9  	|
 * | 	+5V	     	| 	+5V    		|
 * | 	GND	     	| 	GND    		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 11/04/2025 | Document creation		                         |
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
#include "gpio_mcu.h"
#include "led.h"
#include "switch.h"
#include "gpio_mcu.h"
#include "hc_sr04.h"
#include "lcditse0803.h"

/*==================[macros and definitions]=================================*/

/** @def PERDIOD_TECLAS
 *  @brief Tiempo de espera (ms) para la lectura del estado de botones de la placa
 */
#define CONFIG_BLINK_PERIOD_TECLAS 200

/** @def PERDIOD_MEDICION
 *  @brief Tiempo de espera (ms) para la lectura, escritura de las mediciones en LCD y actualizacion de los LEDS
 */
#define CONFIG_BLINK_PERIOD_MEDICION 1000

/*==================[internal data definition]===============================*/

/** @def hold
 * @brief Variable global de tipo booleana que indica si se debe mantener la ultima medicion en LCD */
bool hold = false;

/** @def medir
 * @brief Variable global de tipo booleana que indica si se realizaran las mediciones */
bool medir = false;

/** @def valor_medicion
 *  @brief Variable global de tipo entero sin signo para registrar los velores medidos  */
uint16_t valor_medicion = 0;

/*==================[internal functions declaration]=========================*/
/** @fn controlarLeds (void)
 * @brief funcion encargada de controlar los leds*/

void controlarLeds(void)
{
	if (valor_medicion < 10)
	{
		LedOff(LED_1);
		LedOff(LED_2);
		LedOff(LED_3);
	}
	else
	{
		if (valor_medicion > 10 && valor_medicion < 20)
		{
			LedOn(LED_1);
			LedOff(LED_2);
			LedOff(LED_3);
		}
		else
		{
			if (valor_medicion > 20 && valor_medicion < 30)
			{
				LedOn(LED_1);
				LedOn(LED_2);
				LedOff(LED_3);
			}

			else
			{
				if (valor_medicion > 30)
				{
					LedOn(LED_1);
					LedOn(LED_2);
					LedOn(LED_3);
				}
			}
		}
	}
}

/** @fn medicion (void *parametro)
 * @brief tarea encargada de medir la distancia a partir del sensor*/

static void medicion(void *parametro)
{
	while (true)
	{
		if (medir == true)
		{
			valor_medicion = HcSr04ReadDistanceInCentimeters(); // lector del sensor
		}

		vTaskDelay(CONFIG_BLINK_PERIOD_MEDICION / portTICK_PERIOD_MS);
	}
}

/** @fn mostrar (void *parametro)
 * @brief tarea encargada de mostrar por pantalla la distancia medida por el sensor*/

static void mostrar(void *parametro)
{
	while (true)
	{
		if (medir)
		{
			controlarLeds();
			if (!hold)
			{
				LcdItsE0803Write(valor_medicion);
			}
		}
		else
		{
			LcdItsE0803Off();
			LedOff(LED_1);
			LedOff(LED_2);
			LedOff(LED_3);
		}
		vTaskDelay(CONFIG_BLINK_PERIOD_MEDICION / portTICK_PERIOD_MS);
	}
	
}




/** @fn teclas (void *parametro)
 * @brief tarea encargada de controlar las acciones por tecla */

static void teclas(void *parametro)
{
	uint8_t tecla;
	while (true)
	{
		tecla = SwitchesRead();

		switch (tecla)
		{
		case SWITCH_1:
			medir = !medir;

			break;

		case SWITCH_2:
			hold = !hold;

			break;
		}
		vTaskDelay(CONFIG_BLINK_PERIOD_MEDICION / portTICK_PERIOD_MS);
	}

}

/*==================[external functions definition]==========================*/
void app_main(void)
{

	/* Inicialización de LCD, Leds, Sensor de Ultrasonido (configuracion de pines) y Switchs */
	LcdItsE0803Init();
	LedsInit();
	HcSr04Init(GPIO_3, GPIO_2);
	SwitchesInit();

	/* Creacion de las tareas teclas mostrar y medicion */
	xTaskCreate(&teclas, "teclas", 2048, NULL, 5, NULL);
	xTaskCreate(&mostrar, "mostrar", 2048, NULL, 5, NULL);
	xTaskCreate(&medicion, "medir", 2048, NULL, 5, NULL);
}
/*==================[end of file]============================================*/