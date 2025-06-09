/*! @mainpage Examen
 *
 * @section Examen primer cuatrimestre electronica programable 9/06/2025
 *
 * El programa consta del control de una estacion de monitoreo para el eternauta en el cual
 * medimos la temperatura la humedad y la radiacion del hambiente para saber si es seguro el 
 * exterior.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    sensorR	     |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	ACD		 	| 	CH1			|
 * |	+5V	 		| 	+5V	    	|
 * | 	GND 	 	| 	GND	    	|
 *
 * |    dht11	     |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN	1 2 3 	| 	GPIO_1		| 
 * |	+5V	 		| 	+5V	    	|
 * | 	GND 	 	| 	GND	    	|
 * 
 * |    -			 |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	Led1     	| 	GPIO_05		|
 * | 	Led2	    | 	GPIO_11  	|
 * | 	Led3	    | 	GPIO_10  	|
 *
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 09/06/2025 | Document creation		                         |
 *
 * @author Leonel Borgeat (leonelborgeat@gmial.com)
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
#include "dht11.h"
#include "uart_mcu.h"
#include "analog_io_mcu.h"

/*==================[macros and definitions]=================================*/

#define CONFIG_MEDICION_us 1000000
#define CONFIG_RADIACION_us 5000000
#define CONFIG_TECLAS_us 1000

/*==================[internal data definition]===============================*/
	/**
 * @brief Estacion de monitoreo para el Eternauta
 * 
 * @param medir inicia la medicion de los parametros
 * @param valorTemperatura contiene el valor sensado de temperatura en el exterior
 * @param valorHumedad contiene el valor sensado de temperatura en el exterior
 * @param valorRadiacion contiene el valor de radiacion sensado en el exterior
 * @param misLeds contiene los GPIO de los led de la placa 
 * 
 */
bool medir = false;

uint16_t valorTemperatura;

uint16_t valorHumedad;

uint16_t valorRadiacion;

	/**
 * @brief struct para controlar los led de la placa 
 * 
 * @param misLeds contiene los led rojo amarillo y verde de la placa
 *  */ 

typedef struct 
{
	gpio_t led;
} leds;

leds misLeds[3] = {
	{GPIO_11}, /* LED VERDE */
	{GPIO_10}, /*LED AMARILLO*/
	{GPIO_5}}; /*LED ROJO*/ 

TaskHandle_t sensar_task_handle = NULL;
TaskHandle_t radiacion_task_handle = NULL;
/*==================[internal functions declaration]=========================*/

/** @fn FuncTimerA (void *parametro)
 * @brief timer que se encarga de controlar la tarea sensar temperatura y Humedad */

void FuncTimerA(void *parametro)
{
	vTaskNotifyGiveFromISR(sensar_task_handle, pdFALSE); /* Envía una notificación a la tarea asociada a sensar */
}

/** @fn FuncTimerb (void *parametro)
 * @brief timer que se encarga de controlar la tarea medicion de radiacion */

void FuncTimerB(void *parametro)
{
	vTaskNotifyGiveFromISR(radiacion_task_handle, pdFALSE); /* Envía una notificación a la tarea asociada a radiacion */
}

/** @fn sensarHumedad_Temperatura (void *parametro)
 * @brief tarea encargada de sensar la humedad y la temperatura del ambiente */
static void sensarHumedad_Temperatura(void *parametro)
{
	while (true)
	{

		if (medir == true)
		{
			dht11Read(valorHumedad, valorTemperatura); /*lector del sensor de humedad y temperatura*/ 
		}

		if (valorHumedad > 85 && valorTemperatura > 0 && valorTemperatura < 2)
		{
			GPIOOn(misLeds[2].led);

			UartSendString(UART_PC, "Temperatura: ");
			UartSendString(UART_PC, (char *)UartItoa(valorTemperatura, 10));
			UartSendString(UART_PC, "°C -Humedad: ");
			UartSendString(UART_PC, (char *)UartItoa(valorHumedad, 10));
			UartSendString(UART_PC, "-Humedad:  -RIESGO NEVADA");
			UartSendString(UART_PC, "% \r\n");
		}
		else
		{
			UartSendString(UART_PC, "Temperatura: ");
			UartSendString(UART_PC, (char *)UartItoa(valorTemperatura, 10));
			UartSendString(UART_PC, "°C - Humedad: ");
			UartSendString(UART_PC, (char *)UartItoa(valorHumedad, 10));
			UartSendString(UART_PC, "% \r\n");

			GPIOOff(misLeds[2].led);
		}

		ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /*leer cada 1 segundo*/ 
	}
}

/** @fn sensarHumedad_Temperatura (void *parametro)
 * @brief tarea encargada de sensar la humedad y la temperatura del ambiente */
static void medirRadiacion(void *parametro)
{
	
	while (true)
	{

		if (medir == true)
		{
			AnalogInputReadSingle(CH1, &valorRadiacion);   /*valor en mV*/ 
			valorRadiacion = valorRadiacion / 1000;		   /*convierto de mV a V*/ 
			valorRadiacion = (valorRadiacion * 100) / 3.3; /*Convierto a valores de 0-100*/ 

			if (valorRadiacion > 40)
			{
				UartSendString(UART_PC, "Radiacion: ");
				UartSendString(UART_PC, (char *)UartItoa(valorRadiacion, 10));
				UartSendString(UART_PC, "mR/h - RADIACION ELEVADA \r\n");
				GIPIOOn(misLeds[1].led);

			}
			else
			{
				UartSendString(UART_PC, "Radiacion: ");
				UartSendString(UART_PC, (char *)UartItoa(valorRadiacion, 10));
				UartSendString(UART_PC, "mR/h \r\n");
				if (valorHumedad < 85 && valorTemperatura < 0 && valorTemperatura > 2)
				{
					GPIOOn(misLeds[0].led);
				}
			}
		}
	}

	ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /*leer cada 5 segundo*/ 
}

/** @fn tecla_1 ()
 * @brief funcion que se encarga de controlar la tecla 1 */
void tecla_1()
{
	medir = true;
}
/** @fn tecla_2 ()
 * @brief funcion que se encarga de controlar la tecla 1 */
void tecla_2()
{
	medir = false;
}

/*==================[external functions definition]==========================*/
void app_main(void)
{
	dht11Init( GPIO_1 ); /*Inicializo el sensor dht11*/

	SwitchesInit(); /*Inicializo los switches para las teclas 1 y 2 */
	SwitchActivInt(SWITCH_1, tecla_1, NULL);
	SwitchActivInt(SWITCH_2, tecla_2, NULL);

	analog_input_config_t miStruct = { 			
		.input = CH1,
		.mode = ADC_SINGLE,
		.func_p = NULL,
		.param_p = NULL,
		.sample_frec = 0};
	AnalogInputInit(&miStruct);   /*Inicializo mi conversor analog digital */

	serial_config_t my_uart = {
		.port = UART_PC,
		.baud_rate = 9600,
		.func_p = NULL,
		.param_p = NULL

	};
	UartInit(&my_uart);  /*inicializo UART para mi comunicacion con la pc*/

	timer_config_t timer_medicion = {
		.timer = TIMER_A,
		.period = CONFIG_MEDICION_us,
		.func_p = FuncTimerA,
		.param_p = NULL};
	TimerInit(&timer_medicion); /*Inicializacion del timer para medir cada 1 segundo*/
 
	timer_config_t timer_radiacion = {
		.timer = TIMER_B,
		.period = CONFIG_RADIACION_us,
		.func_p = FuncTimerB,
		.param_p = NULL};
	TimerInit(&timer_radiacion); /*Inicializacion del timer para medir cada 5 segundo*/

	xTaskCreate(&sensarHumedad_Temperatura, "sensa ambiente", 2048, NULL, 5, &sensar_task_handle);  /*creacion de la tarea para sensar temp y humedad	*/
	xTaskCreate(&medirRadiacion, "obtiene valor radiacion", 2048, NULL, 5, &radiacion_task_handle); /*creacion de la tarea para obtener el valor de radiacion*/ 

	TimerStart(timer_medicion.timer); /*Inicializacion del timer de temp y humedad 1 segundo*/ 	
	TimerStart(timer_radiacion.timer); /*Inicializacion del timer radiacion 5 segundos*/ 
}
/*==================[end of file]============================================*/