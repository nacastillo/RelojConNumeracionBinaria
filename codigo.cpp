// CONSTANTES
#define DR digitalRead
#define DW digitalWrite
#define MAX_H 23
#define MAX_M 59
#define MAX_S 59
#define TMP_CLK 1
#define TMP_SEG 930
#define TAM 8

// PINES
#define PUL_1 3  // Interrupt para modo config
#define PUL_2 4  // Pin para sumar 1 al campo
#define LED_M 5  // Salida Verde led RGB
#define LED_S 6  // Salida Azul led RGB
#define LED_H 7  // Salida Rojo led RGB
#define SER_D 8  // Serial Data
#define LAT_C 9  // Latch Clock
#define SHI_C 10 // Shift Clock

enum Campo {
    CS,
    CM,
    CH,
    MH
};

volatile uint8_t estado;
volatile uint8_t segs;
volatile uint8_t mins;
volatile uint8_t horas;

void setup(){
  	Serial.begin(9600);  
  	pinMode(PUL_1, INPUT);
  	pinMode(PUL_2, INPUT);
  	pinMode(LED_H, OUTPUT);
    pinMode(LED_M, OUTPUT);
  	pinMode(LED_S, OUTPUT);  
  	pinMode(SER_D, OUTPUT);
    pinMode(LAT_C, OUTPUT);
    pinMode(SHI_C, OUTPUT);    
    attachInterrupt(digitalPinToInterrupt(PUL_1), config, RISING);
    DW(LED_H, LOW);
    DW(LED_M, LOW);
  	DW(LED_S, LOW);
    DW(LAT_C, LOW);    
    DW(SHI_C, LOW);    
    estado = Campo::MH;
  	segs = 0;
 	mins = 0;
  	horas = 0;    	
}

void loop () {        
    actualizarLEDS();
    imprimirHora();
    segs++;
    contar();    
    milisegundos(TMP_SEG);           
}

void config () {
    estado = (estado + 1) % 4;
    milisegundos(100);
    if (estado != Campo :: MH) {
      actualizarLEDS();      
      setupLedConf(estado);
          while (!DR(PUL_1)) {                
              if(DR(PUL_2)) {
                  milisegundos(100);
                  confSum(estado);
              }
          }            
          setupLedConf(3);  
     }  
}

void pulso (uint8_t pin) {
    milisegundos(TMP_CLK);
    DW(pin, HIGH);
    milisegundos(TMP_CLK);
    DW(pin, LOW);  
}

void enviar (uint8_t info) {
    for (int8_t i = TAM - 1; i >= 0; i--) {  
        DW(SER_D, (info >> i) & 0x01);
        pulso(SHI_C);
    }
}

void contar () {
    if (segs > MAX_S) {
        segs = 0;
        mins++;
    }
    if (mins > MAX_M) {
	    mins = 0;
	    horas++;
    }
    if (horas > MAX_H) {
  	    horas = 0;    
    }
}

void imprimirHora () {	
    Serial.print(horas);
    Serial.print(":");
    Serial.print(mins);
    Serial.print(":");
    Serial.println(segs);  
}

void actualizarLEDS () {
    enviar(horas);
    enviar(mins);
    enviar(segs);
    pulso(LAT_C);
}

void confSum (uint8_t i) {
  	switch (i) {
      case 0:
        segs++;      	    
        break;
      case 1:
        mins++;
        break;
      case 2:
        horas++;          
        break; 
    }
    contar();
    actualizarLEDS();    
  }

void setupLedConf (uint8_t i) {
	switch (i) {
      case 0: // Segundos
        DW(LED_H, LOW);
      	DW(LED_M, LOW);
        DW(LED_S, HIGH);                
        break;
      case 1: // Minutos
        DW(LED_H, LOW);
      	DW(LED_M, HIGH);
        DW(LED_S, LOW);
        break;
      case 2: // Horas
        DW(LED_H, HIGH);
        DW(LED_M, LOW);
        DW(LED_S, LOW);        
        break;              
      case 3: // Apagar
        DW(LED_H, LOW);
      	DW(LED_M, LOW);
        DW(LED_S, LOW);
        break;  
      }
}

void milisegundos (uint16_t n) {
    for (uint16_t i = 0; i < n; i++) {
  	    delayMicroseconds(1000);
    }
}