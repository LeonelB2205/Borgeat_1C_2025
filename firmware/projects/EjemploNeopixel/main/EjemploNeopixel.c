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
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "neopixel_stripe.h"
#include "gpio_mcu.h"
/*==================[macros and definitions]=================================*/
#define RGB_LED_PIN GPIO_16 /*> ESP32-C6-DevKitC-1 NeoPixel it's connected at GPIO_8 */
#define RGB_LED_LENGTH 16	/*> ESP32-C6-DevKitC-1 NeoPixel has one pixel */
/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

void prenderLeds(uint8_t led)
{
	NeoPixelAllColor(NEOPIXEL_OFF);

	switch (led)
	{
	case 1:
		NeoPixelSetPixel(0, NEOPIXEL_COLOR_BLUE);
		NeoPixelSetPixel(1, NEOPIXEL_COLOR_BLUE);
		NeoPixelSetPixel(2, NEOPIXEL_COLOR_BLUE);
		NeoPixelSetPixel(3, NEOPIXEL_COLOR_BLUE);
		break;
	case 2:
		NeoPixelSetPixel(4, NEOPIXEL_COLOR_GREEN);
		NeoPixelSetPixel(5, NEOPIXEL_COLOR_GREEN);
		NeoPixelSetPixel(6, NEOPIXEL_COLOR_GREEN);
		NeoPixelSetPixel(7, NEOPIXEL_COLOR_GREEN);
		break;
	case 3:
		NeoPixelSetPixel(8, NEOPIXEL_COLOR_RED);
		NeoPixelSetPixel(9, NEOPIXEL_COLOR_RED);
		NeoPixelSetPixel(10, NEOPIXEL_COLOR_RED);
		NeoPixelSetPixel(11, NEOPIXEL_COLOR_RED);
		break;
	default:
		break;
	}
}

/*==================[external functions definition]==========================*/
void app_main(void)
{

	static neopixel_color_t color[RGB_LED_LENGTH];

	NeoPixelInit(RGB_LED_PIN, RGB_LED_LENGTH, color);

	NeoPixelAllOff();
	uint8_t i = 0;
	
	while(1)
	{
		prenderLeds(i+1);

		i++;
		i = i % 3;
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}

}
/*==================[end of file]============================================*/