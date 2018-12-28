#include <Arduino.h>

FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>
#include "src/myiot_timer_system.h"
#include "src/myiot_DeviceConfig.h"
#include "src/myiot_webServer.h"
#include "src/myiot_mqtt.h"
#include "src/myiot_ota.h"

#include "src/DeviceButton.h"
#include "src/led.h"
#include "src/relay.h"
#include "src/TestVcc.h"

MyIOT::TimerSystem tsystem;
MyIOT::Mqtt mqtt;
MyIOT::DeviceConfig config;
MyIOT::OTA ota;
MyIOT::WebServer webServer;

DeviceButton button(0); // GPIO 0 is the button
Led led(13);  // LED of sonoff
Relay relay(12);

void setup_system()
{
	config.setup();

	ota.setup(config.getDeviceName());
	webServer.setup(config);
	mqtt.setup(config.getDeviceName(), config.getMqttServer());

	tsystem.add(&ota, MyIOT::TimerSystem::TimeSpec(0, 100e6));  // 100ms
	tsystem.add(&webServer, MyIOT::TimerSystem::TimeSpec(0, 100e6));  // 100ms
	tsystem.add(&mqtt, MyIOT::TimerSystem::TimeSpec(0, 100e6));  // 100ms
}

CRGB leds[1];

void setup_status_led()
{
	FastLED.addLeds<WS2812B, 14, GRB>(leds, 1); // define FASTLED_ESP8266_RAW_PIN_ORDER before "FastLED.h" include
	leds[0] =  CRGB::Cyan;
	FastLED.setBrightness(50);
	FastLED.show();

	mqtt.subscribe("rgb", [](const char*msg)
	{
		int val = ::atoi(msg);
		Serial.println(val);
		leds[0] = val;
		FastLED.show();
	});
}


void setup() {

	Serial.begin(76800);
	setup_system();
	setup_status_led();


   led.setup();
   relay.setup([](const char* topic, const char* msg){mqtt.publish(topic,msg);} );
   button.setup([](){ mqtt.publish("button", "pressed"); }  );
   testVcc.setup([](const char* topic, const char* msg){mqtt.publish(topic,msg);}  );

   mqtt.subscribe("control", [] (const char*msg) {
		if (0 == ::strcmp("on", msg))
		{
			relay.enable(true);
		}
		else if (0 == ::strcmp("off",msg))
		{
			relay.enable(false);
		}
		else if (0 == ::strcmp("toggle", msg))
		{
			relay.toggle();
		}
		else
		{
			int cmd = ::atoi(msg);
			if (cmd > 0)
			  relay.timed_on(cmd); // Schalte für N Sekunden ein.
			else
			  relay.enable(::atoi(msg) == 1);
		}
   });

   tsystem.add(&led, MyIOT::TimerSystem::TimeSpec(0,500000000));
   tsystem.add(&relay, MyIOT::TimerSystem::TimeSpec(0,10e6));
   tsystem.add(&button, MyIOT::TimerSystem::TimeSpec(0,100e6));
   //tsystem.add(&testVcc, MyIOT::TimerSystem::TimeSpec(3));
}



void loop() {
  tsystem.run_loop(10,1);
}
