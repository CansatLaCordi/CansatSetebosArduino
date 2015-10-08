#include <SoftwareSerial.h>
#include <TinyGPS.h>
#define xbee Serial//cambiar por el adecuado
TinyGPS gps;
SoftwareSerial ss(4, 3);

struct Measure{
  String measureDate;
  float externalTemperature,internalTemperature,pressure, latitude, longitude, speed,barometricAltitude, altitude, humidity, pm10, voltage;
  bool ejected;
};
unsigned long actualTime,lastMeasure, measureRate=1000;

String MeasureString(struct Measure m){
  return "mdate="+String(m.measureDate)+
    "&extTemp="+String(m.externalTemperature)  +
    "&intTemp="+String(m.internalTemperature)  +
    "&press="+String(m.pressure)  +
    "&lat="+String(m.latitude,7)  +
    "&lon="+String(m.longitude,7)  +
    "&speed="+String(m.speed)  +
    "&barAlt="+String(m.barometricAltitude)  +
    "&alt="+String(m.altitude)  +
    "&hum="+String(m.humidity)  +
    "&pm10="+String(m.pm10)  +
    "&volt="+String(m.voltage) ;
}

void getGPS(struct Measure &m){
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
  
  if(fix_age == TinyGPS::GPS_INVALID_AGE){
    xbee.println("No gps");
    return;  
  } 
  m.measureDate = String(day)+"-"+String(month)+"-"+String(year)+" "+String(hour)+":"+minute+":"+second+"."+hundredths;
  m.latitude = flat;
  m.longitude = flon;
  m.altitude = falt;
  m.speed = fkmph;
}

void getMeasure(struct Measure &m){
    //llenar mediciones aqui, usar las librerias de cada sensor aqui
    getGPS(m);
}


void setup() {
  xbee.begin(9600);
  actualTime = lastMeasure = millis();
  //Serial.begin(9600);
  ss.begin(9600);  
  Serial.println("Sistema iniciado");
}

void loop() {
  xbee.println(":v");
  actualTime = millis();
  bool newData = false;
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (ss.available())
    {
      char c = ss.read();
      //Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }
  if(actualTime   - lastMeasure > measureRate){ //ejecutamos la lectura solo el tiempo determinado
      xbee.println("--");
      lastMeasure = actualTime;
      struct Measure m;
      getMeasure(m); // llenar la medición
      String strMeasure = MeasureString(m); //preparar la cadena de la medición
      xbee.println(strMeasure); //Enviar la medicion al xbee via serial
  }  
}
