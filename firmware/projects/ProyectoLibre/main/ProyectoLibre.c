/*! @mainpage Proyecto Libre
 *
 * @section genDesc General Description
 *
 * Es un programa el cual devuelve por bluetooth al celular, el tiempo que tarda el usuario en apagar el led por medio
 * de un pulsador.
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
 * | 23/05/2025 | Document creation		                         |
 *
 * @author Leonel Borgeat (leonel.borgeat@ingenieria.uner.edu.ar)
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
#include "lcditse0803.h"
#include "timer_mcu.h"
/*==================[macros and definitions]=================================*/
bool encendido = false;
uint8_t tiempoRespuesta;

/*==================[internal data definition]===============================*/

/** @fn encenderLed (void *parametro)
 * @brief tarea encargada de encender los leds de manera random */
static void encenderLed(void *vparametro)
{
	while (true)
	{
		if (encendido == false)
		{
			// encender 1 led  y cambiar encendido por true
			//medir ();
			//encendido=false;
		}
	}
}

/** @fn medir (void *parametro)
 * @brief tarea encargada de medir el timepo desde que se enciende el led hasta que es apagado por
 * el pulsador */
static void medir (void *vparametro)
{
	//contador encendido apagado segundos
	//cambiar encendido por false
	//salidaBluetooth();
}

/** @fn salidaBluetooth (void *parametro)
 * @brief tarea encargada de devolver por bluetooth el parametro medido */
static void salidaBluetooth (void *vparametro)
{
	// salir por bluetooth el valor tiempo en respuesta
}

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/
void app_main(void)
{
	printf("Hello world!\n");
}
/*==================[end of file]============================================*/