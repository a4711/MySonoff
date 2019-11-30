# This is a small project to control a SONOFF "Basic" switch.

My switch is extended with a WS2812B LED attached to PIN 14, which also can be controlled via MQTT.

To attach the switch to your WIFI network, you have to press the button of the SONOFF BASIC. Than the switch will offer an WIFI access point. Connect to this WIFI and configure the switch via the web browser interface (WIFI credentials, deviceName and MQTT server).

When the switch is configured for your network, there is still a little web browser interface to control the switch and to get information about your hardware.

# MQTT
The switch can be controlled by MQTT:

* &lt;deviceName&gt;/control : message = [ON|OFF|<seconds>|toggle]
* &lt;deviceName&gt;/rgb : message = [red|green|blue|blink|rainbow|&lt;decimalRGB&gt;]
