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
 * |   Pins      |   SP32		|
 * |:--------------:|:-------------:|
 * | 	GPIO_20 	|	D1      	|
 * | 	GPIO_21		| 	D2			|
 * | 	GPIO_22	 	| 	D3			|
 * | 	GPIO_23	    | 	D4	     	|
 * | 	GPIO_19	    | 	SEL_1     	|
 * | 	GPIO_18	    | 	SEL_2     	|
 * | 	GPIO_9	    | 	SEL_3     	|
 * | 	+5V		    | 	+5V     	|
 * | 	GND		    | 	GND	    	|
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
#include "gpio_mcu.h" 
#include "led.h"
#include "switch.h"
#include "gpio_mcu.h"

/*==================[macros and definitions]=================================*/
/**
 * @brief Conversor de BCD a arreglo de numeros
 * 
 * @param data ingresa el dato "numero a convertir"
 * @param digits define la cantidad de digitos que tiene el dato
 * @param bcd_number arreglo que guarda los digitos por separado
 * 
 * @return 0
 */

int8_t convertToBcdArray1(uint32_t data, uint8_t digits, uint8_t * bcd_number)
{
	for (uint8_t i = 0; i < digits; i++)
	{
		bcd_number[digits - 1 - i] = data % 10;
		data = data / 10;
	}
	
	return 0;
}
	/* Definición de estructura de datos GPIO */

typedef struct 
{
	gpio_t pin;			/*!< GPIO pin number */
	io_t dir;			/*!< GPIO direction '0' IN;  '1' OUT*/

} gpioConf_t;



	/* Definición de los pines GPIO para los segmentos del display */

gpioConf_t gpioArray[4] = {
    {GPIO_20, GPIO_OUTPUT},
    {GPIO_21, GPIO_OUTPUT},
    {GPIO_22, GPIO_OUTPUT},
    {GPIO_23, GPIO_OUTPUT}};

/**
 * @brief Configura los pines GPIO segun el valor de bcd 
 * 
 * @param digitoBCD	digito a configurar 
 * @param gpioConf arreglo con la configuracion de pins GPIO
 * 
 * @return 0
 */
void bcdAPin (uint8_t digitoBCD, gpioConf_t gpioConf)
{
 for (uint8_t i=0; i<4;i++)
 { 
	if ((digitoBCD & (1<<i)) == 0)
       GPIOOff(gpioArray[i].pin);
	   
	   else
	   GPIOOn (gpioArray[i].pin);

 }
}
/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

gpioConf_t gpioMap[3] = {
    {GPIO_19, GPIO_OUTPUT},
    {GPIO_18, GPIO_OUTPUT},
    {GPIO_9, GPIO_OUTPUT}};

	/**
 * @brief Conversor de BCD a arreglo de numeros
 * 
 * @param data ingresa el dato "numero a convertir"
 * @param digits define la cantidad de digitos que tiene el dato
 * @param gpioArray arreglo con la configuracion de pins para los segmentos
 * @param gpioMap arreglo con la configuracion de pins para los digitos
 * @param bcd_number arreglo que guarda los datos en BCD a mostrar en el display
 * 
 * 
 * @return 0
 */


void mostrarDisplay(uint32_t data, uint8_t digits, gpioConf_t *gpioArray, gpioConf_t *gpioMap, uint8_t *bcd_number)
	{
	   //Aca verifico si el numero es demasiado grande
		if (convertToBcdArray1(data, digits, bcd_number) != 0)
		{
			printf("Error: Número demasiado grande para el display.\n");
			return;
		}
	
		for (int i = 0; i < digits; i++)
		{
			bcdAPin(bcd_number[i], *gpioArray); // Configura segmentos
			
			GPIOOn(gpioMap[i].pin); // Activa el dígito actual
			
			GPIOOff(gpioMap[i].pin); // Desactiva el dígito
		}
	}
   



/*==================[external functions definition]==========================*/
void app_main(void)
{
	uint32_t numero = 786;
	uint8_t digitos = 3;
	uint8_t arreglo[digitos];
	
	convertToBcdArray1(numero, digitos, arreglo);
	printf("el digito 1 es %d \n", arreglo[0]);
	printf("el digito 2 es %d \n", arreglo[1]);
	printf("el digito 3 es %d \n", arreglo[2]);

	gpioConf_t gpioArray[4] = {
        {GPIO_20, GPIO_OUTPUT},
        {GPIO_21, GPIO_OUTPUT}, // Conectado a la pantalla
        {GPIO_22, GPIO_OUTPUT},
        {GPIO_23, GPIO_OUTPUT}  // Vector de 4 estructuras gpioConf_t
    };	

    gpioConf_t gpioMap[3] = {
        {GPIO_19, GPIO_OUTPUT},
        {GPIO_18, GPIO_OUTPUT}, // Conectado a la pantalla
        {GPIO_9, GPIO_OUTPUT}
    };

	    // Inicializa los GPIOs del array gpioArray
		for (int i = 0; i < 4; i++)
		{
			GPIOInit(gpioArray[i].pin, gpioArray[i].dir);
		}
	
		// Inicializa los GPIOs del array gpioMap
		for (int i = 0; i < 3; i++)
		{
			GPIOInit(gpioMap[i].pin, gpioMap[i].dir);
		}
	
		// Muestra el número en el display
		mostrarDisplay(numero, digitos, gpioArray, gpioMap, arreglo);
}
/*==================[end of fie]============================================*/