# MotionHue-esp32
A motion sensor connected to an ESP32 board, which when it detects motion, it sends a command to turn on a specific light to the server. The server consequently connects to a specified Hue bridge, and sends an HTTP request to a specified light. the board sends a turn off light signal after 15 seconds of no motion.

## Specs used:
**1 - ESP32 development boad**
**2- PIR motion sensor**
**3 -Philips Hue bridge**

## Usage:

1- **the ESP32 should be connected to the wifi**: uncomment the nvs_wifi_cred_set line in main.c in the client side, and set with correct credentials. This will copy the credentials to the board and stay there until nvs memory is reset. ONLY run once! comment out and remove credentials afterwards.
2- **Specify sensor PIN**: in sensor.c in the client side, change SENSOR_PIN to the correct value.
3-**specify correct IP**: in client.c, specify the server address via changing SERVER_ADDR value.
4-**specify hue key**: define an enivomental variable called HUE_KEY, this can be temporarily done via the following command `export HUE_KEY="hue_key_value"` or permenantly via the shell file. 
5-**specifiy the light ID**: in hue.c, specify the light ID of the light you wanna alter.
**NOTE**: for 5 and 6 steps, follow https://developers.meethue.com/develop/hue-api-v2/getting-started/ to obtain the hue key and light ID.
6- **compile and run the server**: compile using the provided makefile.
7-  **run** `./build.sh`  on the client side.
**NOTE**: if sockadd_in is not defined, run `idf.py menuconfig` and under **lwip**, enable **standard POSIX API**.

## Concepts Used:
**esp32 hardware timer**
**esp32 wifi connectivity**
**POSIX API cilent-server communication**


## LIMITATIONS:
1- **The client doesnt detect if it's disconnected from the server**
2- **Hue_key and light ID need to be manually set in the code**
3- **SSL certificate checking is not validated but ignored**
