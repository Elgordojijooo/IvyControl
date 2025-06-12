#include <WebServer.h>
#include "ivy_wifi.h"
#include "ivy_nuts_and_bolts.h"
#include "ivy_relay.h"
#include "DHT.h"
#include "ArduinoOTA.h"
#include "time.h"
#include "time_tools.h"

DHT dht(4, DHT11);

auto current_millis = millis();

auto last_sensor_update = current_millis;
void updateSensors(){
  if(current_millis - last_sensor_update >= 10000){
    getLocalTime(&timeinfo);
    last_sensor_update = current_millis;
    getLocalTime(&timeinfo); // I also update the time here!
    sensors.humidity = dht.readHumidity();
    sensors.temperature = dht.readTemperature();

    Serial.print(timeinfo.tm_hour);
    Serial.print(":");
    Serial.print(timeinfo.tm_min);
    Serial.print(":");
    Serial.print(timeinfo.tm_sec);
    Serial.print(" - ");
    Serial.print("Humidity: ");
    Serial.print(sensors.humidity);
    Serial.print(" Temp: ");
    Serial.print(sensors.temperature);
    Serial.println("");
    
    Serial.println('\n');
  } 
}

void setup() {
  Serial.begin(115200);
  ivy_wifi_begin();
  dht.begin();
  ivy_outlet_begin();
  ArduinoOTA.begin();
  ArduinoOTA.setPassword("1234");
  configTime(-3600*3, 0, "pool.ntp.org");
}

int lastUpdatedMinute5m = -1;
int lastUpdatedMinute1h = -1;
int lastUpdatedHour24h = -1;
void updateHistoryCharts() {
  int currentMinute = timeinfo.tm_min;
  int currentHour = timeinfo.tm_hour;

  // Update 5-minute history every 5 minutes, but only once per minute
  if (currentMinute % 5 == 0 && currentMinute != lastUpdatedMinute5m) {
    lastUpdatedMinute5m = currentMinute;

    for (int i = 0; i < 59; i++) {
      tempHistory5m[i] = tempHistory5m[i + 1];
      humidityHistory5m[i] = humidityHistory5m[i + 1];
    }
    tempHistory5m[59] = (int)sensors.temperature;
    humidityHistory5m[59] = (int)sensors.humidity;
  }

  // Update 1-hour history every hour (minute == 0), but only once
  if (currentMinute == 0 && currentMinute != lastUpdatedMinute1h) {
    lastUpdatedMinute1h = currentMinute;

    for (int i = 0; i < 59; i++) {
      tempHistory1h[i] = tempHistory1h[i + 1];
      humidityHistory1h[i] = humidityHistory1h[i + 1];
    }
    tempHistory1h[59] = (int)sensors.temperature;
    humidityHistory1h[59] = (int)sensors.humidity;
  }

  // Update 24-hour history at midnight (00:00), but only once
  if (currentHour == 0 && currentMinute == 0 && currentHour != lastUpdatedHour24h) {
    lastUpdatedHour24h = currentHour;

    for (int i = 0; i < 23; i++) {
      tempHistory24h[i] = tempHistory24h[i + 1];
      humidityHistory24h[i] = humidityHistory24h[i + 1];
    }
    tempHistory24h[23] = (int)sensors.temperature;
    humidityHistory24h[23] = (int)sensors.humidity;
  }
}


auto last_logic_run = current_millis;
void loop() {
  ArduinoOTA.handle();
  ivy_wifi_loop();
  updateSensors();
  current_millis = millis();
  if(current_millis - last_logic_run >= 5000){
    last_logic_run = current_millis;
    onLogic();
    ivy_outlet_update();
    updateHistoryCharts();
  }
}

const int SI = 1;
const int NO = 0;

#define TEMPERATURA sensors.temperature
#define HUMEDAD sensors.humidity
#define HORA    timeinfo.tm_hour
#define MINUTO  timeinfo.tm_min
#define SEGUNDO timeinfo.tm_sec

// Aliases y nicknames para adaptar los idiomas!
#define LUCES_2         outlet[0].auto_state
#define LUCES_1         outlet[1].auto_state
#define VENTILADOR      outlet[2].auto_state
#define EXTRACTOR       outlet[3].auto_state
#define VEGE_LUCES      outlet[4].auto_state
#define VEGE_VENTILADOR outlet[5].auto_state

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Vos te encargás de la logica, yo me encargo del resto
void onLogic()
{
  // 2
  LUCES_1 = entre_horas(21, 9);

  // 3
  LUCES_2 = entre_horas(23, 7);
  
  // 30 minutos cada dos horas
  VENTILADOR = (HORA % 1 == 0) && (MINUTO < 30);

  // 30 minutos cada cuatro horas
  EXTRACTOR = (HORA % 3 == 0) && (MINUTO < 30);

  // Humidificador 
  //if (sensors.humidity > 80) HUMIDIFICADOR = NO;
  //if (sensors.humidity < 60) HUMIDIFICADOR = SI;
}








