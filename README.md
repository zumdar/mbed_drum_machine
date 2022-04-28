# Mbed Bicycle Drum Machine 
### ECE 4180 B
William Kennedy, Patrick Hellman, Bhuiyan Rifat al hadi 

Georgia Institute of Technology

bicycle tempo sync'ed analog drum machine using the Mbed
<br>
based on [juanito moore's easy ei8ht drum triggering code for arduino](https://github.com/ozerik/EasyEi8ht)
<br>
and 
<br>
[delptronics analog drum machine](https://delptronics.com/ldb1.php)

## Overview
The goal of this project is to create a digital drum machine using circuit componenets and an Mbed microcontroller. The idea is to use basic componennts like resistors, capacitors and gates to create circuits that produce commone drumnoises and actiave those circuits using digitalout pins from the mbed.   

![Final Product](https://github.com/zumdar/mbed_drum_machine/blob/main/3600%20project%20prototype%20.jpeg)

With the advent of smartphones, smart assistants, and smart watches, the alarm clock has seemingly faded into the technology graveyard. However, our group believed that alarm clocks still have the potential to make a resurgence built around practical features. What if your alarm clock acted like your mom when you were younger, forcing you to get out of bed and not leaving you alone until you were **_really_** awake? This smart alarm clock allows you to set an alarm along with a code the night before. The next morning, you have to remember the code that you entered the night before in order to turn the alarm off. Otherwise, the clock will increase the stimulus by turning up the volume and increasing the LED blinking intensity. This keeps the user on his/her toes and is built around the core functionality of actually making sure they start their day right. Additionally, the clock is paired with a high resolution touchscreen display that shows at-a-glance current weather and world news. The alarm consists of an LED ring stimulus and music, which are detailed later on this page.

## Components
### AWS IoT Core
Being an Internet of Things device, Amazon Web Services IoT Core is the backbone of how this alarm clock runs. IoT Core runs off of using MQTT as a communication protocol between devices. This project uses three “devices” which are communicating with one another: a JavaScript controlled GUI which displays to the user, a C script which controls the speakers and LED ring, and the user’s cell phone which is used to set the alarms.

### MQTT
MQTT is a lightweight communication protocol which is commonly used in IoT devices. It is based off of the premise that many devices exist and do not communicate directly with one another. Instead, all communication is routed through a “broker.” Devices can subscribe to various “topics” which are used to differentiate what information they see/can send to other devices. Likewise, the devices can publish to various topics. Any device that is subscribed to a topic which has a message published to it will be able to see these messages. The broker in this case manages what devices are subscribed to which topics. When a device publishes a message to a specific topic, the broker handles this message and sends it to all other devices subscribed to the same topic. This protocol is fantastic for vast, complicated sensor networks with various functionalities and workgroups.

### Our Device Topology
In our case, the layout is much simpler. The JavaScript GUI is served in an HTML file from a Node.JS server. This Node.JS server is built inside of a script which connects to AWS IoT Core. The script subscribes to the “sdkTest/sub” topic and also publishes to the same topic. This script is detailed later in this document. The C Script which controls the speakers and LED ring also is written inside of a larger script which connects to AWS IoT Core (and subscribes/publishes to “sdkTest/sub”). Lastly, the user’s cell phone connects to the system by using a node-red built user interface. More information on node-red setup can be found [here](https://nodered.org/#get-started). The UI is built using nodes that are installed from node-red-dashboard. This webpage takes in information inputted by the user to set their alarm, including the date/time and code to turn it off. A JavaScript function converts this user input into a single string which can be interpreted by the C Script and GUI and sends it to the AWS IoT Core broker on the topic “sdkTest/sub.” This lays the foundation for communication between the three major functions in our embedded device.

### Included Files
Information and a detailed tutorial for AWS IoT Core setup can be found [here](https://aws.amazon.com/iot-core/). The GitHub repository in this project omits the entirety of the AWS provided directories, as to avoid publishing any private information to a public location. AWS requires a full account setup and includes various API keys and certificates inside of its directories, so only the main scripts that were edited are included in this repository (with sensitive information excluded). The files “device-example.js” and “subscribe-publish-sample.c” are included to show how the embedded device is setup and runs. If you would like to recreate this project, simply replace these files in the AWS provided sample project directories and include your own user-generated certificates and keys from your account.

### Raspberry Pi 3
We chose to use the Raspberry Pi 3 Model B as the main controller of the alarm clock due to its ease of use and support for our IO devices. Since the Pi has a built-in Wi-Fi chip and support for the touchscreen, we could focus on implementing features instead of trying to get many small devices working with each other. The Adafruit Speaker Bonnet also allows easy control of speakers directly from the GPIO pins. This alarm clock did not have any real-time response constraints, so we did not need a faster, dedicated microcontroller for any functionality.

### Raspberry Pi Foundation Touch Screen
The alarm clock is outfitted with a touch display which can be found [here](https://www.raspberrypi.org/products/raspberry-pi-touch-display/). This connects to the Raspberry Pi using a short ribbon cable in the Pi’s DSI Port. The display receives power from the Pi’s 5V Out and Ground GPIO connectors. The display is 800x480, but the Pi automatically adjusts its resolution and output for this.

### JavaScript Controlled GUI
The JavaScript GUI is one of the two fronts that the user interacts with. Mostly, this component exists to show the user at-a-glance information that other IoT devices may display, such as current time, weather, and a scrolling daily news feed. Additionally, this is where the user inputs their code whenever the keypad pops up at the time of the alarm. 

![GUI](https://github.com/ecenteno7/RPi3-IoT-Alarm-Clock-ECE4180/blob/master/images/gui.jpg)

The JavaScript code is found in device-example.js, which connects to AWS IoT core and establishes a subscription on topic “sdkTest/sub” using our credentials. Also in this script, an Express (NodeJS module) server is set up which serves the browser an HTML page, index.html. The Express server also serves the client-side JavaScript in the HTML images from the /img directory.

When the script receives a message, it runs a function which you can see in the device.on(‘message’, function) listener. This function checks to see if the received message is in the format 1_A_CODE or 1_B_CODE. The “1” signifies that this is a message intended to turn the number pad script on or off. The “A” tells the script to emit a message to the client-side JavaScript to turn the number pad display code on, and the “B” tells it to turn the code off. The Node.JS server and the client-side JavaScript communicate using the [Socket.IO](https://socket.io/docs/) library and module. The client-side JavaScript has an event listener waiting for the “enableNumPad.alarmCode” or “disableNumPad” message from the Node.JS server, which has a function to run for either message. The first message turns on the display for the number pad and listens for the entry by the user. If the entry matches the code that was sent along with the “enableNumPad” message, the client-side Javascript sends an “alarmNotification” message with a payload of “true” to the Node.JS server. For each incorrect code entry attempt, the client-side script sends “alarmNotification” message with a payload of “false.” The Node.JS server has an event listener that waits for these alarmNotification messages, and runs a function for either message that is received. The function either publishes a “3_0” or a “3_1” to the topic “sdkTest/sub” which the IoT Core MQTT broker receives and sends off to the C script.

While all of this JavaScript runs while the alarm is going off, the rest of the client-side JavaScript serves to dynamically update the HTML GUI that the user is seeing. The weather is displayed using the [OpenWeatherMap API](https://openweathermap.org/current) by using an httpGet() request. The current news is displayed using an RSS feed from BBC World News. The client-side JavaScript listens for the Node.JS server to update the newest entries from the RSS feed using the parser.parseURL() function and loops through displaying them every 10 seconds. After 10 stories have been displayed, the client-side JavaScript emits a feedUpdateREQ message which tells the Node.JS server to pull the RSS feed again for refreshed news. Additionally, the current time is displayed using the startTime() and checkTime() functions in the client-side JavaScript in index.html. The page is styled using CSS and is updated with the JavaScript by editing the innerHTML or stye.display of each element.

The GUI can be launched by visiting localhost:3000 in the DuckDuckGo browser on the Raspberry Pi after the following startup command is run. To run this properly, the following command can be run in a bash terminal on the Raspberry Pi in the same directory as “device-example.js”:

`node device-example.js --host-name=HOSTNAME --private-key=PRIVKEY.private.key --client-certificate=CLIENTCERT.cert.pem --ca-certificate=root-CA.crt --client-id=NodeJS --test-mode=1`

These files should have paths leading to them, if they are not in the same directory as the device-example.js file. The following logic diagram shows an example of communication when a user sets an alarm from the node-red UI from their phone. More detail is given later on how to properly execute our clock’s demo.

![Alarm Set Logic](https://github.com/ecenteno7/RPi3-IoT-Alarm-Clock-ECE4180/blob/master/images/alarm%20on.jpg)

As shown, the JavaScript code will trigger the number pad to turn on when a message is published to the topic “sdkTest/sub” indicating that the alarm is now on. The logic diagrams for when the alarm stimuli need to be increased/turned off are found in the next section.

### Node-Red User Interface
Node-Red is a tool that is used to wire together JavaScript functions in order to make more complicated applications. It is built off of Node.JS and has several pre-written blocks of code that make interfacing with one another very simple. Node-Red was chosen to implement the cell phone alarm setting functionality due to the ease of use of adding AWS connection and connection to the cell phone without any native scripts running on it.

To create the user interface, node-red-dashboard was installed into our node-red modules folder, and a “forms” node was added to our flow. This node has all of the inputs that the user must include in order to set the alarm. The output of this node goes into a function which formats the message into the readable string that the C script can read (2_DAY_MONTH_YEAR_HOUR_MINUTE_CODE). The “2” indicates that an alarm is being set, and the C script interprets this. This function node outputs the message to the MQTT publish node, which connects to our AWS IoT Core MQTT Broker and publishes the message to topic “sdkTest/sub.” This node needs all of the similar certificates and keys that the other scripts needed in order to properly connect to AWS. The flow that we used in node-red is shown, followed by the interface a user will see on their cell phone. The “node-red” command can be run from the Raspberry Pi’s terminal to start the server.  To connect to the user interface page, the user can simply connect to the Raspberry Pi’s IP address (on the same wireless network) at port 1880 at the ui directory (ie. 192.168.1.1/ui).

![Node Red](https://github.com/ecenteno7/RPi3-IoT-Alarm-Clock-ECE4180/blob/master/images/node%20red.jpg)
![Alarm Input](https://github.com/ecenteno7/RPi3-IoT-Alarm-Clock-ECE4180/blob/master/images/alarm%20input.jpg)

### C Script 
The driver that sets the alarm and controls the audio lives in our C script. The script is subscribed to the “sdkTest/sub” AWS topic, so every time a MQTT message is published on this topic, the C script will receive it. It begins by iterating through an infinite loop waiting for a message to be published by the Node.JS server. Once a message is received, the C script decodes it and determines what time to set the alarm for. The script also stores the current date and time at each iteration of the while loop, and if an alarm is set, it will compare the alarm time with the current time. If they are a match, the C script publishes a message to AWS “sdkTest/sub” topic indicating that the alarm has been activated.

The C file controls the speaker and NeoPixel functionality as well. Once the message that the alarm has been activated is sent, the script immediately turns on the speakers and plays our Star Wars theme wav file via ALSA (Advanced Linux Sound Architecture) for easy command line manipulation. This is done by running a bash command through C and forcing the process to run in the background (using a system() function call). Once the alarm is on, the script still runs and waits for an encoded message indicating whether the correct code has been input by the user. If the code is incorrect, the volume is turned up on the speakers, and the speakers are turned off when the code is inputted correctly. The process that is playing the sound in the background is killed by using a kill command inside of a system() function call. If the code is correct, the script runs a pkill command using a system() call to kill the background process playing the audio. The ALSA process will loop the audio file if it reaches the end before the alarm is turned off.

For controlling the NeoPixel, the C script will start a Python script that controls the NeoPixel using a similar system() call as the speakers, running it in the background. This script has various parameters for how bright and how frequently the NeoPixel flashes. When the alarm starts, the NeoPixel will flash white about once a second. When the alarm needs to increase the intensity of the lights, it kills the old script with a pkill command and starts a new script with double the frequency. After increasing the frequency four times, the C script will start a new Python script that displays a spinning rainbow pattern on the NeoPixel. Each failed attempt will increase the speed of the spinning, up to four times the original speed. When the alarm is disabled, a final script will run to set the lights to nothing and will kill all the Python scripts with a final pkill command.

![Alarm up](https://github.com/ecenteno7/RPi3-IoT-Alarm-Clock-ECE4180/blob/master/images/alarm%20up.jpg)
![Alarm off](https://github.com/ecenteno7/RPi3-IoT-Alarm-Clock-ECE4180/blob/master/images/alarm%20off.jpg)

### Adafruit 3W Speaker Bonnet
For the speakers for the alarm music, we used the [Adafruit I2S Stereo Speaker Bonnet](https://www.adafruit.com/product/3346). This “bonnet” board covers the GPIO pins on the Raspberry Pi. It controls two speakers using a I2S signal from the Pi.

### Adafruit Neopixel LED Ring 
We used an [Adafruit Neopixel LED Ring](https://www.adafruit.com/product/1586) with 24 RGB LEDs for the light on top of the alarm. We used the NeoPixel ring since each LED is individually addressable and let’s us control color and brightness. We used the [Adafruit CircuitPython Neopixel](https://circuitpython.readthedocs.io/projects/neopixel/en/latest/) library to control the ring with the Raspberry Pi. Unfortunately controlling NeoPixels with a Raspberry Pi is only possible with this Python library, so we created a Python script to connect and communicate with the NeoPixel ring. The script flashes the NeoPixels white at different frequencies or creates a spinning rainbow effect. More details about this can be found above (Demo/C Script). The code for the rainbow effect was adapted from the example code found [here](https://circuitpython.readthedocs.io/projects/neopixel/en/latest/examples.html).

## Wiring
The wiring for the alarm clock is largely simplified by the speaker bonnet. The bonnet connects directly to the Pi’s GPIO pins and has a connector for the speakers used. The speaker bonnet uses GPIO18, GPIO19, and GPIO21 for the audio signal, which means these pins cannot be used for other devices, like the NeoPixel. Both the NeoPixel and touchscreen both require a +5V input for power, which is easily done using the two 5V pins from the Pi. The ground wires for both devices can be connected to any ground pin. Running the Pi from wall power was enough to drive the speakers at full volume, the LEDs at full brightness, and the touchscreen at full brightness at the same time.

The NeoPixel can be controlled by a PWM signal, a PCM signal, or a SPI signal, which means it can only use GPIO pins 12 (PWM), 18 (PWM), 21 (PCM), or 10 (SPI). Since the speaker bonnet was already using 18 and 21, we ended up using GPIO12. Adafruit also recommends using a level shifter to bring the signal on pin 12 from 3.3V to 5V. However, since we were not using a long string of pixels, we did not find it necessary to use one.

![Pins](https://github.com/ecenteno7/RPi3-IoT-Alarm-Clock-ECE4180/blob/master/images/pi-pinout.png)

## Case
We designed a simple case to hold the screen and contain the speakers and NeoPixel ring that could be assembled by anyone with access to a 3D printer and common shop tools. The case mainly consists of polycarbonate panels and a few 3D-printed frame parts. The Raspberry Pi mounts to the back of the touchscreen, which is then attached to four frame parts. These frame parts have grooves to hold the polycarb panels in place. The side panels have mounting holes for the speakers. The top panel has mounting holes for 3D-printed brackets that hold the NeoPixel ring in place. The back panel has a hole for the power cable for the Pi. We cut the panels using a water jet, but they can be made using a saw or laser cutter. We joined all the brackets and panels using small amounts of hot glue in the grooves of the brackets.

![Case](https://github.com/ecenteno7/RPi3-IoT-Alarm-Clock-ECE4180/blob/master/images/labeled-cad.png)

## Demo
To setup the device you first need to do the following:
* Power the device by plugging in the Raspberry Pi to the 5V external power supply
* Navigate to the directory containing device-example.js and run the command:
 `node device-example.js --host-name=HOSTNAME --private-key=PRIVKEY.private.key --client-certificate=CLIENTCERT.cert.pem --ca-   certificate=root-CA.crt --client-id=NodeJS --test-mode=1`
* In a separate terminal, navigate to the directory containing subscribe-publish-sample.c and run the command:
 `./subscribe-publish-sample`
* In a third terminal, run `node-red` to start the node-red server
* With another device, connect to the same WiFi network as the Raspberry Pi
* Open a web browser on the other device and go type the following:
 `<raspberry pi’s IP address>:1880/ui`
* Open a web browser on the Raspberry Pi and navigate to localhost:3000 to display the GUI on the touchscreen

The demo includes a user inputting an alarm time and a corresponding 4-digit code on a cell phone. The alarm activates and starts playing the Star Wars theme. The sound gets louder and the LEDs get brighter after incorrect code inputs. The alarm, sound, and lights all turn off once the correct code is input into the GUI.

Demo link: https://youtu.be/2EGQX7V5hsM
