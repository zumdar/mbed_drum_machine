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
The goal of this project is to create a digital drum machine using circuit components and an Mbed microcontroller. The idea is to use analog circuitry (op amps, transistors, diodes, transistors, resistors, capacitors) to create circuits that produce common drum sounds and activate those circuits using digitalOut pins from the mbed.   

![Final Product](https://github.com/zumdar/mbed_drum_machine/blob/main/3600%20project%20prototype%20.jpeg)

## Components
### Drum Circuits
Analog circuitry is used to make circuits that reproduce the sound of bass drum, snare drum, closed hi-hat drum and open hi-hat drum. 

![Drums Schematic](https://github.com/zumdar/mbed_drum_machine/blob/main/drum%20circuits%20schematic.jpg)


They are activated by four pushbuttons, with one button to play each sound. There are also buttons for recording drum sequences, playing them back and a button for changing the tempo. 

![Drum Machine Schematic](https://github.com/zumdar/mbed_drum_machine/blob/main/system%20schematic.jpg)

Additionally, A PCB board was printed for the entire system

![PCB Schematic](https://github.com/zumdar/mbed_drum_machine/blob/main/board_rev1.png)

And here is the physical board with components 

![PCB irl](https://github.com/zumdar/mbed_drum_machine/blob/main/ece%203600%20project%20PCB%20with%20components%20.jpeg)

## MBED 
The mbed microcontroller was used to setup the buttons to activate the drum circuits and run the software for the overall system. It consists of a main while loop that checks if the buttons are being pressed to play the drum noises and additional code for several modes including a record mode, playback mode, and tempo mode. 

### RecordMode 
After the record button is pressed, the mbed records the drums being hit by the user and begins overdubbing (playing them back in sequence) after the first drum is hit. It is able to record up to one hit per drum during each set time period called a step(set by the tempo, 0.5 seconds by default). And it records a number of steps equal to the sequence length(currently 8) before it resets to the beginning of the sequence and overdubs the sequence recorded while recording any beats to add to the sequence. Hitting the record button again leaves the RecordMode 

### PlaybackMode 
After the playback button is pressed, the mbed continuously loops through the last sequence recorded. Normal drum functionality is preserved so the user can still make new drum sounds to produce music while the sequence is being played back

### TapTempoMode
After the user presses the tempo button four times, it records the average time between each press and uses that as the new time increment allocated for each step. This means if the PlaybackMode is on while the tap button is pressed, it averages it with the playback tempo and plays back the recorded sequence faster or slower based on if the user taps faster or slower than the current tempo. If the user keeps pressing the tempo button, the program adjusts the tempo accordingly.


## How to run it 
Copy code from mbed_bicycle_drum_machine.cpp onto the main file for an empty mbed compiler project. Add Pindetech.h header file to the project and compile onto the mbed. 


### Link to demo 
https://drive.google.com/file/d/1pjsoj4m6NsC67ci9VwtBcKR3x_5BQM0i/view?usp=sharing

