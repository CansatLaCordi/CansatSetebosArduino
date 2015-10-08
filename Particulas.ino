int measurePin = 1;
int ledPower = 5;
 
int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;
 
float voMeasured = 0;
float calcVoltage = 0;
// float dustDensity = 0;
 
void GPY101AU0F_Setup(){
  //Serial.begin(9600);
  pinMode(ledPower,OUTPUT);
}
 
long Get_Particulas(){  
  digitalWrite(ledPower,LOW); // power on the LED
  delayMicroseconds(samplingTime); 
  voMeasured = analogRead(measurePin); // read the dust value 
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH); // turn the LED off
  delayMicroseconds(sleepTime); 
  calcVoltage = voMeasured * (3.3 / 1024);
  //dustDensity = 0.17 * calcVoltage - 0.1;
  Serial.println("GetParticulas");
  return long((0.17 * calcVoltage - 0.1) * 1000.000); //Calculo para microgramos
}



