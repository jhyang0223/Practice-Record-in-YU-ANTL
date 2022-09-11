# Linux-Character-Device
## Description
I implemented exercise example for graduate-level linux kernel programming class.  
(PPT in this directory is used for a portion of the lecture material)  
I used Raspberry PI

## Simple GPIO Character Device Driver
This Simple_gpiodev.c module uses only two pins. 
- Input pin is BCM 17 pin.
- Output pin is BCM 27 pin.

The gpio pins initialization is happened at module open or init.
- When write() function is called, the gpio pins initialization is happened.

When input pin value is 1 to 0, interrupt is generated.
- interrupt handler function has
  - output value toggle.
  - sending MY_SIGNAL(Signal Number is 34).
  
## GPIO Simple IOCTL
This code is Simple GPIO Character Device Driver example using IOCTL
