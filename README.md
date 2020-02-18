# Linear Encoder Midi Controller

High Resolution (5um) TTL Linear Encoder to Send Control Change values from 0-127 based on encoder position and a Force Sensitive Resistor pad to send Note on/off messages with velocity sensing and continuous aftertouch. 
This sketch depends on the MIDIUSB2 library by 'deanm1278' which is a version of the MIDIUSB arduino library but w/ callback and sysex support. 
The encoder I am using has 5 pins, 3 of these are for A/B/Z channels and the other two connect to 5v and Ground.
I have A/B pins attatched to pins 2 & 3 on an arduino micro with interrupts attatched for best performance, I am  yet to implement use of the index(z) channel as there is very limited documentation on using these kinds of encoders and any information on how to apply their use to my particular application is virtually nonexistant. I was also unable to find a data sheet for the encoder I used so it took a lot of guess work to get this up and running.
The linear encoder has a travel length of 400mm however having such a high resolution over such a long scale can cause the arduino to miss a few pulses when the encoder head is travelling at a fast pace, this is where use of the index(z) channel could clean things up a bit and keep track of the zero position. 
As it is now I have managed to get pretty stable readings from 0-127 across the travel length of the encoder however if I push it too fast it is prone to loosing its position and needs to be reset. 
While I'm still working out how to implement the index channel I will eventually add a button to recalibrate the zero position for when the readings go bad. 
I'm not sure if how I am using interrupts in my code is efficient or if how I have written it might be responsible for the missing pulses, if anyone has a solution or advice feel free to open a pull request! 

-Jim Foster
