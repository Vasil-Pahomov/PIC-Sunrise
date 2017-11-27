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

#define INCREASE_DIVIDER 400 //��������� �������� ���������� ������� (����� ������� ������������ ���� ������� ������������� �� 1)
#define MIN_DIVIDER 50 //����������� �������� ��������
#define PERIOD 1050 //��������� �������� ���� (� ������ ������� 1)
#define LED_BLINK_PERIOD 200//������ ������� ������������� ����������, � ������������ ����
#define _XTAL_FREQ 4000000


short counter; //������� ������� ������ ������ ������� ����
unsigned char ledCounter; //������� ��� ��������� ������������� ����������
short phase; //������� �������� ���� ���������� (0...PERIOD)
short incDivider;//������� �������� ��������
short divCounter; //������� ��� �������� ���������� �������
short lastVal;
short GPIOs; //GPIO shadow

void interrupt isr() {
   if (GPIF) {
      //���������� �� GPIO - ���������� ����
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
     //���������� �� ������� 1
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
    
     
    //��������� ������ �����/������ PIC12F675
    //���������� ������������� ����������
    WPU = 0x00110000;
    //��������� ������������� ������������� ����������
    nGPPU = 0;
   
    ANSEL = 0;//set ports as digital IO, not analog inputs
    ADCON0 = 0;//shut down ADC
    CMCON = 0;//shut down comparator
    VRCON = 0;//shut down Vref
     
    //�������� ������� GP1 out, GP5 out, ��������� IN
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
    //���������� �� GP4
    IOC4=1;
    //���������� ����������
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
    
    T1CON=0b00110001; //��������� ������� 1 �� ������ �� ����������� ��������� ����������, �������� 8
    TMR1=-phase;
    
    while(1) {
    }
  
}
