# Motor Controller Boards
There are two Motor Controller boards, one is in charge receiving commands from the Signal Distribution board, process the commands, and send the required PWM signals to the other second controller board (the analog/power side). The second controller board is in charge of receving the PWM signals and send the power to the motors. 

I have uploaded the complet project to hackster.io: https://www.hackster.io/ederfernandotorres3/precision-motor-controller-with-lm629-and-pic-mcu-a708f2

The main components of this module are: a microcontroller (PIC18F46K22), precission motor controllers (TI's LM629), H-Bridges (TI's LMD18200T) that can take sign-magnitude signals (one pin determines direction of motion and the other pin send a PWM wave), and motors with quadrature encoders. 

The heart of this module is the LM629 IC, this precission motor controller contains an internal PID controller and takes commands from a microcontroller. The microcontrollers sets up the LM629 by sending the Kp, Ki, and Kd coefficients and telling it in which mode it will be running (absolute or relative velocity for example or maybe just trajectory). The the microcontroller will send a motion command through parallel communication by specifying various motion characteristics such as acceleration, max speed, direction and distance. The motor controller will send motion signals in the form of sign-magnitude to an H-Bridge. The H-Bridge then powers the motor and the motion starts. The motors start sending the quadrature encoder signals back to the LM629. The LM629 compares the frequency and the phase of the two square waves and adjusts its output (sign-magnitude) to accomplish the trajectory set by the microcontroller.

Both circuit are electrically isolated to keep the noise from the motors from reaching all the digital side microcontrollers and processors. This is accomplished by using TI's ISO7220 digital isolators. Optocouplers were not using as in the Servo Controller board since they couldn't handle the higher frequency produced by the LM629.

In this revision of the digital board, the SPI bus wires are soldered directly to the PCB pins. The uploaded PDF contains a new added connector. 

### Dataflow between both Motor Controller Boards
<img src="https://user-images.githubusercontent.com/86902176/210906091-4052b8e8-73b1-4b76-b83f-b4c0d930148b.png"  width="400">

### Board 1 - Microcontroller and speed controllers - Not most recent revision
<img src="https://user-images.githubusercontent.com/86902176/210906740-d6bb6d22-972f-4d89-a026-f3c1677c3507.jpg" width="400">

### Board 2 - H-Bridges with digital isolators  - Most recent revision
<img src="https://user-images.githubusercontent.com/86902176/210906843-c741019b-2faa-4a23-9576-937da54c3929.jpg" width="400">

### Both Board 1 and Board 2 connected and stacked together
<img src="https://user-images.githubusercontent.com/86902176/210906936-614f232c-5f31-4c07-a582-80629d36594d.jpg" width="400">
