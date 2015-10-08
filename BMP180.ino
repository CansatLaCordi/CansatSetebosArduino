/*
Hardware connections:

- (GND) to GND
+ (VDD) to 3.3V

(WARNING: do not connect + to 5V or the sensor will be damaged!)

You will also need to connect the I2C pins (SCL and SDA) to your
Arduino. The pins are different on different Arduinos:

Any Arduino pins labeled:  SDA  SCL
Uno, Redboard, Pro:        A4   A5
Mega2560, Due:             20   21
Leonardo:                   2    3

*/

// You will need to create an SFE_BMP180 object, here called "pressure":

SFE_BMP180 pressure;

#define ALTITUDE 1655.0 // Altitude of SparkFun's HQ in Boulder, CO. in meters

char status;
double T,P,p0,a;
  
void BMP_setup()
{
// Initialize the sensor (it is important to get calibration values stored on the device).

  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.
    Serial.println("BMP180 init fail\n\n");
  }
}

long Get_BMP10_Altitud()
{
  return long(ALTITUDE);
}

long Get_BMP10_Temperatura()
{
  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(T);
    if (status != 0)
    {
      // Print out the measurement:
      return long(T); //°C
    }
    else 
      Serial.println("Error al leer temperatura BMP180\n");
  }
}

long Get_BMP10_PrecionAbsoluta()
{
  status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);
        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          // Print out the measurement:
          return long(P);
          //Serial.print(p0,2)//mb
          //Serial.print(p0*0.0295333727,2); //inHg
        }
      }
}

long Get_BMP10_PrecionRelativa()
{
  status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);
        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          p0 = pressure.sealevel(P,ALTITUDE); // we're at 1655 meters (Boulder, CO)
          return long(p0); //mb
        }
      }
}

long Get_BMP10_AlturaCalculada()
{
  status = pressure.getPressure(P,T);
  if (status != 0)
  {
    p0 = pressure.sealevel(P,ALTITUDE); // we're at 1655 meters (Boulder, CO)
    a = pressure.altitude(P,p0);
    return long(a);//m
  }
  else 
    Serial.println("error retrieving pressure measurement\n");
}
