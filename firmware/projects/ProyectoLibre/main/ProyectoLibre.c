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
 * | 	LED1	 	| 	GPIO_10		|
 * | 	LED2	 	| 	GPIO_11		|
 * | 	LED3	 	| 	GPIO_5		|
 * | 	Pulsador1	| 	GPIO_1		|
 * | 	Pulsador2	| 	GPIO_2		|
 * | 	Pulsador3	| 	GPIO_3		|
 *
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
#include "switch.h"
#include "gpio_mcu.h"
#include "timer_mcu.h"
#include "ble_mcu.h"

/*==================[macros and definitions]=================================*/

#define CONFIG_MEDICION_ms 20
#define CONFIG_TECLAS_us 1000
#define LENGTH_SECUENCIA 15

/*==================[internal data definition]===============================*/

uint16_t tiempoRespuesta;
uint16_t tiempoRespuestaTotal = 0;

bool encendido = false;

bool inicio = false;

uint8_t ledActual;

uint8_t iterador;

uint8_t secuencia[LENGTH_SECUENCIA] = {2, 1, 2, 1, 1, 2, 2, 1, 2, 0, 1, 0, 0, 0, 2};

typedef struct
{
	gpio_t led;
	gpio_t tecla;
} leds;

leds misLeds[3] = {
	{GPIO_11, GPIO_1},
	{GPIO_10, GPIO_2},
	{GPIO_5, GPIO_3}};

TaskHandle_t medir_task_handle = NULL;
TaskHandle_t inicio_task_handle = NULL;
TaskHandle_t salidaBluetooth_task_handle = NULL;

/*==================[internal functions declaration]=========================*/

void FuncTimerA(void *parametro)
{
	vTaskNotifyGiveFromISR(inicio_task_handle, pdFALSE); /* Envía una notificación a la tarea asociada a medir */
	vTaskNotifyGiveFromISR(medir_task_handle, pdFALSE);	 /* Envía una notificación a la tarea asociada a medir */
}

/** @fn salidaBluetooth (void *parametro)
 * @brief tarea encargada de devolver por bluetooth el parametro medido */
void salidaBluetooth()
{
	char mensaje[200];

	sprintf(mensaje, "El tiempo de respuesta promedio es: %d\r\n", tiempoRespuestaTotal);

	if (BleStatus() == BLE_CONNECTED)
		BleSendString(mensaje);
}

/** @fn controlLed (void *parametro)
 * @brief tarea encargada de encender los leds de manera random */
static void controlLed(void *vparametro)
{
	while (true)
	{
		if (inicio)
		{
			if (encendido == false)
			{
				if (iterador == LENGTH_SECUENCIA)
				{
					GPIOOff(misLeds[ledActual].led);
					inicio = false;
					tiempoRespuestaTotal = tiempoRespuestaTotal/LENGTH_SECUENCIA;
					printf("%d\r\n", tiempoRespuestaTotal);
					salidaBluetooth();
				}
				else
				{
					if (iterador > 0)
						GPIOOff(misLeds[ledActual].led);

					ledActual = secuencia[iterador];
					GPIOOn(misLeds[ledActual].led);
					encendido = true;
					iterador++;
				}
			}
		}
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	}
}

/** @fn medir (void *parametro)
 * @brief tarea encargada de medir el timepo desde que se enciende el led hasta que es apagado por
 * el pulsador */
static void medir(void *vparametro)
{
	uint16_t contador = 0;
	while (true)
	{
		if (inicio)
		{
			if (encendido)
			{
				if (!GPIORead(misLeds[ledActual].tecla))
				{
					tiempoRespuesta = (contador * CONFIG_TECLAS_us) / 1000;
					tiempoRespuestaTotal = tiempoRespuestaTotal + tiempoRespuesta;
					encendido = false;
					contador = 0;
				}
				else
				{
					contador++;
				}
			}
		}
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	}
}

/** @fn tecla_1 (void *parametro)
 * @brief funcion que se encarga de controlar la tecla 1 */
void tecla_1()
{
	inicio = true;
	iterador = 0;
}

void read_data(uint8_t *data, uint8_t length)
{
}

/*==================[external functions definition]==========================*/
void app_main(void)
{
	iterador = 0;

	SwitchesInit();
	SwitchActivInt(SWITCH_1, tecla_1, NULL);

	ble_config_t ble_configuration = {
		"ESP_EDU_1",
		read_data};

	timer_config_t timer_inicio = {
		.timer = TIMER_A,
		.period = CONFIG_TECLAS_us,
		.func_p = FuncTimerA,
		.param_p = NULL};
	TimerInit(&timer_inicio);

	for (uint8_t i=0; i<3; i++)
	{
		GPIOInit(misLeds[i].led, GPIO_OUTPUT);
		GPIOInit(misLeds[i].tecla, GPIO_INPUT);
	}
	xTaskCreate(&controlLed, "ControlaLeds", 2048, NULL, 5, &inicio_task_handle);
	xTaskCreate(&medir, "medirTiempo", 2048, NULL, 5, &medir_task_handle);
	// xTaskCreate(&salidaBluetooth, "muestraTiempo", 2048, NULL, 5, &salidaBluetooth_task_handle);

	TimerStart(timer_inicio.timer);

	BleInit(&ble_configuration);
}
/*==================[end of file]============================================*/