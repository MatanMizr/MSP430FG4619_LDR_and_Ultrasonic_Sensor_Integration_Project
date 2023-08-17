# MSP430FG4619_LDR_and_Ultrasonic_Sensor_Integration_Project
Light Source and Objects Proximity Detector System – the project structured as a finite state machine. The system comprises several states: 
1.	Sleep Mode (state 0): the system is idle and conserving energy. 
2.	Objects Detector (state1): the system scans a 180-degree range, identifies objects presence, and display its findings to the user. 
3.	Precise Objects check (state2): The system moves to a specific angle, verifies object presence, and display its finding to the user. 
4.	Light Source Detection (state3): A scan identifies light source and showcases their locations to the user. 
5.	Light sources and Objects Detection (state4): a scan identifies both light sources and objects, displaying the results to the user. 
6.	Script Execution (state5): The system enters script mode, allowing programmed tasks to be stored in the MCU flash memory and to be executed by the MCU. 
Each state is triggered by the user.
Through this design, the system can effectively detect objects and light sources while allowing for customizable scripting.
For this project we used MSP430FG4619 microcontroller, the code was written in C language in CCS (Code Composer Studio) IDE. For the PC side of this project we used Python and GUI for the user’s interface. 

# Libraries included: 
MCU SIDE: 
• “stdio.h ”
• “string.h”
• “stdint.h”
• “msp430FG4619.h”
PC SIDE:
• “serial”
• “1me”
• “PySimpleGUI”
• “numpy”
• “matplotlib.pyplot”
• “os”
Packages:
• Pyserial
• Pysimplegui
• Pyautogui
• Numpy
• Matplotlib

# LCD configuration:
Data0 – Data3 -> P1.4 – P1.7
LCD control:
E -> P2.5
RS -> P2.6
RW -> P2.7
Pushbu5on: 
PB2 -> P1.0 – input 
Ultrasonic:
Trigger leg -> P2.1
Echo leg -> P2.0
GND -> DC voltage supplier GND
VCC -> DC voltage supplier VCC
Servo:
Signal leg -> DAC0 leg in MSP430FG4619
GND -> DC voltage supplier GND
VCC -> DC voltage supplier VCC
LDR:
LDR1 (ADC sample) -> P6.3 (channel A3 in the MCU)
LDR2 (ADC sample) -> P6.4 (channel A4 in the MCU)
DC voltage supplier:
GND -> MCU GND.

# Hardware Connections
![image](https://github.com/MatanMizr/MSP430FG4619_LDR_and_Ultrasonic_Sensor_Integration_Project/assets/106826633/5b145782-cb0a-4269-8cf2-010b577c1746)

# Explanations
# Timers
The MSP430FG4619 MCU comes with two Timers, TimerA and TimerB, which can be used to take Xme 
measurements. The registers TAR for TimerA and TBR for TimerB gives the Timer’s current count. 
There are two modes that the Timers can be in: Compare Mode and Capture Mode, Moreover, TimerB have 
seven Capture\Compare registers and TimerA have three Capture\Compare registers, which gives the Timers 
the capabiliXes to support mulXple Capture\Compare, PWM outputs and interval Xming. Also, the Timers has 
extensive interrupt capabiliXes. 
Capture Mode:
Capture Mode is used to record Xme events. It can be used for speed computaXons or Xme measurements.
Compare Mode:
Compare Mode is used to generate PWM signals or interrupts at specific Xme intervals.

# UART
The MSP430FG4619 comes with the universal synchronous\asynchronous receive\transmit (USART) 
peripheral interface that support two serial modes with one hardware module.
The UART module, which we use in this project, in the MSP430FG4619 MCU allows for asynchronous serial 
communicaXon, which means data is transmiaed without a common clock signal between the sender and the 
receiver. This is a parXcularly useful for interconnecXng devices that might operate at different clock speeds 
or in scenarios where precise synchronizaXon is not required. 
Our UART configuraXon for this project includes: 
• Baud rate: 9600 BPS (Bits Per Second)
• Data Size: 8 – bits
• 1 start bit
• 1 stop bit
• No Parity bit

# HC-SR04 Ultrasonic sensor
The HC-SR04 ultrasonic sensor is a widely used distance measuring module. It operates by applying at least 10microseconds HIGH signals at the Trigger leg, it emits eight 40kHz pulses that hit the object and bounce back to the sensor, the Echo pin emits a HIGH signal that his width indicates the time between the pulses were sent and the when the reflected pulses are detected. This time delay is used to calculate the distance between the sensor and the object.
Distance Calculation: 
Range[cm] ≅ EchoPulseTime×(34,000/2) [cm/sec] = EchoPulseTime × 17,000[cm/sec], when 34,000[cm/sec] is the speed of sound c.
To be more precise, the speed of sound is depended in the temperature, assuming we are inside a lab and the temperature is 25 Celsius then
c[m/sec] = 331.3 + 0.606 × TemperatueInCelsius = 346.45[m/sec] = 34,645[cm/sec]
Range[cm] = EchoPulseTime × 17,322.5

In order to calculate the Echo Pulse Time, we used the Input Capture mode in TimerA at layer 3, we configured this register to raise the interrupt flag when there is a raising or failing edge. In the interrupt vector we placed the value that was captured in the TimerA inside two variables, one variable for falling edge and the other for raising edge. By subtract one variable from the other we know how many MCU clocks were between the raising edge and the falling edge, TimerA counts in 2^17[Hz]  so the Echo Pulse Time will be: 
EchoPulseTime = Number of clock × (1/2^17) 

![image](https://github.com/MatanMizr/MSP430FG4619_LDR_and_Ultrasonic_Sensor_Integration_Project/assets/106826633/cf99315a-8750-49da-896c-3066be202293)

# Servo Motor
A servo motor is a small electromechanical device utilized for controlling precise angular positions. It responds to input PWM signals (Max frequency = 50[Hz]), and adjusts its shaft to a designated angle. By adjusting the duration of the High-level signal within the PWM, the motor’s shaft is positioned at a specific angle. For out project’s 180-degree scan, a PWM signal at 50Hz with a High-level time of 0.48msec corresponds to the angle 0 degrees, while a High-level of 2.1msec achieves 180 degrees. The relationship between angle “R” and High-level time is given by the linear transformation: 
T_on (R) = 0.0089 × R + 0.48
![image](https://github.com/MatanMizr/MSP430FG4619_LDR_and_Ultrasonic_Sensor_Integration_Project/assets/106826633/dc3be44a-6b20-4df0-8403-3badb5f71b7e)

#LDR
An LDR, or Light Dependent Resistor, is a type of resistor whose resistance varies based on the intensity of light it is exposed to. As light increases, the resistance of the LDR decreases, and vice versa. In our project we used two LDR, placed them on each side of the ultrasonic sensor and used them to detect light sources in the area. In the MSP430FG4619 there is a 12-bit ADC peripheral module, we use him to sample the voltage that drops on the LDR, as the resistance decreases so as the voltage, and from the voltage sampled we can know the distance of the light source from the LDRs. 
In order to determine the specific distance of the light source we first need to calibrate the LDRs data to the specific light source and specific area of the scan. 
![image](https://github.com/MatanMizr/MSP430FG4619_LDR_and_Ultrasonic_Sensor_Integration_Project/assets/106826633/abec348b-c180-49c7-9668-c09fa6b99e60)

# The MSP430FG4619 we used 
![image](https://github.com/MatanMizr/MSP430FG4619_LDR_and_Ultrasonic_Sensor_Integration_Project/assets/106826633/64463452-f2db-4c25-a577-5d3aa3afaf18)
![image](https://github.com/MatanMizr/MSP430FG4619_LDR_and_Ultrasonic_Sensor_Integration_Project/assets/106826633/8fc0c0b8-6dc2-418f-b5f6-f4d1b5879234)
![image](https://github.com/MatanMizr/MSP430FG4619_LDR_and_Ultrasonic_Sensor_Integration_Project/assets/106826633/f8a068fd-b2de-4c0c-986d-38d8624b67e8)
![image](https://github.com/MatanMizr/MSP430FG4619_LDR_and_Ultrasonic_Sensor_Integration_Project/assets/106826633/8293b815-5144-4e30-bb49-b8d1f0878a32)
![image](https://github.com/MatanMizr/MSP430FG4619_LDR_and_Ultrasonic_Sensor_Integration_Project/assets/106826633/5a91ef75-6308-4845-a8fe-7f1d0fd969c8)
![image](https://github.com/MatanMizr/MSP430FG4619_LDR_and_Ultrasonic_Sensor_Integration_Project/assets/106826633/639a6273-16da-40a6-9260-d5f57d6609bd)

# Examples from the project
![image](https://github.com/MatanMizr/MSP430FG4619_LDR_and_Ultrasonic_Sensor_Integration_Project/assets/106826633/7f3828d9-c347-411b-a1c0-405762aaba67)
![image](https://github.com/MatanMizr/MSP430FG4619_LDR_and_Ultrasonic_Sensor_Integration_Project/assets/106826633/428cf45c-26fe-4638-93de-43a8d2329919)
Objects Detector:
![image](https://github.com/MatanMizr/MSP430FG4619_LDR_and_Ultrasonic_Sensor_Integration_Project/assets/106826633/3cc55d18-fbd4-41ed-ad14-4177005301db)
![image](https://github.com/MatanMizr/MSP430FG4619_LDR_and_Ultrasonic_Sensor_Integration_Project/assets/106826633/f3bc84a1-808a-4e24-94d6-ad50a17030b4)

Application:
![image](https://github.com/MatanMizr/MSP430FG4619_LDR_and_Ultrasonic_Sensor_Integration_Project/assets/106826633/8f121b7e-2f7d-4d6c-aaaa-a14e6de71f89)
![image](https://github.com/MatanMizr/MSP430FG4619_LDR_and_Ultrasonic_Sensor_Integration_Project/assets/106826633/2d68f96c-fdc1-4c2b-b89f-cad89360fcb0)
![image](https://github.com/MatanMizr/MSP430FG4619_LDR_and_Ultrasonic_Sensor_Integration_Project/assets/106826633/54db5aac-d3bf-4a70-a89e-6820b85c5591)
![image](https://github.com/MatanMizr/MSP430FG4619_LDR_and_Ultrasonic_Sensor_Integration_Project/assets/106826633/ee9550e7-96ee-43c1-85d0-33ce181d037f)
![image](https://github.com/MatanMizr/MSP430FG4619_LDR_and_Ultrasonic_Sensor_Integration_Project/assets/106826633/bfd42e7b-7b30-4cd2-84c0-4aa2149413aa)

Script mode:
![image](https://github.com/MatanMizr/MSP430FG4619_LDR_and_Ultrasonic_Sensor_Integration_Project/assets/106826633/d0a64c1b-51ba-46c7-ac06-51d3fbedcf6a)
![image](https://github.com/MatanMizr/MSP430FG4619_LDR_and_Ultrasonic_Sensor_Integration_Project/assets/106826633/d4bad98f-97a7-45e9-8d63-214d672f7501)
















