/* 
 *  Stephanie Wyckoff
 *  CSc 4110 - Embedded Systems
 *  Final Project - Running Clothes Barometer
 *  Due: 24 Apr 2018
 *  
 *  Description: This project sources real time weather data from OpenWeatherMap.org via an API key, is translated into 
 *    a useable format via the JSON parser, and that information is relayed to the user via lit up LEDs and LCD display.
 *    The bulk of this code came from the  ESP32 Weather Station Project v1.00, which can be found at 
 *    http://educ8s.tv/esp32-weather-station. I hava annotated which parts were wholy my own creation, which parts were 
 *    modified from the oritginal code and which parts are wholy the original code.
 *    
 *  Input: OpenWeatherMap.org weather information
 *  
 *  Output: Light up correct LED which is associated with the current weather (rain, wind, sun/clear, clouds) and also 
 *    update the LCD display from its default "Wifi Connecting..." to the coresponding image of running clothes or 
 *    incliment weather condition. 
 *  
*/
    
/*BEGIN ALL CODE*/

/*BEGIN CODE MODIFIED BY ME*/

#define ARDUINOJSON_ENABLE_PROGMEM 0
#include <ArduinoJson.h>    //https://github.com/bblanchon/ArduinoJson
#include <WiFi.h>

/*YOU WILL NOTICE THAT THE SSID, PASSWORD AND APIKEY HAVE BEEN REDACTED FOR SECURITY PURPOSES*/
const char* ssid     = "****INFORMATION REDACTED****";
const char* password = "****INFORMATION REDACTED****";
String CityID = "4225309"; //suwanee, Ga, USA
String APIKEY = "****INFORMATION REDACTED****";

/*END CODE MODIFIED BY ME*/


/*BEGIN CODE BY ME*/

#define red 14
#define white 27
#define yellow 26
#define green 25
#define pink 33
#define blue 32


float temperature = 0;
int weatherID = 0;

/*END CODE BY ME*/

/*BEGIN CODE BY EDUC8S.TV*/
WiFiClient client;
const char* servername ="api.openweathermap.org";  // remote server we will connect to
String result;

int  iterations = 1800;

/*END CODE BY EDUC8S.TV*/


void setup() 
{

     /*BEGIN CODE BY ME*/
    
     //this is to make the LEDs light up
     pinMode(red, OUTPUT); //sun
     pinMode(yellow, OUTPUT); //rain
     pinMode(green, OUTPUT); //cloudy
     pinMode(blue, OUTPUT); //windy
     pinMode(white, OUTPUT); //incliment weather
     pinMode(pink, OUTPUT); //Jacket

     /*END CODE BY ME*/

     /*BEGIN CODE BY EDUC8S.TV*/
     Serial.begin(9600);

     connectToWifi();

     /*END CODE BY EDUC8S.TV*/
}


/*ALL OF LOOP WRITTEN BY EDUC8S.TV*/

void loop() {
 
 delay(2000);

 if(iterations == 1800)//We check for updated weather forecast once every hour
 {
   getWeatherData();
   iterations = 0;   
 }
 iterations++;
}


/*CONNECT_TO_WIFI() WRITTEN BY EDUC8S.TV*/

void connectToWifi()
{
  WiFi.enableSTA(true);
  
  delay(2000);
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print("Connecting to WiFi...\n");
    }
   Serial.print("connected to wifi\n"); //ADDED BY ME
}



/*BULK OF GET_WEATHER_DATA() WRITTEN BY EDUC8S.TV... THIS SECTION OF CODE CONTAINS THE JSON PARSER CALLS WHICH HAVE STOPPED
 * WORKING FOR MY CODE, AS SUCH I ONLY HAVE 0 OR NULL VALUES AT THE END.
 */
 
void getWeatherData() //client function to send/receive GET request data.
{
      String result ="";
      WiFiClient client;
      const int httpPort = 80;
      if (!client.connect(servername, httpPort)) {
            return;
        }
          // We now create a URI for the request
        String url = "/data/2.5/weather?zip=30024,us&units=imperial&cnt=1&APPID="+APIKEY; //METRIC CHANGED TO IMPERIAL BY ME
    
           // This will send the request to the server
        client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                     "Host: " + servername + "\r\n" +
                     "Connection: close\r\n\r\n");
        unsigned long timeout = millis();
        while (client.available() == 0) {
            if (millis() - timeout > 5000) {
                client.stop();
                return;
            }
        }
    
        // Read all the lines of the reply from server
        while(client.available()) {
            result = client.readStringUntil('\r');
        }
    
    result.replace('[', ' ');
    result.replace(']', ' ');
    
    char jsonArray [result.length()+1];
    result.toCharArray(jsonArray,sizeof(jsonArray));
    jsonArray[result.length() + 1] = '\0';
    
    StaticJsonBuffer<1024> json_buf;
    JsonObject &root = json_buf.parseObject(jsonArray);
    if (!root.success())
    {
      Serial.println("parseObject() failed");
    }

    /*BEGIN CODE HEAVILY MODIFIED OR CREATED ENTIRELY BY ME*/
    
    //this gives us the current temperature outside
    String idTemp = root["main"]["temp"];
    //Serial.println("String temp is " + idTemp + "\n");
    
    float temp = idTemp.toFloat();
    
    Serial.print("\nThe current temperature is: ");
    Serial.println(temp);
    
        
    //this gives us the weatherID so we know what to display
    String idString = root["weather"]["id"];
    weatherID = idString.toInt();
    Serial.print("\nThe weather code is: ");
    Serial.println(idString);

    weatherLED(weatherID);
    
    getClothes(temp);
    
    wearJacket(temp, weatherID);

    /*END CODE HEAVILY MODIFIED OR CREATED ENTIRELY BY ME*/
    
    endNextionCommand(); //We need that in order the nextion to recognise the first command after the serial print

}

/*BEGIN CODE BY ME*/

/*WEATHER_LED() IS WHOLY MY OWN CREATION*/
void weatherLED(int weather_condition){
     
     if(weather_condition == 800){
          //light up sun LED... also clear sky
          //if temp < 60, light up jacket led
          digitalWrite(red, HIGH);
          Serial.println("\nThe LED is RED\n");
          Serial.println("\nThe weather is clear ");
        }
        else if(weather_condition >= 801 && weather_condition <= 804){
           //light up clouds LED
          //if temp < 60, light up jacket led
          digitalWrite(yellow, HIGH);
          Serial.println("\nThe LED is YELLOW\n");
          Serial.println("\nThe weather is cloudy");
        }
        else if((weather_condition >= 300 && weather_condition <= 321) || (weather_condition >= 500 && weather_condition <= 503) || (weather_condition >= 520 && weather_condition <= 531)){
          //light up rain LED
          digitalWrite(green, HIGH);
          Serial.println("\nThe LED is GREEN\n");
          Serial.println("\nThe weather is rainy");
        }
        else if(weather_condition >= 952 && weather_condition <= 956){
          //light up wind LED
          digitalWrite(blue, HIGH);
          Serial.println("\nThe LED is BLUE\n");
          Serial.println("\nThe weather is windy");
        }
       
        else{//incliment weather assumed
          digitalWrite(white, HIGH);
          Serial.println("\nThe LED is WHITE\n");
          Serial.println("\nThe weather is incliment");
          incliment_weather(weather_condition);
         
        }   
}


/*INCLIMENT_WEATHER() IS WHOLY MY OWN CREATION*/
void incliment_weather(int weather_condition){

  if(weather_condition == 900){
    //display torando
    sendTornadoToNextion();
  }
  else if(weather_condition == 901 || weather_condition == 902){
    //display hurricane
    sendHurricaneToNextion();
  }
  else if(weather_condition >= 957 && weather_condition <= 959){
    //extremem wind
    sendExtremeWindToNextion();
  }
  else if(weather_condition== 906){
    //hail
    sendHailToNextion();
  }
  else if((weather_condition == 504 || weather_condition == 511) || (weather_condition == 960 || weather_condition == 961)){
    //extreme rain
    sendExtremeRainToNextion();
  }
  else if((weather_condition >= 200 && weather_condition <= 232) || (weather_condition >= 960 && weather_condition <= 961)){
    //thunder storm
    sendThunderStormToNextion();
  }
  else if(weather_condition >= 600 && weather_condition <= 622){
    //display snow
    sendSnowToNextion();
  }
}


/*GET_CLOTHES() IS WHOLY MY OWN CREATION*/
void getClothes(int temp){
      Serial.print("\nBecause the temperature is: ");
      Serial.print(temp);
      Serial.println(" I should wear...");
      
      if(temp <= 32){
          Serial.println("Flannel Jammies with Feet, its too cold to run today!");
          sendStayInsideToNextion();
        }
        else if(temp > 32 && temp <= 40){
          //show hat, long sleeves, pants
          Serial.println("... a hat, long sleeves, and pants.\n");
          sendHatLongSleevesPantsToNextion();
        }
        else if(temp > 40 && temp <= 60){
          //show long sleeves, shorts
          Serial.println("... long sleeves, and shorts.\n");
          sendLongSleevesShortsToNextion();
        }
        else if(temp > 60 && temp <= 80){
          //show short sleeves, shorts
          Serial.println("... short sleeves, and shorts.\n");
          sendShortSleevesShortsToNextion();
        }
        else if(temp > 80){
          //show sleeveless, shorts
          Serial.println("... a sleeveless top, and shorts.\n");
          sendSleevelessShortsToNextion();
        }
}


/*WEAR_JACKET() IS WHOLY MY OWN CREATION*/
void wearJacket(float temp, int weather_condition){
//jacket LED lights up when its raining w/o thunder/lightning, and when its windy
  if((weather_condition >= 300 && weather_condition <= 321) || /* drizzle */
  (weather_condition >= 500 && weather_condition <= 503) || /* rain w/o lightning */  
  (weather_condition >= 520 && weather_condition <= 521) || /* more rain w/o lightning */
  (weather_condition >= 952 && weather_condition <= 956)){ /* wind */
    if(temp <= 60 && temp >= 32){
       digitalWrite(pink, HIGH);
       Serial.println("\nlight up pink jacket led\n");
    }
   
  } Serial.println("\n\nDONT light up pink jacket led\n");
}

/*END CODE BY ME*/


/*THE FOLLOWING CODE WAS MODIFIED FROM THE ORIGINAL TO USE WITH MY OWN IMAGES... THIS IS
 * EXACTLY HOW THE CODE IS FORMATED IN THE ORIGINAL, BUT FOR SOME REASON IT DOES NOT WORK
 * FOR MY CODE. I BELIEVE THIS TO BE A BAD CONNECTION IN THE HARDWARE AS THE SERIAL MONITOR 
 * OUTPUTS THE PROPER INFORMATION.
 */
//incliment weather diplays
void sendTornadoToNextion(){
    
  String command = "weatherIcon.pic=5";
  Serial.print(command);
  endNextionCommand();
}
void sendHurricaneToNextion(){
  
  String command = "weatherIcon.pic=4";
  Serial.print(command);
  endNextionCommand();
}
void sendExtremeWindToNextion(){
  
  String command = "weatherIcon.pic=3";
  Serial.print(command);
  endNextionCommand();
}
void sendHailToNextion(){
 
  String command = "weatherIcon.pic=8";
  Serial.print(command);
  endNextionCommand();
}
void sendExtremeRainToNextion(){

  String command = "weatherIcon.pic=2";
  Serial.print(command);
  endNextionCommand();
}
void sendThunderStormToNextion(){
  
  String command = "weatherIcon.pic=1";
  Serial.print(command);
  endNextionCommand();
}
void sendSnowToNextion(){
 
  String command = "weatherIcon.pic=0";
  Serial.print(command);
  endNextionCommand();
}


//stay inside its too cold display
void sendStayInsideToNextion(){
    
  String command = "weatherIcon.pic=9";
  Serial.print(command);
  endNextionCommand();
}

//begin clothing displays
void sendHatLongSleevesPantsToNextion(){
    
  String command = "weatherIcon.pic=10";
  Serial.print(command);
  endNextionCommand();
}
void sendLongSleevesShortsToNextion(){
    
  String command = "weatherIcon.pic=11";
  Serial.print(command);
  endNextionCommand();
}
void sendShortSleevesShortsToNextion(){

   
  String command = "weatherIcon.pic=12";
  Serial.print(command);
  endNextionCommand();
 
}
void sendSleevelessShortsToNextion(){
   
  String command = "weatherIcon.pic=13";
  Serial.print(command);
  endNextionCommand();
}

//end nextion commands
void endNextionCommand(){
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);
}


/*END ALL CODE*/


