#ifndef __DEVICE_CONFIG_H__
#define __DEVICE_CONFIG_H__

#include <FS.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>

class DeviceConfig
{
  static constexpr const char *CONFIG_FILE = "/config.json";
  
  public:
  DeviceConfig():deviceName{0}, mqttServer{0} {}

  const char* getDeviceName() const{return deviceName;}
  const char* getMqttServer() const {return mqttServer;}

  void setDeviceName(const char* name) { strncpy(deviceName, name, sizeof(deviceName));    deviceName[sizeof(deviceName)-1] = 0; }
  void setMqttServer(const char* server) { strncpy(mqttServer, server, sizeof(mqttServer)); mqttServer[sizeof(mqttServer)-1] = 0; }
  
  void setup()
  {
     fsReadConfig();
     WiFiManager wifiManager;
     WiFiManagerParameter custom_device_name("device", "Device Name", deviceName, sizeof(deviceName));
     WiFiManagerParameter custom_mqtt_server("server", "MQTT Server", mqttServer, sizeof(mqttServer));
     
     wifiManager.addParameter(&custom_device_name);
     wifiManager.addParameter(&custom_mqtt_server);

     wifiManager.setSaveConfigCallback([](){saveConfig = true;});
     wifiManager.autoConnect();  

     if (saveConfig)
     {
       strncpy(deviceName, custom_device_name.getValue(), sizeof(deviceName));
       strncpy(mqttServer, custom_mqtt_server.getValue(), sizeof(mqttServer));
       fsSaveConfig();
     }

  	 WiFi.hostname(this->getDeviceName());
     IPAddress localIp = WiFi.localIP();
     Serial.print("localIp: ");
     Serial.println(localIp.toString());
  }

  void save(){fsSaveConfig();}
  
  private:

    void error(const char* msg)
    {
      Serial.print("DeviceConfig error: ");
      Serial.println(msg);
    }
  
    void info(const char* msg)
    {
      Serial.print("DeviceConfig info: ");
      Serial.println(msg);
    }
    void info(const char* msg1, const char* msg2)
    {
      Serial.print("DeviceConfig info: ");
      Serial.print(msg1);
      Serial.print(" ");
      Serial.println(msg2);
    }

    void fsReadConfig()
    {
      info("fsReadConfig()");
      if (SPIFFS.begin())
      {
        if (SPIFFS.exists(CONFIG_FILE))
        {
          File configFile = SPIFFS.open(CONFIG_FILE, "r");
          if (configFile)
          {
            size_t size = configFile.size();
            std::unique_ptr<char[]> buffer(new char[size]); // dynamic memory !!!
            configFile.readBytes(buffer.get(), size);
            configFile.close();
            
            DynamicJsonBuffer jsonBuffer;
            JsonObject& json = jsonBuffer.parseObject(buffer.get());
            if (json.success())
            {
              auto jsonDeviceName = json["device_name"];
              if (jsonDeviceName.success())
              {
                strncpy(deviceName, jsonDeviceName, sizeof(deviceName));
                info("device_name", deviceName);
              }
  
              auto jsonMqttServer = json["mqtt_server"];
              if (jsonMqttServer.success())
              {
                strncpy(mqttServer, jsonMqttServer, sizeof(mqttServer));
                info("mqtt_server", mqttServer);
              }
            } else error("failed to parse config file data");
          }
          else error("failed to open config file");
        }
        else info("no config file");
      }
      else error("SPIFFS.begin() failed");
    }

    void fsSaveConfig()
    {
      info("fsSaveConfig()");
      DynamicJsonBuffer jsonBuffer;
      JsonObject& json = jsonBuffer.createObject();

      info("device_name", deviceName);
      info("mqtt_server", mqttServer);
                      
      json["device_name"] = deviceName;
      json["mqtt_server"] = mqttServer;
      File configFile = SPIFFS.open(CONFIG_FILE, "w");
      if (configFile)
      {
        json.printTo(configFile);
        configFile.close();
      } 
      else error ("failed to save config file");
    }

     char deviceName[40];
     char mqttServer[40];
     static bool saveConfig;
};

bool DeviceConfig::saveConfig = false;

#endif
