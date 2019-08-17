/* 
 * File:   ADC.cpp
 * Author: Gunnar Turnquist
 *
 * Created on May 21, 2019, 10:27 AM
 */

#include <xc.h>

void init_adc(){
    //AN49 - SX
    //AN5 - SY
    
    //copy factory calibration into registers
    ADC0CFG = DEVADC0;
    ADC1CFG = DEVADC1;
    ADC2CFG = DEVADC2;
    ADC3CFG = DEVADC3;
    ADC4CFG = DEVADC4;
    ADC7CFG = DEVADC7;
    
    /* Configure ADCCON1 */
    ADCCON1 = 0; // No ADCCON1 features are enabled including: Stop-in-Idle, turbo,
    ADCCON1bits.SELRES = 3; // ADC7 resolution is 12 bits
    ADCCON1bits.STRGSRC = 1; // Scan trigger is GSWTRG
    ADCCON1bits.FSSCLKEN = 1;   //SYSCLK

    /* Configure ADCCON2 */
    ADCCON2 = 0;
    ADCCON2bits.SAMC = 5; // ADC7 sampling time = 5 * TAD7
    ADCCON2bits.ADCDIV = 1; // ADC7 clock freq is half of control clock = TAD7
    
    /* Initialize warm up time register */
    ADCANCON = 0;
    ADCANCONbits.WKUPCLKCNT = 5; // Wakeup exponent = 32 * TADx
    
    /* Clock setting */
    ADCCON3bits.ADCSEL = 0b01; // Select input clock source -SYSCLK
    ADCCON3bits.CONCLKDIV = 0b100;   // Control clock frequency is /6   
    ADCCON3bits.VREFSEL = 0; // Select AVDD and AVSS as reference source
    
    //set timing for ADC3 and 4
    ADC3TIMEbits.SELRES = 3;    //12bit resolution
    ADC3TIMEbits.ADCDIV = 1;    // ADC CLK is /2 Control CLK
    ADC3TIMEbits.SAMC = 5;
    
    ADC4TIMEbits.SELRES = 3;
    ADC4TIMEbits.ADCDIV = 1;
    ADC4TIMEbits.SAMC = 5;
    
    /* Select analog input for ADC modules, no presync trigger, not sync sampling */
    ADCTRGMODE = 0; //no presync triggers
    ADCTRGMODEbits.SH4ALT = 1;   //AN49
    ADCTRGMODEbits.SH3ALT = 1;   //AN48
    
    /* Select ADC input mode */
    ADCIMCON1 = 0;
    ADCIMCON2 = 0;
    ADCIMCON3 = 0;    
    ADCIMCON1bits.SIGN3 = 0;    //ADC3 will be AN48
    ADCIMCON1bits.DIFF3 = 0;    //unsigned
    ADCIMCON1bits.SIGN4 = 0;    //ADC4 will be AN49
    ADCIMCON1bits.DIFF4 = 0;    //unsigned
    ADCIMCON1bits.SIGN5 = 0;    //unsigned data format
    ADCIMCON1bits.DIFF5 = 0;    //Single ended mode
    
    /* Configure ADCGIRQENx */
    ADCGIRQEN1 = 0;     //No interrupts are used.
    ADCGIRQEN2 = 0;
    
    /* Configure ADCCSSx */
    ADCCSS1 = 0x0000; // Clear all bits
    ADCCSS2 = 0x0000;
    //set which pins are in the scan
    //ADCCSS1bits.CSS3 = 1;   //AN48 
    ADCCSS1bits.CSS4 = 1;   //AN49
    ADCCSS1bits.CSS5 = 1;   //AN5 
    
    /* Configure ADCCMPCONx */
    ADCCMPCON1 = 0; // No digital comparators are used. Setting the ADCCMPCONx
    ADCCMPCON2 = 0; // register to '0' ensures that the comparator is disabled.
    ADCCMPCON3 = 0; // Other registers are ?don't care?.
    ADCCMPCON4 = 0;
    ADCCMPCON5 = 0;
    ADCCMPCON6 = 0;
    
    /* Configure ADCFLTRx */
    ADCFLTR1 = 0; // No oversampling filters are used.
    ADCFLTR2 = 0;
    ADCFLTR3 = 0;
    ADCFLTR4 = 0;
    ADCFLTR5 = 0;
    ADCFLTR6 = 0;
    
    /* Set up the trigger sources */
    ADCTRGSNSbits.LVL3 = 0;     //edge trigger
    ADCTRGSNSbits.LVL4 = 0;     //edge trigger
    ADCTRGSNSbits.LVL5 = 0;
    ADCTRG1bits.TRGSRC3 = 3;    //AN48 trigger from GSWTRG
    ADCTRG2bits.TRGSRC4 = 3;    //AN49 trigger from GSWTRG
    ADCTRG2bits.TRGSRC5 = 3;    //AN5 trigger from GSWTRG
    
    /* Early interrupt */
    ADCEIEN1 = 0; // No early interrupt
    ADCEIEN2 = 0;
    
    /* Turn the ADC on */
    ADCCON1bits.ON = 1;
    
    /* Wait for voltage reference to be stable */
    while(!ADCCON2bits.BGVRRDY); // Wait until the reference voltage is ready
    while(ADCCON2bits.REFFLT); // Wait if there is a fault with the reference voltage
    
    /* Enable clock to analog circuit */
    //ADCANCONbits.ANEN3 = 1; // Enable ADC3
    ADCANCONbits.ANEN4 = 1; // Enable ADC4
    ADCANCONbits.ANEN7 = 1; // Enable ADC7
    
    /* Wait for ADC to be ready */
    //while(!ADCANCONbits.WKRDY3); // Wait until ADC3 is ready
    while(!ADCANCONbits.WKRDY4); // Wait until ADC4 is ready
    while(!ADCANCONbits.WKRDY7); // Wait until ADC7 is ready
    
    /* Enable the ADC module */ 
    //ADCCON3bits.DIGEN3 = 1; // Enable ADC3
    ADCCON3bits.DIGEN4 = 1; // Enable ADC4
    ADCCON3bits.DIGEN7 = 1; // Enable ADC7
    
    ADCCON3bits.GSWTRG = 1;
}

int * read_adc() {

    static int result[2];
    
    /* Trigger a conversion */
    ADCCON3bits.GSWTRG = 1;

    while (ADCDSTAT1bits.ARDY4 == 0);
    /* fetch the result */
    result[0] = ADCDATA4;     //SX
    
    while (ADCDSTAT1bits.ARDY5 == 0);
    /* fetch the result */
    result[1] = ADCDATA5;    //SY

    return result;
}