/* SDS011 Dust Handheld 
Kufferath , NOV 2020  Germany
RX/TX PIN  Softserial for Nextion Display 2/3  und SDS011 6/7
Display on  Nextion , Wave und Mean/MIN/MAX
*/
#include "SdsDustSensor.h"  // SDS 011  LIB
#include <SoftwareSerial.h>
#include <Nextion.h>   //nextion-master Lib
#define DHTPIN 13    // PIN zum Arduino
#define DHTTYPE    DHT22     // DHT 22 (AM2302)

uint32_t delayMS; 
int rxPin = 6;
int txPin = 7;
int mean_2=0;
int xx=1;
int PM10=0;
int PM25=0;
float PM10f=0;
int PM10i=0;
float PM25f=0;
int PM25i=0;
int warte_in_sec =3;
int loopcount=0;
SoftwareSerial nextion(2, 3);// Nextion TX to pin 2 and RX to pin 3 of Arduino

Nextion myNextion(nextion, 9600); //create a Nextion object named myNextion using the nextion serial port @ 9600bps
  int x=0;
  int y=0;
  int PMx[10];
  int z=0;
  float mean=0;
  int mini=1000;
  int max_v = 0;
  int max_i = 0;
  bool state=0;

SdsDustSensor sds(rxPin, txPin);
 
void setup() {
  Serial.begin(9600);
 
  myNextion.init();
  sds.begin();
  Serial.println(sds.queryFirmwareVersion().toString()); // Firmware version
  Serial.println(sds.setActiveReportingMode().toString()); // Sensor  mode
  Serial.println(sds.setCustomWorkingPeriod(0).toString()); // Sensor sendet alle (x) Minuten einen Wert. Bei (0) kontinuierlich
 // Serial.println(sds.setCustomWorkingPeriod(10).toString()); //  HIer Sensor sendet alle (10) Minuten 
}
void loop() {

  PmResult pm = sds.readPm();  // at first read sds Dust values
  if (xx>warte_in_sec){
      xx=0;
      myNextion.setComponentText("t19", String (loopcount));
  if (pm.isOk()) {            // Wenn Daten anliegen
    Serial.print(", PM10 = ");
    Serial.println(pm.pm10); // hier wird PM10 µ  ausgegeben

  myNextion.setComponentText("t1", String(pm.pm10));  // PM10µ Konzentration ausgeben
  myNextion.setComponentText("t2", String(pm.pm25));  // PM2,5µ Konzentration ausgeben

     myNextion.setComponentText("t5", "20");  //Temperature   // Best mesuremet conditions
     myNextion.setComponentText("t7", "50");  //Humidity
//*********************PM10 calculate **************************
     PM10=pm.pm10;
     PM10f=pm.pm10;
     PM10f=PM10f*4;   // Display hat die Maße 240x240 obere Grenze soll 60µg sein deshalb 240/60=4 Faktor
     PM10i=PM10f; // Umwandeln in Integer;
     if (PM10i >240)
            {PM10i=236;
            myNextion.setComponentText("t3","Over");
            state =1;  }
      if ((PM10i <235)&& state ==1){
                       myNextion.setComponentText("t3", "60ug");
                        state =0;} 
  //*********************PM25 calculate **************************
  
     PM25f=pm.pm25;
     PM25f=PM25f*4;   // Display hat die Maße 240x240 obere Grenze soll 60µg sein deshalb 240/60=4 Faktor
     PM25i=PM25f; // Umwandeln in Integer; 
       if (PM25i >240)
            PM25i=233;                     
                            
  //*************   wave plot  *************************

  nextion.print("add 1,0,");
  nextion.print(PM10i);  //PM10 Value multiply by 4 for better display the Wave
  nextion.write(0xff);
  nextion.write(0xff);
  nextion.write(0xff);
    delay (30);

    nextion.print("add 1,1,");
  nextion.print(PM25i);  //PM10 Value multiply by 4 for better display the Wave
  nextion.write(0xff);
  nextion.write(0xff);
  nextion.write(0xff);
    delay (30);  
    
  PMx[y]=PM10;
  y++;
          } else {  }

    x++; // Counter 
    if (x==20)x=0;
    
 //---------------calculate MEAN -----------------------
if (y==10){
  y=0;
  for (z=0;z<10;z++)
        {mean+= PMx[z];
          }
          mean=mean/10;
   myNextion.setComponentText("t14", String (mean));
   mean=0;

//----------------- -calculate max --------
    for ( z = 0; z < 10; z++ )
        {
          if ( PMx[z] > max_v )
              { max_v = PMx[z];
                max_i = z;  }
        }
  myNextion.setComponentText("t16", String (max_v));
  max_v=0;

  //-------------- -calculate  min ----------------

   for ( z = 0; z < 10; z++ )
        {
          if ( PMx[z] < mini )
              mini = PMx[z];  }
              
          myNextion.setComponentText("t18", String (mini));
          mini=1000;
          } 
  loopcount++;
if (y==10) y=0;}
xx++;

    delay(1000);
}
