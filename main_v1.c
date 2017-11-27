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

#define MAINS_BOUNCE_THRESHOLD 100 //�������� ��������� ������� ������������ �������� ���� ����� 0 (�����������), � ������ ������� 1
#define INCREASE_DIVIDER 1 //�������� ���������� ������� (����� ������� �������� ���� ������� ������������� �� 1; 1 ������� �������� 12 ������)
#define PULSE_WIDTH 20 //������������ �������� �������� ��������� (� ������ ������� 1)
#define PERIOD 1050//��������� �������� ���� (� ������ ������� 1)
#define LED_BLINK_PERIOD 100//������ ������� ������������� ����������, � �������� ����
#define _XTAL_FREQ 4000000


short counter; //������� ������� ������ ������ ������� ����
unsigned char ledCounter; //������� ��� ��������� ������������� ����������
short phase; //������� �������� ���� ���������� (0...PERIOD)
short divCounter; //������� ��� �������� ���������� �������
short lastVal;
short GPIOs; //GPIO shadow

void interrupt isr() {
   if (!GPIF) return;
   GPIF=0;
   //GP5=1;
   lastVal = GP4;
   if (TMR1 >= MAINS_BOUNCE_THRESHOLD) {
      TMR1 = 0;
      //counter = 0;
      divCounter--;
      if (divCounter <= 0) {
	 if (phase > 0) {
	    phase--;
	 }
	 divCounter=INCREASE_DIVIDER;
      }
      
      ledCounter--;
      if (ledCounter == 0) {
	 ledCounter = LED_BLINK_PERIOD;
      }
       if (ledCounter > LED_BLINK_PERIOD / 10) {
	  setbit(GPIOs,1);
       } else {
	  clrbit(GPIOs,1);
       }
      
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
    GP5=1;
    GP1=0;
    
     
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
 /**/
    phase = PERIOD-PULSE_WIDTH;
    divCounter = INCREASE_DIVIDER;
    ledCounter = LED_BLINK_PERIOD;
    counter=0;
    
    T1CON=0b00110001; //��������� ������� 1 �� ������ �� ����������� ��������� ����������, �������� 8
    TMR1=0;
    
    while(1) {
      counter=TMR1;
      if ((counter <= (phase + PULSE_WIDTH)) && (counter >= phase)) {
         clrbit(GPIOs,5); 
       } else {
         setbit(GPIOs,5);
       } 
       GPIO=GPIOs;
    }
  
}
