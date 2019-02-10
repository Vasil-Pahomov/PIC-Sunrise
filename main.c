#include <pic12f675.h>
// PIC12F675 Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSC oscillator: I/O function on GP4/OSC2/CLKOUT pin, I/O function on GP5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-Up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // GP3/MCLR pin function select (GP3/MCLR pin function is MCLR)
#pragma config BOREN = ON       // Brown-out Detect Enable bit (BOD enabled)
#pragma config CP = OFF         // Code Protection bit (Program Memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)

#define setbit(var, bit) ((var) |= (1 << (bit)))
#define clrbit(var, bit) ((var) &= ~(1 << (bit)))

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>

#define INCREASE_DIVIDER 400 //начальный делитель увеличения яркости (через сколько полупериодов сети яркость увеличивается на 1)
#define MIN_DIVIDER 50 //минимальное значение делителя
#define PERIOD 1050 //начальное значение фазы (в тактах таймера 1)
#define LED_BLINK_PERIOD 200//период мигания индикаторного светодиода, в полупериодах сети
#define _XTAL_FREQ 4000000


short counter; //текущий счётчик циклов внутри периода сети
unsigned char ledCounter; //счётчик для подсветки индикаторного светодиода
short phase; //текущее значение фазы открывания (PERIOD...0)
short incDivider;//текущее значение делителя
short divCounter; //счётчик для делителя увеличения яркости
short lastVal;
short GPIOs; //GPIO shadow

void interrupt isr() {
   if (GPIF) {
      //прерывание от GPIO - полупериод сети
      GPIF=0;
      lastVal = GP4;

      divCounter--;
      if (divCounter <= 0) {
	 if (phase > 1) {
	    phase--;
	 }
	 divCounter=incDivider;
	 if (incDivider > MIN_DIVIDER) {
	   incDivider--;
	 }
      }
      
      ledCounter--;
      if (ledCounter == 0) {
	 ledCounter = LED_BLINK_PERIOD;
      }
       if (ledCounter < LED_BLINK_PERIOD / 10) {
	  setbit(GPIOs,1);
       } else {
	  clrbit(GPIOs,1);
       }
       GPIO=GPIOs;

       TMR1 = -phase;
       TMR1ON=1;
       
       GPIE=0;

   } else if (TMR1IF) {
     //прерывание по таймеру 1
      TMR1ON=0;
      TMR1IF=0;
      clrbit(GPIOs,5); 
      GPIO=GPIOs;
      lastVal=10;
      while (lastVal--);
      setbit(GPIOs,5);
      GPIO=GPIOs;      
      GPIE=1;
      
   }   
}

void main(void){
    
     
    //Ќастройка портов ввода/вывода PIC12F675
    //¬ыключение подтягивающих резисторов
    WPU = 0x00110000;
    //«апретить использование подтягивающих резисторов
    nGPPU = 0;
   
    ANSEL = 0;//set ports as digital IO, not analog inputs
    ADCON0 = 0;//shut down ADC
    CMCON = 0;//shut down comparator
    VRCON = 0;//shut down Vref
     
    //Ќастрока выходов GP1 out, GP5 out, остальные IN
    TRISIO = 0b00011101;
    setbit(GPIOs,5);
    clrbit(GPIOs,1);
    GPIO=GPIOs;    
    
     
    /*
    GP1=1;
    __delay_ms(200);
    GP1=0;
    __delay_ms(1000);
*/
    //прерывание по GP4
    IOC4=1;
    //разрешение прерываний
    GIE=1;
    GPIE=1;
    PEIE=1;
    TMR1IE=1;
 /**/
    phase = PERIOD;
    divCounter = INCREASE_DIVIDER;
    ledCounter = LED_BLINK_PERIOD;
    counter=0;
    incDivider=INCREASE_DIVIDER;
    
    T1CON=0b00110001; //настройка таймера 1 на работу от внутреннего тактового генератора, делитель 8
    TMR1=-phase;
    
    while(1) {
    }
  
}
