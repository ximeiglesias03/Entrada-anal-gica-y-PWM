#include <xc.h>
#define _XTAL_FREQ 4000000  // Frecuencia del oscilador (4 MHz)

// ========== Configuración ADC ========== //
void ADC_Init() {
    ANSEL  = 0b00000001;    // AN0 como entrada analógica, resto digital
    ANSELH = 0x00;          // Pines AN8-AN13 como digitales
    ADCON0 = 0b01000001;    // ADC ON, Fosc/8, canal AN0
    ADCON1 = 0b10000000;    // Justificación derecha, VDD y VSS como referencias
}

unsigned int ADC_Read(unsigned char channel) {
    ADCON0 &= 0b11000101;          // Limpiar bits de selección de canal (CHS<3:0>)
    ADCON0 |= (channel << 3);       // Seleccionar canal (AN0 = 0, AN1 = 1, etc.)
    __delay_us(5);                  // Tiempo de estabilización
    GO_DONE = 1;                    // Iniciar conversión
    while(GO_DONE);                 // Esperar a que termine
    return ((ADRESH << 8) | ADRESL); // Retornar valor de 10 bits
}

// ========== Configuración PWM ========== //
void PWM_Init() {
    TRISC2 = 0;             // CCP1 (RC2) como salida PWM
    PR2 = 255;              // Periodo para 10 bits de resolución (ajustable)
    T2CON = 0b00000100;     // Timer2 ON, prescaler 1:1 (T2CKPS = 00)
    CCP1CON = 0b00001100;   // Modo PWM (CCP1M = 1100)
}

void PWM_Set_Duty(unsigned int duty) {
    if (duty <= 1023) {     // Asegurar que el ciclo de trabajo sea válido (10 bits)
        CCPR1L = duty >> 2;         // 8 MSB a CCPR1L
        CCP1CON = (CCP1CON & 0xCF) | ((duty & 0x03) << 4); // 2 LSB a CCP1CON<5:4>
    }
}

// ========== Programa principal ========== //
void main() {
    ADC_Init();     // Inicializar ADC
    PWM_Init();     // Inicializar PWM
    TRISD = 0x00;   // Puerto D como salida (opcional, para visualización)

    while(1) {
        unsigned int adc_value = ADC_Read(0);    // Leer AN0 (0-1023)
        PWM_Set_Duty(adc_value);                // Ajustar PWM según el ADC
        
        // Opcional: Mostrar el valor del ADC en PORTD (solo 8 bits MSB)
        PORTD = adc_value >> 2;  // Escalar de 10 bits a 8 bits
        __delay_ms(10);          // Pequeño retardo para estabilidad
    }
}
