#include <SFE_BMP180.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <SFE_BMP180.h>
#include <Wire.h>
#include <math.h>
#include <Servo.h>

/*Pines usados Arduino
 * SDA A4  -> SDA HIH -> SDA BMP180
 * SCL A5  -> SCL HIH -> SCL BMP180
 * 
 * Tx D8 -> Xbee Rx
 * Rx D9 -> Xbee Tx
 * 
 * Tx D3 -> GPS Rx
 * Rx D4 -> GPS Tx
 * 
 * D5 -> Led Particulas
 * A1 -> Lectura particulas
 * 
 * PWM D10 -> Servo
 */


#define xbee ss1//cambiar por el adecuado
#define servo_pin 10
TinyGPS gps;
SoftwareSerial ss(4, 3), ss1(9, 8);
Servo myservo;
char c ;
bool ejected;
struct Measure {
  String measureDate;
  float extT, intT, presb, lat, lng, vel, barAlt, alti, hum, pm10, volt;
  bool ejected;
};
unsigned long actualTime, lastMeasure, measureRate = 1000;

void getGPS(struct Measure &m) {
  float flat, flon;
  unsigned long age;
  float falt = gps.f_altitude();
  float fkmph = gps.f_speed_mps();
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long fix_age;
  gps.crack_datetime(&year, &month, &day,
                     &hour, &minute, &second, &hundredths, &fix_age);
  gps.f_get_position(&flat, &flon, &age);

  if (fix_age != TinyGPS::GPS_INVALID_AGE) {
    m.measureDate = String(day) + "-" + String(month) + "-" + String(year) + " " + String(hour) + ":" + minute + ":" + second + "." + hundredths;
    m.lat = flat;
    m.lng = flon;
    m.alti = falt;
    m.vel = fkmph;
  }
}

void checkEjection(struct Measure &m){
  if(!ejected && min(m.alti,m.barAlt) >= 4000 ){
      ejected = 1;
       myservo.write(180);
  }
  myservo.write(0);
  m.ejected = ejected;
}

void getMeasure(struct Measure &m) {
  //llenar mediciones aqui, usar las librerias de cada sensor aqui
  getGPS(m);
  m.alti = 0;
  Serial.println("BMP");
  m.extT = Get_BMP10_Temperatura();
  m.presb = Get_BMP10_PrecionAbsoluta();
  m.barAlt = Get_BMP10_Altitud();
  Serial.println("HIH");
  m.intT = Get_Tem_HIH6130(); 
  m.hum = Get_Hum_HIH6130();
  Serial.println("Particulas");
  m.pm10 = Get_Particulas();
  checkEjection(m);
}

void sendMeasure(struct Measure m) {
  xbee.print("mdate="); xbee.print(m.measureDate);
  xbee.print("&extTemp="); xbee.print(m.extT);
  xbee.print("&intTemp="); xbee.print(m.intT) ;
  xbee.print("&pres=");  xbee.print(m.presb,3);
  xbee.print("&lat=");  xbee.print(m.lat, 7);
  xbee.print("&lon=");  xbee.print(m.lng, 7);
  xbee.print("&vel=");  xbee.print(m.vel,3);
  xbee.print("&barAlt=");  xbee.print(m.barAlt);
  xbee.print("&alt=");  xbee.print(m.alti,2);
  xbee.print("&hum=");  xbee.print(m.hum);
  xbee.print("&pm10=");  xbee.print(m.pm10);
  xbee.print("&eje=");  xbee.print(m.ejected);
  xbee.print("&volt=");  xbee.println(m.volt);
}

void setup() {
  xbee.begin(9600);
  actualTime = lastMeasure = millis();
  Serial.begin(9600);
  ss.begin(9600);
  Serial.println("Sistema iniciado");
  xbee.println("Sistema iniciado");
  BMP_setup();
  HIH6130_setup();
  GPY101AU0F_Setup();
  myservo.attach(servo_pin);
}

void loop() {
  Serial.println(":v");
  xbee.println(":v");
  actualTime = millis();
  bool newData = false;
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (ss.available())
    {
      c = ss.read();
      Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      //xbee.write(c);
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }
  Serial.println("--");
  xbee.println("--");
  struct Measure m;
  getMeasure(m); // llenar la medición
  sendMeasure(m);
  
  /*if (actualTime   - lastMeasure > measureRate) { //ejecutamos la lectura solo el tiempo determinado
    lastMeasure = actualTime;

  }*/
}
