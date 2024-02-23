/******************************************************************
Mesure de co2 temperature humidite pression
les capteurs sont :
ccs811 ---> mesure eco2 et ecov
le dht 11 est remplacé par un bme280
Un serveur webb est opérationnel en mode ap sur http://192.168.4.1/

********************************************************************/
#include <Arduino.h>

#include <WiFi.h>
#include <HttpClient.h>
// librairie horloge mondiale
#include <NTPClient.h>
// libairie pour un serveur web
#include <ESPAsyncWebServer.h>
// libriarie de gestion du wifi
#include <WiFiUdp.h>
// les pages web sont stockée la
#include <serveur_webb.h>
// librairie du capteur de température
#include <Adafruit_AHTX0.h>


// instanciation du capteur de température
Adafruit_AHTX0 aht;
// création des meusere
sensors_event_t humidity, temp;

// Replace with your network credentials
//char *ssid = "";
 const char *ssid = "phoebus_gaston";
//char *password = "";
 const char *password = "phoebus09";

WiFiClient client; // pour utiliser un proxy

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

long previousMillis = 0;
long previousMillis_scroll = 0;
#define INTERVAL 60000 // 300000

// variable pour l affichage de l'horodatage
time_t lheure;
String ladate = "";
String heure = "";
time_t utcCalc;

// non du wifi ap
const char *soft_ap_ssid = "truc2";
const char *soft_ap_password = "";

/*variables permettant de traiter le get de la page web
lors d'une requette web de type GET, les paramètres sont identifier
ex : http://192.168.1.159/?fname=garage&lname=AC%3A67%3AB2%3A37%3AB9%3AB0&commentaire=&reseauwifi=Livebox-6780
*/ 

const char *PARAM_INPUT_1 = "fname";
const char *PARAM_INPUT_2 = "lname";
const char *PARAM_INPUT_3 = "commentaire";
const char *PARAM_INPUT_8 = "reseauwifi";

const char *PARAM_INPUT_4 = "calibrate";
const char *PARAM_INPUT_5 = "state";

String numsalle = "garage";
String numsonde = "2";
String commentaire = "";

String boutton = "le bouton";

String letat_calibration = "on";

boolean transmit = false;

/*mise en place d'une listre pour recevoir
les id des réseau wifi
*/
String liste_wifi[10];

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

float arrondi(float val, int precision)
/*
Fonction de calcul de l'arrondi
param :
  val (float) valeur a arrondir)
  precision (in) precision de l'arrondi
return :
  renvoi un float de la valeur arrondie
*/
{
  val = val * precision;
  val = int(val);
  val = val / precision;
  return val;
}



void printValues()
{
  /*
  Affiche les valeurs mesurées
  */
  Serial.print("Temperature = ");
  Serial.print(temp.temperature);
  Serial.println(" °C");


  Serial.print("Humidity = ");
  Serial.print(humidity.relative_humidity);
  Serial.println(" %");

  Serial.println();
}

void horodatage()
{
  /*
  mise à jour de l'heure en interrogeant une horloge atomique
  formatage de la date et l'heur
  */
  while (!timeClient.update())
  {
    timeClient.forceUpdate();
  }

  formattedDate = timeClient.getFormattedDate();
  // Extract date
  int splitT = formattedDate.indexOf("T");
  ladate = formattedDate.substring(0, splitT);

  // Extract time
  heure = formattedDate.substring(splitT + 1, formattedDate.length() - 1);
}

// Replaces placeholder with button section in your web page
String processor(const String &var)
/* 
Remplace les tags %tag% (mot encadré par %) dans la page web par
les variables nécessaire à la création dynamique de la page web
*/
{

  // Serial.println(var);
  if (var == "numsalle")
  {
    return String(numsalle);
  }
  else if (var == "numsonde")
  {
    return String(numsonde);
  }
  else if (var == "co2")
  {
    return String("ppm");
  }
  else if (var == "humidite")
  {
    return String(humidity.relative_humidity);
  }
  else if (var == "temperature")
  {
    return String(temp.temperature);
  }
  else if (var == "date")
  {
    return String(ladate);
  }
  else if (var == "heure")
  {
    return String(heure);
  }
  else if (var == "etat")
  {
    return String(letat_calibration);
  }
  else if (var == "BUTTONPLACEHOLDER")
  {
    String buttons = "";
    buttons += "<h4>Calibration </h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"2\" " + boutton + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  else if ( var == "LISTRESEAUWIFI")
  {
    String leselect ="";
    int n = (sizeof(liste_wifi)/sizeof(*liste_wifi));

  for (int i = 0; i < n; i++)
  {
    //Serial.println(liste_wifi[i]);
    leselect +="<option value=\""+liste_wifi[i]+"\">"+liste_wifi[i]+"</option>";
  }
  return leselect;
  }
  return String();
}

void send_Request(byte *Request, int Re_len)
{
  while (!Serial1.available())
  {
    Serial1.write(Request, Re_len); // Send request to S8-Sensor
    delay(50);
  }
}

void setup()
{
  // Initialize Serial Monitor
  Serial.begin(115200);

  Serial.println("Adafruit AHT10/AHT20 demo!");

  if (! aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }
  Serial.println("AHT10 or AHT20 found");


  WiFi.mode(WIFI_AP_STA);

  WiFi.softAP(soft_ap_ssid, soft_ap_password);
  IPAddress IP = WiFi.softAPIP();
  /* --------------  Selection du reseau ------------*/
  int n = WiFi.scanNetworks();
  Serial.print(n);
  Serial.println(" network(s) found");
  for (int i = 0; i < n; i++)
  {
    Serial.println(WiFi.SSID(i));
    liste_wifi[i]=WiFi.SSID(i);
    if (WiFi.SSID(i) == "Borde Basse Personnels")
    {
      ssid = "Borde Basse Personnels";
      password = "";
      Serial.println("ssid trouvé");
    }
    else if ((WiFi.SSID(i) == "phoebus_gaston"))
    {
      ssid = "phoebus_gaston";
      password = "phoebus0911";
    }
  }
  Serial.print("connecté sur ");
  Serial.println(ssid);
  /*--------------- fin de selection du reseau --------*/
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    // delay(500);
    Serial.print(".");
  }

  if (ssid == "Borde Basse Personnels")
  {
    client.connect(IPAddress(10, 255, 6, 124), 3128);
  }

  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Mac address  ");
  numsonde = WiFi.macAddress();
  Serial.println(numsonde);

  Serial.print("AP IP address: ");
  Serial.println(IP);
  Serial.println(WiFi.localIP());
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(3600);

  Serial.println();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              // nombre de parametres dans le GET
               int paramsNr = request->params();
                Serial.println(paramsNr);
 
    for(int i=0;i<paramsNr;i++){
        // affichage des parametres nom et valeurs
        AsyncWebParameter* p = request->getParam(i);
        Serial.print("Param name: ");
        Serial.println(p->name());
        Serial.print("Param value: ");
        Serial.println(p->value());
        Serial.println("------");
    }
              Serial.print("Reception get ");
              Serial.println(request->hasParam(PARAM_INPUT_2));
              if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2))
              {

                String inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
                String inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
                String inputMessage3 = request->getParam(PARAM_INPUT_3)->value();
                String inputMessage4 = request->getParam(PARAM_INPUT_8)->value();
                // digitalWrite(inputMessage1.toInt(), inputMessage2.toInt());
                numsalle = inputMessage1;
                numsonde = inputMessage2;
                commentaire = inputMessage3;
                Serial.print(inputMessage1);
                Serial.print(" ");
                Serial.print(inputMessage2);
                 Serial.print(" ");
                Serial.println(inputMessage4);
                // renvoi de la page index avec traitement par la fonction  processor
                request->send_P(200, "text/html", index_html, processor);
                transmit = true;
              }
              else
              {
                horodatage();
                request->send_P(200, "text/html", index_html, processor);
              } });

  // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
  server.on("/calibration", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String rep1;
    String rep2;
    Serial.println("calibration");
    // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    if (request->hasParam(PARAM_INPUT_4) && request->hasParam(PARAM_INPUT_5)) {
      rep1 = request->getParam(PARAM_INPUT_4)->value();
      rep2 = request->getParam(PARAM_INPUT_5)->value();
      Serial.print("State ");
      Serial.println(rep2);      

      if (rep2=="1")
      {
        letat_calibration="Ok";
        Serial.println("ok");
      }
      if (rep2=="0")
      {
        letat_calibration="NOk";
        Serial.println("nok");
      }
      request->send(200, "text/plain", letat_calibration);
    }
    request->send(200, "text/plain", "OK"); });

  // Start server
  server.begin();

  timeClient.begin();

  horodatage();
  printValues();

}
void loop()
{

  aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data

  // int remainingTimeBudget = ui.update();

  unsigned long currentMillis = millis();
  /*
    if (remainingTimeBudget > 0)
    {
      // You can do some work here
      // Don't do stuff if you are below your
      // time budget.
      humidity = bme.readHumidity();
      temperature = bme.readTemperature();

      delay(remainingTimeBudget);
    }
    */

  if (currentMillis - previousMillis >= INTERVAL)
  {
    aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
    while (!timeClient.update())
    {
      timeClient.forceUpdate();
    }
    // The formattedDate comes with the following format:
    // 2018-05-28T16:00:13Z
    // We need to extract date and time
    horodatage();
    formattedDate = timeClient.getFormattedDate();
    Serial.println(formattedDate);

    // Extract date
    int splitT = formattedDate.indexOf("T");
    dayStamp = formattedDate.substring(0, splitT);
    Serial.print("DATE: ");
    Serial.println(dayStamp);
    // Extract time
    timeStamp = formattedDate.substring(splitT + 1, formattedDate.length() - 1);
    Serial.print("HOUR: ");
    Serial.println(timeStamp);


    aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
    Serial.print(temp.temperature);
    Serial.print("\t");
    Serial.print(humidity.relative_humidity);
    Serial.print(" %");

    if (WiFi.status() == WL_CONNECTED)
    {
      //HTTPClient http; // Object of class HTTPClient
      // mySensor.measureAirQuality();
      //Serial.println("");
      //String requete = "http://flal09.free.fr/meteo/capteurs/update_co2.php?date=" + ladate + "&heure=" + heure + "&co2=" + ppm + "&tvoc=" + tvoc + "&capteur=" + numsonde + "&temperature=" + temp.temperature + "&humidite=" + String(humidity.relative_humidity) + "&pression=" + P;

      //Serial.println(requete);
      //http.begin(requete);
      // http.begin(requete);
      //int httpCode = http.GET();
      //Serial.print("Retour execution ");
      //Serial.println(httpCode);
      //client.stop();
    }

 

    previousMillis = currentMillis;
  }
}