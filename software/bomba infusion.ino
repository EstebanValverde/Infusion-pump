#include <LiquidCrystal.h>

/* MAPA DE PINES DEL ARDUINO UNO

D1:
D2: LCD DB7
D3: LCD DB6
D4: LCD DB5
D5: LCD DB4
D6: 
D7:
D8:
D9: SALIDA PWM DEL MOTOR
D10:
D11: LCD EN
D12: LCD RS
D13:

A0: 
A1: 
A2:
A3:
A4:
A5: POTENCIOMETRO ENTRADA FLUJO

*/

/* 
Bomba GOSO-AB11 12V-7.5W
- Voltios / Corriente: DC 12V / 24V
- Condiciones de trabajo: 0-40 grados Celsius
- Humedad relativa < 80%
- Caudal: 0-100 ml/min
- Rotación de velocidad: 0.1-100 rpm
- Material: Metal
- Tamaño: 65 * 55 * 80mm
 */

// medi 500mL en 10min, entonces
#define FLUJOMAXIMO (500.0/10.0)

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
// http://www.arduino.cc/en/Tutorial/LiquidCrystalDisplay
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int potenPin = A5; // leo el potenciometro de velocidad
const int motorPin = 9;  // ajusto el PWM de salida al motor
const int buzzerPin = A1;

void setup()
{
    // inicializo pines analogico de entrada y PWM de salida
    pinMode(potenPin, INPUT);
    pinMode(motorPin, OUTPUT);
    analogWrite(motorPin, 0);
	
	// inicializo buzzer de alarma apagado
	pinMode(buzzerPin, OUTPUT);
	digitalWrite(buzzerPin, LOW); 
  
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);

    // Print a message to the LCD.
    lcd.clear();
    lcd.print("    BOMBA DE    ");
    lcd.setCursor(0, 1);
    lcd.print("    INFUSION    ");
    delay(1000);
    lcd.clear();
}

unsigned char leerVelocidad(void)
{
  unsigned char velocidad;
  
  velocidad = analogRead(potenPin)/4;  
  return velocidad;
}

unsigned char ajustarFlujo(unsigned char velocidad)
{
  unsigned char flujo;
  
  analogWrite(motorPin, velocidad);
  
  // ecuacion para convertir a flujo
  flujo = (float)velocidad * FLUJOMAXIMO / 255.0;
  return flujo;
}

void loop()
{
  static char linea1[16];
  static char linea2[16];
  static char strflujototal[8];
  static char llenado = 255;
  static unsigned char i = 0;
  static unsigned char flujo = 0; // valor expresado en ml/min
  static unsigned int flujototal = 0; // expresado en ml
  static unsigned char velocidad = 0, oldvelocidad = 0; // valor entre 0 - 255
  static unsigned char flujomaximo = 200; // flujo maximo, hay que ingresarlo x panel

  //---------------------------------------------
  // leo velocidad y calculo el flujo
  velocidad = leerVelocidad();
  if(velocidad != oldvelocidad)
  {
    flujo = ajustarFlujo(velocidad);
    oldvelocidad = velocidad;
  }
	
	//---------------------------------------------
	// informacion al LCD
  flujototal += flujo;
  dtostrf((float)flujototal/60.0, 7, 1, strflujototal);
  sprintf(linea1, "TOTAL %7s ml", strflujototal);
  lcd.setCursor(0, 0);
  lcd.print(linea1);

  sprintf(linea2, "       %2d ml/min", flujo);
  lcd.setCursor(0, 1);
  lcd.print(linea2);

  // imprimo el cursor rotatorio de llenado
  lcd.setCursor(i, 1);
  lcd.print(llenado);
  
  //---------------------------------------------
  // cada 5 segundos, i retorna a 0
  i = (i+1) % 5;
  delay(1000);
}
