/*
 * StatusColorLED.h
 *
 *  Created on: 13.01.2019
 *      Author: a4711
 */

#ifndef SRC_STATUSCOLORLED_H_
#define SRC_STATUSCOLORLED_H_

#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>
#include "myiot_timer_system.h"

class StatusColorLED : public MyIOT::ITimer {
public:

	enum Mode
	{
		None,
		Rainbow,
		Blink
	};


	StatusColorLED();
	virtual ~StatusColorLED();

	template <uint8_t PIN>
	void setup(){
		FastLED.addLeds<WS2812B, PIN, GRB>(leds, 1); // define FASTLED_ESP8266_RAW_PIN_ORDER before "FastLED.h" include
		leds[0] =  0; // CRGB::Cyan;
		FastLED.setBrightness(50);
		FastLED.show();
	}

	void setColor(int rgb)
	{
		reset(None);
		leds[0] = rgb;
		FastLED.show();
	}

	void rainbow()
	{
		reset(Rainbow);
		interval_ms = 100;
		hue = 0;
	};

	void blink()
	{
		reset(Blink);
		interval_ms = 500;
	}

	void expire()
	{
		unsigned long cur = millis();
		if (cur - last >= interval_ms)
		{
		    tick();
			last = cur;
		}
	}
	void destroy(){}
private:

	void tick()
	{
		switch (mode)
		{
		case Rainbow:
			leds[0].setHSV(hue, 255, 255);
			FastLED.show();
			hue += 5;
			break;
		case Blink:
			if (0 == brightness)
			{
				brightness = 50;
			}
			else
			{
				brightness = 0;
			}
			FastLED.setBrightness(brightness);
			FastLED.show();
			break;
		default:
			break;

		}
	}

	void reset(Mode m)
	{
		mode = m;
		last = 0;
		interval_ms = 0;
		brightness = 50;
		FastLED.setBrightness(brightness);
	}


	CRGB leds[1];
	Mode mode = None;

	unsigned long last = 0;
	unsigned long interval_ms = 0;
	uint8_t brightness = 50;

	uint8_t hue = 0;
};

#endif /* SRC_STATUSCOLORLED_H_ */
