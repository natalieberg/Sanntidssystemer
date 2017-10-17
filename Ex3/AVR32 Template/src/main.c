/**
 * AVR32 UC3 template for TTK4147
 */

// include header files for all drivers that have been imported from AVR Software Framework (ASF).
#include <asf.h>
#include <board.h>
#include <gpio.h>
#include <sysclk.h>
#include "busy_delay.h"

// defines for USB UART
#define CONFIG_USART_IF (AVR32_USART2)

// defines for BRTT interface
#define TEST_A AVR32_PIN_PA31
#define RESPONSE_A AVR32_PIN_PA30
#define TEST_B AVR32_PIN_PA29
#define RESPONSE_B AVR32_PIN_PA28
#define TEST_C AVR32_PIN_PA27
#define RESPONSE_C AVR32_PIN_PB00

uint32_t flagA = 0;
uint32_t flagB = 0;
uint32_t flagC = 0;

// declare interrupts
__attribute__((__interrupt__)) 
static void interrupt_J3(void);

void init()
{
	// board init
	sysclk_init();
	board_init();
	busy_delay_init(BOARD_OSC0_HZ);
	
	// interrupts init
	cpu_irq_disable();
    INTC_init_interrupts();
	INTC_register_interrupt(&interrupt_J3,   AVR32_GPIO_IRQ_3, AVR32_INTC_INT1);
	cpu_irq_enable();

	//  stdio init
	stdio_usb_init(&CONFIG_USART_IF);

	// Specify that stdout and stdin should not be buffered.

#if defined(__GNUC__) && defined(__AVR32__)
	setbuf(stdout, NULL);
	setbuf(stdin,  NULL);
#endif
}

/*********************************************************************
User decelerations
*********************************************************************/

/*********************************************************************
Interrupt routines
*********************************************************************/
__attribute__((__interrupt__)) 
static void interrupt_J3(void) 
{ 
	if (gpio_get_pin_interrupt_flag(TEST_A) == 1)
	{
		flagA = 1;
		gpio_clear_pin_interrupt_flag(TEST_A);
	}
	if (gpio_get_pin_interrupt_flag(TEST_B) == 1)
	{
		flagB = 1;
		gpio_clear_pin_interrupt_flag(TEST_B);
	}
	if (gpio_get_pin_interrupt_flag(TEST_C) == 1)
	{
		flagC = 1;
		gpio_clear_pin_interrupt_flag(TEST_C);
	}	
}

/*********************************************************************
Functions
*********************************************************************/
int main (void)
{
	int i;
	// initialize
	init();
	// start code from here
	gpio_configure_pin (TEST_A, (GPIO_DIR_INPUT | GPIO_PULL_UP));
	gpio_configure_pin (RESPONSE_A, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	
	gpio_configure_pin (TEST_B, (GPIO_DIR_INPUT | GPIO_PULL_UP));
	gpio_configure_pin (RESPONSE_B, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	
	gpio_configure_pin (TEST_C, (GPIO_DIR_INPUT | GPIO_PULL_UP));
	gpio_configure_pin (RESPONSE_C, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	
	gpio_set_pin_high(RESPONSE_A);
	gpio_set_pin_high(RESPONSE_B);
	gpio_set_pin_high(RESPONSE_C);
	
	gpio_enable_pin_interrupt(TEST_A, GPIO_PIN_CHANGE);
	gpio_enable_pin_interrupt(TEST_B, GPIO_PIN_CHANGE);
	gpio_enable_pin_interrupt(TEST_C, GPIO_PIN_CHANGE);

	//while(1)
	//{
		////Del 1
		////gpio_toggle_pin(LED0_GPIO);
		////busy_delay_ms(500);
		////gpio_toggle_pin(LED1_GPIO);
		////gpio_set_pin_high(LED2_GPIO);
		////busy_delay_ms(500);
		////gpio_set_pin_low(LED2_GPIO);
		////gpio_set_pin_low(LED3_GPIO);
		////busy_delay_ms(500);
		////
		////gpio_set_pin_high(LED3_GPIO);
		////printf("hey\n");
		////
		////busy_delay_ms(500);
		//
		////Assignment A
		//
		//if (gpio_pin_is_low(TEST_A)){
			//gpio_set_pin_low(RESPONSE_A);
			//busy_delay_ms(0.005);
			//gpio_set_pin_high(RESPONSE_A);
		//}
		//
		//if (gpio_pin_is_low(TEST_B)){
			//gpio_set_pin_low(RESPONSE_B);
			//busy_delay_ms(0.005);
			//gpio_set_pin_high(RESPONSE_B);
		//}
//
		//if (gpio_pin_is_low(TEST_C)){
			//gpio_set_pin_low(RESPONSE_C);
			//busy_delay_ms(0.005);
			//gpio_set_pin_high(RESPONSE_C);
		//}
//
		//
	//}
		
		
	
	while (1)
	{
		if(flagA){
			gpio_set_pin_low(RESPONSE_A);
			busy_delay_ms(0.005);
			gpio_set_pin_high(RESPONSE_A);
			flagA = 0;
		}
		if(flagB){
			gpio_set_pin_low(RESPONSE_B);
			busy_delay_ms(0.005);
			gpio_set_pin_high(RESPONSE_B);
			flagB = 0;
		}
		if(flagC){
			gpio_set_pin_low(RESPONSE_C);
			busy_delay_ms(0.005);
			gpio_set_pin_high(RESPONSE_C);
			flagC = 0;
		}
	}
	
	
	
}
