#include "main.h"
#include "driverlib/driverlib.h"
#include "hal_LCD.h"
#include <msp430.h>

typedef enum state{SENS1, SENS2} STATE;
/*
 * This project contains some code samples that may be useful.
 *
 */

char ADCState = 0; //Busy state of the ADC
int16_t ADCResult = 0; //Storage for the ADC conversion result
volatile uint16_t time;
volatile uint16_t time2;
volatile uint16_t threshold1=0;
volatile uint16_t threshold2=0;
volatile uint16_t threshold3=0;
volatile uint16_t threshold4=0;
volatile uint16_t threshold5=0;
STATE sensor = SENS1;
uint16_t Buff[10];
int p=0;
int g=0;
int index=0;
int size = sizeof(Buff);

Init_data(){
    int i;

    for(i=0;i<size;i++)
        Buff[i] = 0;
}


void getdata(uint16_t entry, uint16_t *time){
    Buff[index] = entry;
    int i;
    for(i=0;i<size;i++){
        *time += Buff[i];
    }

     *time /= size;
    index = (index+1)%size;
}


void show2bytes(uint16_t num, uint16_t num2){

    showChar((char)((num / 100) % 10) + '0', pos1);
    showChar((char)((num / 10) % 10) + '0', pos2);
    showChar((char)(num % 10) + '0', pos3);

    showChar((char)((num2 / 100) % 10) + '0', pos4);
    showChar((char)((num2 / 10) % 10) + '0', pos5);
    showChar((char)(num2 % 10) + '0', pos6);

}

void showstepup(){
    char *s ="INPUT IN CM\0";
    displayScrollText(s);
}
void showsthreshold(int state, int threshold){

    if( state >= 4 && p==1){
        displayScrollText("FORNT SENSOR");
        p++;
    }
    if(state <4 && p==0){
        displayScrollText("REAR SENSOR");
        p++;
    }

    showChar((char)((threshold / 100) % 10) + '0', pos1);
    showChar((char)((threshold / 10) % 10) + '0', pos2);
    showChar((char)(threshold % 10) + '0', pos3);

    showChar((char)('T'), pos4);
    showChar((char)('H'), pos5);
    showChar((char)(state % 10) + '0', pos6);
}

#pragma vector=PORT2_VECTOR
__interrupt
void PORT2_VECTOR_ISR(void){
    int got_thresholds = 0;
    threshold1=0;
    threshold2=0;
    threshold3=0;
    threshold4=0;
    threshold5=0;

    char buttonState=0;
    char buttonState2=0;

    uint16_t state=0;
    showstepup();
    while(!got_thresholds){

        if ((GPIO_getInputPinValue(SW1_PORT, SW1_PIN) == 1) & (buttonState == 0)){

            switch(state){
            case(0):
                    break;
            case(1):

                    threshold1 += 10;
                    showsthreshold(state, threshold1);
                    break;
            case(2):

                    threshold2 += 10;
                    showsthreshold(state, threshold2);
                    break;
            case(3):
                    threshold3 += 10;
                    showsthreshold(state, threshold3);
                    break;
            case(4):
                    threshold4 += 10;
                    showsthreshold(state, threshold4);
                    break;
            case(5):
                    threshold5 += 10;
                    showsthreshold(state, threshold5);
                    break;
            default:
                break;
            }
            buttonState=1;
        }

        if ((GPIO_getInputPinValue(SW1_PORT, SW1_PIN) == 0) && (buttonState == 1)) //Look for falling edge
        {
            buttonState = 0;
        }
        if ((GPIO_getInputPinValue(SW2_PORT, SW2_PIN) == 1) & (buttonState2 == 0)){
            state++;
            buttonState2=1;
        }
        if ((GPIO_getInputPinValue(SW2_PORT, SW2_PIN) == 0) & (buttonState2 == 1)){
            buttonState2=0;
        }

        /*write code for LCD display in setup mode*/
        if(state == 6){
            got_thresholds=1;
        }
    }
    GPIO_clearInterrupt(
            GPIO_PORT_P2,
            GPIO_PIN6);
    g=1;
    p=0;
}

void Greenon(void){
    //1.5
      GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN1|GPIO_PIN5|GPIO_PIN4);
      GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN3);
      GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN5);
      GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN5);

}
void Yellowon(void){
//1.4
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN1|GPIO_PIN5|GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN5);
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN4);
}

void Redon(void){
//5.3
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN1|GPIO_PIN5|GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN5);
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN3);
}
void Orangeon(void){
//2.5
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN1|GPIO_PIN5|GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN5);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN5);
}
void gen_audio2(){
    //5.2
    int i;
    int j;
    for (i=0;i<2;i++){
        for(j=0;j<100;j++){
            GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN2);
            __delay_cycles(1000);
            GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN2);
        }
    }
}
void gen_audio4(){
    int i;
    int j;
    for (i=0;i<4;i++){
        for(j=0;j<100;j++){
            GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN2);
            __delay_cycles(500);
            GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN2);
        }
    }
}
void Init_Timer(void){
    timer_param.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    timer_param.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    timer_param.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    timer_param.timerClear = TIMER_A_DO_CLEAR;
    timer_param.startTimer = false;

    Timer_A_initContinuousMode(TIMER_A1_BASE,&timer_param);

}
void Init_Sensor_Trigs(void)
{

    GPIO_setAsInputPin(
            GPIO_PORT_P1,
            GPIO_PIN6);

    GPIO_setAsInputPin(
            GPIO_PORT_P1,
            GPIO_PIN3);

    GPIO_enableInterrupt(
            GPIO_PORT_P2,
            GPIO_PIN6);
    GPIO_selectInterruptEdge(
            GPIO_PORT_P2,
            GPIO_PIN6,
            GPIO_LOW_TO_HIGH_TRANSITION);

    GPIO_clearInterrupt(
            GPIO_PORT_P1,
            GPIO_PIN_ALL8);
    GPIO_clearInterrupt(
            GPIO_PORT_P2,
            GPIO_PIN_ALL8);
}

void Init_GPIO(void)
{
    // Set all GPIO pins to output low to prevent floating input and reduce power consumption
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P7, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);

    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0|GPIO_PIN1|GPIO_PIN4|GPIO_PIN5|GPIO_PIN7|GPIO_PIN2);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P7, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);

    //Set LaunchPad switches as inputs - they are active low, meaning '1' until pressed

    GPIO_setAsInputPin(SW1_PORT,SW1_PIN);
    GPIO_setAsInputPin(SW2_PORT,SW1_PIN);
    GPIO_setAsInputPinWithPullUpResistor(SW1_PORT, SW1_PIN);
    GPIO_setAsInputPinWithPullUpResistor(SW2_PORT, SW2_PIN);

    /*Set LED1 and LED2 as outputs
    //GPIO_setAsOutputPin(LED1_PORT, LED1_PIN); //Comment if using UART
    GPIO_setAsOutputPin(LED2_PORT, LED2_PIN);*/
}

/* Clock System Initialization */
void Init_Clock(void)
{
    /*
     * The MSP430 has a number of different on-chip clocks. You can read about it in
     * the section of the Family User Guide regarding the Clock System ('cs.h' in the
     * driverlib).
     */

    /*
     * On the LaunchPad, there is a 32.768 kHz crystal oscillator used as a
     * Real Time Clock (RTC). It is a quartz crystal connected to a circuit that
     * resonates it. Since the frequency is a power of two, you can use the signal
     * to drive a counter, and you know that the bits represent binary fractions
     * of one second. You can then have the RTC module throw an interrupt based
     * on a 'real time'. E.g., you could have your system sleep until every
     * 100 ms when it wakes up and checks the status of a sensor. Or, you could
     * sample the ADC once per second.
     */
    //Set P4.1 and P4.2 as Primary Module Function Input, XT_LF
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN1 + GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);

    // Set external clock frequency to 32.768 KHz
    CS_setExternalClockSource(32768);
    // Set ACLK = XT1
    CS_initClockSignal(CS_ACLK, CS_XT1CLK_SELECT, CS_CLOCK_DIVIDER_1);
    // Initializes the XT1 crystal oscillator
    CS_turnOnXT1LF(CS_XT1_DRIVE_1);
    // Set SMCLK = DCO with frequency divider of 1
    CS_initClockSignal(CS_SMCLK, CS_DCOCLKDIV_SELECT, CS_CLOCK_DIVIDER_1);
    // Set MCLK = DCO with frequency divider of 1
    CS_initClockSignal(CS_MCLK, CS_DCOCLKDIV_SELECT, CS_CLOCK_DIVIDER_1);
}

/* UART Initialization */
void Init_UART(void)
{
    /* UART: It configures P1.0 and P1.1 to be connected internally to the
     * eSCSI module, which is a serial communications module, and places it
     * in UART mode. This let's you communicate with the PC via a software
     * COM port over the USB cable. You can use a console program, like PuTTY,
     * to type to your LaunchPad. The code in this sample just echos back
     * whatever character was received.
     */

    //Configure UART pins, which maps them to a COM port over the USB cable
    //Set P1.0 and P1.1 as Secondary Module Function Input.
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN1, GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN0, GPIO_PRIMARY_MODULE_FUNCTION);

    /*
     * UART Configuration Parameter. These are the configuration parameters to
     * make the eUSCI A UART module to operate with a 9600 baud rate. These
     * values were calculated using the online calculator that TI provides at:
     * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
     */

    //SMCLK = 1MHz, Baudrate = 9600
    //UCBRx = 6, UCBRFx = 8, UCBRSx = 17, UCOS16 = 1
    EUSCI_A_UART_initParam param = {0};
        param.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK;
        param.clockPrescalar    = 6;
        param.firstModReg       = 8;
        param.secondModReg      = 17;
        param.parity            = EUSCI_A_UART_NO_PARITY;
        param.msborLsbFirst     = EUSCI_A_UART_LSB_FIRST;
        param.numberofStopBits  = EUSCI_A_UART_ONE_STOP_BIT;
        param.uartMode          = EUSCI_A_UART_MODE;
        param.overSampling      = 1;

    if(STATUS_FAIL == EUSCI_A_UART_init(EUSCI_A0_BASE, &param))
    {
        return;
    }

    EUSCI_A_UART_enable(EUSCI_A0_BASE);

    EUSCI_A_UART_clearInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);

    // Enable EUSCI_A0 RX interrupt
    EUSCI_A_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
}

/* EUSCI A0 UART ISR - Echoes data back to PC host */
#pragma vector=USCI_A0_VECTOR
__interrupt
void EUSCIA0_ISR(void)
{
    uint8_t RxStatus = EUSCI_A_UART_getInterruptStatus(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG);

    EUSCI_A_UART_clearInterrupt(EUSCI_A0_BASE, RxStatus);

    if (RxStatus)
    {
        EUSCI_A_UART_transmitData(EUSCI_A0_BASE, EUSCI_A_UART_receiveData(EUSCI_A0_BASE));
    }
}

/* PWM Initialization */
void Init_PWM(void)
{
    /*
     * The internal timers (TIMER_A) can auto-generate a PWM signal without needing to
     * flip an output bit every cycle in software. The catch is that it limits which
     * pins you can use to output the signal, whereas manually flipping an output bit
     * means it can be on any GPIO. This function populates a data structure that tells
     * the API to use the timer as a hardware-generated PWM source.
     *
     */
    //Generate PWM - Timer runs in Up-Down mode
    param.clockSource           = TIMER_A_CLOCKSOURCE_SMCLK;
    param.clockSourceDivider    = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    param.timerPeriod           = TIMER_A_PERIOD; //Defined in main.h
    param.compareRegister       = TIMER_A_CAPTURECOMPARE_REGISTER_1;
    param.compareOutputMode     = TIMER_A_OUTPUTMODE_RESET_SET;
    param.dutyCycle             = HIGH_COUNT; //Defined in main.h

    //PWM_PORT PWM_PIN (defined in main.h) as PWM output
    GPIO_setAsPeripheralModuleFunctionOutputPin(PWM_PORT, PWM_PIN, GPIO_PRIMARY_MODULE_FUNCTION);
}

void Init_Timer_Trig(){
    timer_trig.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    timer_trig.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    timer_trig.timerPeriod = TIMER_A_PERIOD;
    timer_trig.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    timer_trig.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
    timer_trig.timerClear = TIMER_A_DO_CLEAR;
    timer_trig.startTimer = false;
    Timer_A_initUpDownMode(TIMER_A0_BASE, &timer_trig);

    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE,
        TIMER_A_CAPTURECOMPARE_REGISTER_0
        );


    initCompParam.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_0;
    initCompParam.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
    initCompParam.compareOutputMode = TIMER_A_OUTPUTMODE_OUTBITVALUE;
    initCompParam.compareValue = COMPARE_VALUE;
    Timer_A_initCompareMode(TIMER_A0_BASE, &initCompParam);

}

void start_timer(){
    Timer_A_startCounter( TIMER_A0_BASE,
                          TIMER_A_UPDOWN_MODE
                );
}
void Init_ADC(void)
{
    /*
     * To use the ADC, you need to tell a physical pin to be an analog input instead
     * of a GPIO, then you need to tell the ADC to use that analog input. Defined
     * these in main.h for A9 on P8.1.
     */

    //Set ADC_IN to input direction
    GPIO_setAsPeripheralModuleFunctionInputPin(ADC_IN_PORT, ADC_IN_PIN, GPIO_PRIMARY_MODULE_FUNCTION);

    //Initialize the ADC Module
    /*
     * Base Address for the ADC Module
     * Use internal ADC bit as sample/hold signal to start conversion
     * USE MODOSC 5MHZ Digital Oscillator as clock source
     * Use default clock divider of 1
     */
    ADC_init(ADC_BASE,
             ADC_SAMPLEHOLDSOURCE_SC,
             ADC_CLOCKSOURCE_ADCOSC,
             ADC_CLOCKDIVIDER_1);

    ADC_enable(ADC_BASE);

    /*
     * Base Address for the ADC Module
     * Sample/hold for 16 clock cycles
     * Do not enable Multiple Sampling
     */
    ADC_setupSamplingTimer(ADC_BASE,
                           ADC_CYCLEHOLD_16_CYCLES,
                           ADC_MULTIPLESAMPLESDISABLE);

    //Configure Memory Buffer
    /*
     * Base Address for the ADC Module
     * Use input ADC_IN_CHANNEL
     * Use positive reference of AVcc
     * Use negative reference of AVss
     */
    ADC_configureMemory(ADC_BASE,
                        ADC_IN_CHANNEL,
                        ADC_VREFPOS_AVCC,
                        ADC_VREFNEG_AVSS);

    ADC_clearInterrupt(ADC_BASE,
                       ADC_COMPLETED_INTERRUPT);

    //Enable Memory Buffer interrupt
    ADC_enableInterrupt(ADC_BASE,
                        ADC_COMPLETED_INTERRUPT);
}

//ADC interrupt service routine
#pragma vector=ADC_VECTOR
__interrupt
void ADC_ISR(void)
{
    uint8_t ADCStatus = ADC_getInterruptStatus(ADC_BASE, ADC_COMPLETED_INTERRUPT_FLAG);

    ADC_clearInterrupt(ADC_BASE, ADCStatus);

    if (ADCStatus)
    {
        ADCState = 0; //Not busy anymore
        ADCResult = ADC_getResults(ADC_BASE);
    }
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt
void TIMER_A0_ISR(void){
    //enum state logic with high to to that pin...
    if(sensor == SENS1){
       GPIO_setOutputHighOnPin(GPIO_PORT_P1,GPIO_PIN7);
       __delay_cycles(10);
       GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN7);
//        //__delay_cycles(5);
        sensor  = SENS2;
    }
    else{
        GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN7);
        __delay_cycles(10);
        GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN7);
         //__delay_cycles(5);
         sensor = SENS1;
    }

    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE,
        TIMER_A_CAPTURECOMPARE_REGISTER_0
        );
}
void got(){
    while(!g){
    displayScrollText("PRESS BUTTON");
    }
}
void main(void)


{
    //char buttonState = 0; //Current button press state (to allow edge detection)

    /*
     * Functions with two underscores in front are called compiler intrinsics.
     * They are documented in the compiler user guide, not the IDE or MCU guides.
     * They are a shortcut to insert some assembly code that is not really
     * expressible in plain C/C++. Google "MSP430 Optimizing C/C++ Compiler
     * v18.12.0.LTS" and search for the word "intrinsic" if you want to know
     * more.
     * */

    //Turn off interrupts during initialization
    __disable_interrupt();

    //Stop watchdog timer unless you plan on using it
    WDT_A_hold(WDT_A_BASE);

    // Initializations - see functions for more detail
    Init_GPIO();    //Sets all pins to output low as a default
    //Init_PWM();     //Sets up a PWM output
    Init_ADC();     //Sets up the ADC to sample
    Init_Clock();   //Sets up the necessary system clocks
    Init_UART();    //Sets up an echo over a COM port
    Init_LCD();     //Sets up the LaunchPad LCD display
    Init_Timer();
    Init_Sensor_Trigs();

     /*
     * The MSP430 MCUs have a variety of low power modes. They can be almost
     * completely off and turn back on only when an interrupt occurs. You can
     * look up the power modes in the Family User Guide under the Power Management
     * Module (PMM) section. You can see the available API calls in the DriverLib
     * user guide, or see "pmm.h" in the driverlib directory. Unless you
     * purposefully want to play with the power modes, just leave this command in.
     */
    //GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN6);
    PMM_unlockLPM5(); //Disable the GPIO power-on default high-impedance mode to activate previously configured port settings
    //All done initializations - turn interrupts back on.
    Init_Timer_Trig();
    //Init_data();
    displayScrollText("ECE 298");
    __enable_interrupt();

    //start_timer();
    got();
    time=0;
    time2=0;
    //GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN3);
    //GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN5);
    //GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN4);
    //GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN3);
    while(1){
		uint16_t t;
	if(sensor == SENS1){
	       GPIO_setOutputHighOnPin(GPIO_PORT_P1,GPIO_PIN7);
	       __delay_cycles(1);
	       GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN7);
	//        //__delay_cycles(5);
	    Timer_A_clear(TIMER_A1_BASE);
		while (GPIO_getInputPinValue(GPIO_PORT_P1,GPIO_PIN3) == 0){};

		Timer_A_startCounter(TIMER_A1_BASE,TIMER_A_CONTINUOUS_MODE);

		while(GPIO_getInputPinValue(GPIO_PORT_P1,GPIO_PIN3) == GPIO_INPUT_PIN_HIGH){
		}

		Timer_A_stop(TIMER_A1_BASE);

		t = Timer_A_getCounterValue(TIMER_A1_BASE);
		t = t / 58;
		time = .375*t+ .625*time;
        sensor  = SENS2;
        __delay_cycles(2000);
		//getdata(t,&time);
	}
	if(sensor == SENS2){
        GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN7);
        __delay_cycles(1);
        GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN7);
	    Timer_A_clear(TIMER_A1_BASE);
		while (GPIO_getInputPinValue(GPIO_PORT_P1,GPIO_PIN6) == 0){};

		Timer_A_startCounter(TIMER_A1_BASE,TIMER_A_CONTINUOUS_MODE);

		while(GPIO_getInputPinValue(GPIO_PORT_P1,GPIO_PIN6) == GPIO_INPUT_PIN_HIGH){
		}

		Timer_A_stop(TIMER_A1_BASE);

		t = Timer_A_getCounterValue(TIMER_A1_BASE);
		t = t / 58;
		//getdata(t,&time2);
		time2 = .375*t + .625*time2;
        sensor  = SENS1;
        __delay_cycles(2000);
	}
//
//
//	    //getdata(t,&time);
       show2bytes(time,time2);

       if(time <= threshold1){ //rear
            Redon();
        }
       else if((time<=threshold2) && (time>threshold1)){
           Orangeon();
        }
       else if((time<=threshold3) && (time>threshold2)){
            Yellowon();
        }
        else if(time>threshold3){
            Greenon();
        }

       if(time2 < threshold4)
       {
           gen_audio4();
       }
       else if(time2 >= threshold4 && time2 <= threshold5){
           gen_audio2();
       }
       else{
           GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN2);
       }

    /*
     * You can use the following code if you plan on only using interrupts
     * to handle all your system events since you don't need any infinite loop of code.
     *
     * //Enter LPM0 - interrupts only
     * __bis_SR_register(LPM0_bits);
     * //For debugger to let it know that you meant for there to be no more code
     * __no_operation();
    */
    }
}

