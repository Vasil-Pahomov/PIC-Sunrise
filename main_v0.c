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

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>

#define MAINS_BOUNCE_THRESHOLD 10 //величина защитного периода отслеживания перехода сети через 0 (антидребезг)
#define INCREASE_DIVIDER 1 //делитель увеличения яркости (через сколько периодов сети яркость увеличивается на 1)
#define WARMUP_DELAY 1 //задержка начала повышения яркости (для стабилизации измерений периода сети)
#define PULSE_WIDTH 50 //длительность импульса открытия тиристора (в тактах таймера 1)
#define _XTAL_FREQ 4000000

void interrupt isr() {
}

void main(void){
    
    short counter; //текущий счётчик циклов внутри периода сети
    unsigned char ledCounter; //счётчик для подсветки индикаторного светодиода
    short period;//значение периода сети
    short brightness; //текущее значение яркости (0...period)
    short divCounter; //счётчик для делителя увеличения яркости
    unsigned char prevVal;//предыдущее значение полярности сети (для отслеживания перехода через 0)
     
    //Настройка портов ввода/вывода PIC12F675
    //Выключение подтягивающих резисторов
    WPU = 0x00110000;
    //Запретить использование подтягивающих резисторов
    nGPPU = 0;
   
    ANSEL = 0;//set ports as digital IO, not analog inputs
    ADCON0 = 0;//shut down ADC
    CMCON = 0;//shut down comparator
    VRCON = 0;//shut down Vref
     
    //Настрока выходов GP1 out, GP5 out, остальные IN
    TRISIO = 0b00011101;
    GP5=1;
    GP1=0;
     
    
    GP1=1;
    __delay_ms(200);
    GP1=0;
    __delay_ms(1000);

 /**/
    prevVal = 0;
    brightness = -WARMUP_DELAY;
    divCounter = INCREASE_DIVIDER;
    ledCounter = 50;
    period = 0;
    counter=0;
    
    T1CON=0b00110001; //настройка таймера 1 на работу от внутреннего тактового генератора, делитель 8
    TMR1=0;
    
    while(1) {
        unsigned char val = GP4;
        //отслеживание перехода сети через 0
       counter=TMR1;
        if (val != prevVal && counter >= MAINS_BOUNCE_THRESHOLD) {
            if (counter > period) {//ищем максимум, чтобы значение периода не "плавало" в процессе
	       period = counter;
	    }
            TMR1 = 0;
	    divCounter--;
	    if (divCounter == 0) {
	       if (brightness < period ) {
		  brightness++;
	       }
	       divCounter=INCREASE_DIVIDER;
	    }
	    /*
	    ledCounter--;
	    if (ledCounter == 0) {
	       ledCounter = 50;
	    }
	    GP1 = (ledCounter > 45) ? 1 : 0;/**/
        }    
        prevVal = val;
        
        if ((counter >= (period - brightness)) && (counter <= (period - brightness + PULSE_WIDTH)) && (brightness > 0)) {
            GP5 = 0; 
	} else {
            GP5 = 1;
        }         
        //counter++;
        
    }
 /**/    
 /* 
    counter = 0;
    //Бесконечный цикл. 
    while(1){
        //Установка GP4 в лог. 1, GP1 в лог. 0
        GPIO = 0x10;
        //Задержка на 5000 циклов
        for(counter = 0; counter < 5000; counter++);
        //Установка GP5 в лог. 0, GP1 в лог. 1
        GPIO = 0x20;
        //Задержка на 5000 циклов
        for(counter = 0; counter < 20000; counter++);
    }
   /**/   
}
