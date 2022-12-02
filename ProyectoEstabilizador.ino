#include <Wire.h>
#include <Servo.h>

 Servo servo1;
 Servo servo2;

//Direccion I2C de la IMU
#define MPU 0x68

//Ratios de conversion
#define A_R 16384.0
#define G_R 131.0

//Conversion de radianes a grados 180/PI
#define RAD_A_DEG = 57.295779

//MPU-6050 da los valores en enteros de 16 bits
//Valores sin refinar
int16_t AcX, AcY, AcZ, GyX, GyY, GyZ;
int val1;
int val2;
//Angulos
float Acc[2];
float Gy[2];
float Angle[2];


void setup()
{
Wire.begin();
Wire.beginTransmission(MPU);
Wire.write(0x6B);
Wire.write(0);
Wire.endTransmission(true);
servo1.attach(5);
servo2.attach(6);
Serial.begin(38400);

}

void loop()
{
   //valores del Acelerometro de la IMU
   Wire.beginTransmission(MPU);
   Wire.write(0x3B); //Pedir el registro 0x3B - corresponde al AcX
   Wire.endTransmission(false);
   Wire.requestFrom(MPU,6,true); //A partir del 0x3B, se piden 6 registros
   AcX=Wire.read()<<8|Wire.read(); //Cada valor ocupa 2 registros
   AcY=Wire.read()<<8|Wire.read();
   AcZ=Wire.read()<<8|Wire.read();

    //angulos Y, X respectivamente.
   Acc[1] = atan(-1*(AcX/A_R)/sqrt(pow((AcY/A_R),2) + pow((AcZ/A_R),2)))*RAD_TO_DEG;
   Acc[0] = atan((AcY/A_R)/sqrt(pow((AcX/A_R),2) + pow((AcZ/A_R),2)))*RAD_TO_DEG;
// proximamente mas proyectos en  http://elxedelectronics.blogspot.mx

   //valores del Giroscopio
   Wire.beginTransmission(MPU);
   Wire.write(0x43);
   Wire.endTransmission(false);
   Wire.requestFrom(MPU,4,true); //A diferencia del Acelerometro, solo se piden 4 registros
   GyX=Wire.read()<<8|Wire.read();
   GyY=Wire.read()<<8|Wire.read();

   //Calculo del angulo del Giroscopio
   Gy[0] = GyX/G_R;
   Gy[1] = GyY/G_R;

   //Filtro Complementario
   Angle[0] = 0.98 *(Angle[0]+Gy[0]*0.010) + 0.02*Acc[0];
   Angle[1] = 0.98 *(Angle[1]+Gy[1]*0.010) + 0.02*Acc[1];

   //Mostrar valores
 
 
  int Uangle0 = Angle[0] * 100;
  int Uangle1 = Angle[1] * 100;

   Serial.print("Angle X: "); Serial.print(Uangle0); Serial.print("\n");
   Serial.print("Angle Y: "); Serial.print(Uangle1); Serial.print("\n------------\n");


    ////// establecer datos de angulos y adaptarlos a los servos
    //// podrían variar según los servos utilizados

   float val1 = Uangle0;
   val1 = map(val1, -3500, 4500, 48, 132); ///30, -30, 12, 220
   servo1.write(val1);

   float val2 = Uangle1;
   val2= map(val2, -4000, 4000, 48, 132); ///-25, 32, 12, 220
   servo2.write(val2);
 
delay(10);
 
}
