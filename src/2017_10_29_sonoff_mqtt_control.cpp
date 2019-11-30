#include <Arduino.h>

#include "myiot_timer_system.h"
#include "myiot_DeviceConfig.h"
#include "myiot_webServer.h"
#include "myiot_mqtt.h"
#include "myiot_ota.h"

#include "DeviceButton.h"
#include "led.h"
#include "relay.h"
#include "TestVcc.h"
#include "StatusColorLED.h"

MyIOT::TimerSystem tsystem;
MyIOT::Mqtt mqtt;
MyIOT::DeviceConfig config;
MyIOT::OTA ota;
MyIOT::WebServer webServer;

DeviceButton button(0); // GPIO 0 is the button
Led led(13);  // LED of sonoff
Relay relay(12);
StatusColorLED statusLED; // WS2812B on GPIO14

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
	statusLED.setup<14>();

	mqtt.subscribe("rgb", [](const char*msg)
	{
		if (0 == strcmp(msg,"rainbow"))
		{
			statusLED.rainbow();
		}
		else if (0 == strcmp(msg,"red"))
		{
			statusLED.setColor(CRGB::Red);
		}
		else if (0 == strcmp(msg,"green"))
		{
			statusLED.setColor(CRGB::Green);
		}
		else if (0 == strcmp(msg,"blue"))
		{
			statusLED.setColor(CRGB::Blue);
		}
		else if (0 == strcmp(msg, "blink"))
		{
			statusLED.blink();
		}
		else
		{
			int val = ::atoi(msg);
			Serial.println(val);
			statusLED.setColor(val);
		}
	});

    tsystem.add(&statusLED, MyIOT::TimerSystem::TimeSpec(0,50e6));
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
			  relay.timed_on(cmd); // Schalte f�r N Sekunden ein.
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
