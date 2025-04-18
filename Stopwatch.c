#include <stdio.h> 
#include <stdlib.h> 
#include <system.h> 
#include <io.h> 
#include <math.h> 
#include "unistd.h" 
#include "altera_avalon_pio_regs.h" 
#include "sys/alt_irq.h" 
#include "altera_avalon_timer_regs.h" 
#include "sys/alt_stdio.h" 
#include "altera_avalon_jtag_uart_regs.h" 
int hex_display[16] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 
0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71}; 
//Displays the numbers on the seven - segment displays 
//global variables that will be user throughout the program 
volatile int edge_capture = 0; 
int enable = 0; 
int temp = 0; 
int count = 0; 
int H0, H1, H3, H2, H4, H5, H6, H7 = 0; 
char start_msg [6] = {'s', 't', 'a', 'r', 't', '\n'}; 
char stop_msg [5] = {'s', 't', 'o', 'p', '\n'}; 
char reset_msg [6] = {'r', 'e', 's', 'e', 't', '\n'}; 
static void keys_isr(void* context, alt_u32 id) 
{ 
volatile int* edge_capture_ptr = (volatile int*) context; 
/* 
* Read the edge capture register on the button PIO. 
* Store value. 
*/ 
*edge_capture_ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(KEY_BASE); 
if(edge_capture & 0x1){//Key 0 start/stop 
if(enable){//If enable is true then we need to disable. 
IOWR_ALTERA_AVALON_TIMER_CONTROL(SYSTEM_TIMER_BASE, 
0x9);//write to control register. stop timer 
IOWR_32DIRECT(LED_BASE, 0, 0x3FFFF); 
IOWR_16DIRECT(LEDG_BASE, 0, 0x0); 
enable = 0; 
11 
12 
 
   for(int i = 0; i < 5; i++){ 
   
 IOWR_ALTERA_AVALON_JTAG_UART_DATA(JTAG_BASE, stop_msg[i]); 
   } 
  }else{//If enable is not true then we need to enable. 
  
 IOWR_ALTERA_AVALON_TIMER_CONTROL(SYSTEM_TIMER_BASE, 0x7); 
   IOWR_32DIRECT(LED_BASE, 0, 0x0); 
   IOWR_16DIRECT(LEDG_BASE, 0, 0xFF); 
   enable = 1; 
   for(int i = 0; i < 6; i++){ 
   
 IOWR_ALTERA_AVALON_JTAG_UART_DATA(JTAG_BASE, start_msg[i]); 
   } 
  } 
 }else if(edge_capture & 0x2){//if key 1 pressed then reset 
  for(int i = 0; i < 6; i++){ 
   IOWR_ALTERA_AVALON_JTAG_UART_DATA(JTAG_BASE, 
reset_msg[i]); 
  } 
  //Resetting all seven - segment displays and LEDs 
  IOWR_16DIRECT(HEX_0_BASE, 0, hex_display[0]); 
  IOWR_16DIRECT(HEX_3_BASE, 0, hex_display[0]); 
  IOWR_16DIRECT(HEX_1_BASE, 0, hex_display[0]); 
  IOWR_16DIRECT(HEX_2_BASE, 0, hex_display[0]); 
  IOWR_16DIRECT(HEX_4_BASE, 0, hex_display[0]); 
  IOWR_16DIRECT(HEX_5_BASE, 0, hex_display[0]); 
  IOWR_16DIRECT(HEX_6_BASE, 0, hex_display[0]); 
  IOWR_16DIRECT(HEX_7_BASE, 0, hex_display[0]); 
  IOWR_16DIRECT(LED_BASE, 0, 0); 
  IOWR_16DIRECT(LEDG_BASE, 0, 0); 
  enable = 0;//disable timer 
  count = 0;//reset count 
  IOWR_ALTERA_AVALON_TIMER_CONTROL(SYSTEM_TIMER_BASE, 
0x9);//write to control register. stop timer 
  IOWR_ALTERA_AVALON_TIMER_PERIODL(SYSTEM_TIMER_BASE, 
0xf080);//Low side. 
  IOWR_ALTERA_AVALON_TIMER_PERIODH(SYSTEM_TIMER_BASE, 
0x2fa);//High side. 
  //reload the value for one second into the periol and periodh register for 
the timer 
 } 
 edge_capture = 0; 
 /* Write to the edge capture register to reset it. */ 
 IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY_BASE, 0); 
 /* reset interrupt capability for the Button PIO. */ 
 IOWR_ALTERA_AVALON_PIO_IRQ_MASK(KEY_BASE, 0xF); 
} 
/* Initialize the button_pio. */ 
static void init_keys() 
{ 
/* Recast the edge_capture pointer to match the 
alt_irq_register() function prototype. */ 
void* edge_capture_ptr = (void*) &edge_capture; 
/* Enable all 4 button interrupts. */ 
IOWR_ALTERA_AVALON_PIO_IRQ_MASK(KEY_BASE, 0xF); 
/* Reset the edge capture register. */ 
IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY_BASE, 0x0); 
/* Register the ISR. */ 
alt_ic_isr_register(KEY_IRQ_INTERRUPT_CONTROLLER_ID,KEY_IRQ, 
keys_isr, edge_capture_ptr, 0x0); 
} 
static timer_isr(void* context, alt_u32 id){ 
//Clear the interrupt 
IOWR_ALTERA_AVALON_TIMER_STATUS(SYSTEM_TIMER_BASE, 0); 
count++; 
temp = count; 
//In this section, each individual digit from count is retreived 
H0 = temp % 10; 
temp = temp / 10; 
H3 = temp % 10; 
temp = temp / 10; 
H1 = temp % 10; 
temp = temp / 10; 
H2 = temp % 10; 
temp = temp / 10; 
H4 = temp % 10; 
temp = temp / 10; 
H5 = temp % 10; 
temp = temp / 10; 
H6 = temp % 10; 
temp = temp / 10; 
H7 = temp % 10; 
temp = temp / 10; 
13 
14 
 
 
 IOWR_16DIRECT(HEX_0_BASE, 0, hex_display[H0]); 
 IOWR_16DIRECT(HEX_3_BASE, 0, hex_display[H3]); 
 IOWR_16DIRECT(HEX_1_BASE, 0, hex_display[H1]); 
 IOWR_16DIRECT(HEX_2_BASE, 0, hex_display[H2]); 
 IOWR_16DIRECT(HEX_4_BASE, 0, hex_display[H4]); 
 IOWR_16DIRECT(HEX_5_BASE, 0, hex_display[H5]); 
 IOWR_16DIRECT(HEX_6_BASE, 0, hex_display[H6]); 
 IOWR_16DIRECT(HEX_7_BASE, 0, hex_display[H7]); 
 
 
} 
 
void init_timer(void){ 
 //Register ISR with HAL 
 alt_irq_register(SYSTEM_TIMER_IRQ, NULL, timer_isr); 
 IOWR_ALTERA_AVALON_TIMER_CONTROL(SYSTEM_TIMER_BASE, 
0x9);//write to control register. stop timer 
 IOWR_ALTERA_AVALON_TIMER_PERIODL(SYSTEM_TIMER_BASE, 
0xf080);//Low side. 
 IOWR_ALTERA_AVALON_TIMER_PERIODH(SYSTEM_TIMER_BASE, 
0x2fa);//High side. 
 //Timer period of one second. 
} 
 
 
int main() 
{ 
 for(int i = 0; i < 16; i++){ 
 hex_display[i] = hex_display[i] ^ 0xFF;//inverting bits to match the active low 
inputs to the 
                      //seven - 
segment displays. 
 } 
 //Resetting all seven - segment displays and LEDs 
 IOWR_16DIRECT(HEX_0_BASE, 0, hex_display[0]); 
 IOWR_16DIRECT(HEX_3_BASE, 0, hex_display[0]); 
 IOWR_16DIRECT(HEX_1_BASE, 0, hex_display[0]); 
 IOWR_16DIRECT(HEX_2_BASE, 0, hex_display[0]); 
 IOWR_16DIRECT(HEX_4_BASE, 0, hex_display[0]); 
 IOWR_16DIRECT(HEX_5_BASE, 0, hex_display[0]); 
 IOWR_16DIRECT(HEX_6_BASE, 0, hex_display[0]); 
 IOWR_16DIRECT(HEX_7_BASE, 0, hex_display[0]); 
 IOWR_16DIRECT(LED_BASE, 0, 0); 
 IOWR_16DIRECT(LEDG_BASE, 0, 0); 
 
 init_keys(); 
init_timer(); 
while(1){ 
} 
return 0; 
}