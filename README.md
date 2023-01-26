# DC Motor Control Project
## Final Project for Introduction to Mechatronics

## Overview
This was the final project developed for the ME 333 Introduction to Mechatronics class at Northwestern University during my Master's program.

The purpose was to provide low level current control and upper level position control of a DC motor to drive a laser-cut acryllic propeller according to user input. C was used to program a PIC32 microcontroller to accomplish the task by using UART and I2C communication protocols.

A Raspberry Pi PICO was used to read encoder output from the DC motor using I2C to implement position control. I developed timer interrupt functions to re-caclulate current requirements based on user-set PI gains and position feedback. The output from the position controller was input to the current controller getting feedback from an INA219 current sensor. The current controller determined output PWM with appropriate duty cycles to get to the desired position (rotation angle) as set by the user.

A user interface was developed in Python to allow the user to choose from multiple modes including but not limited to sending a desired position, loading and setting trajectories, manually tuning PI gains, and manually setting PWM duty cycle. See videos below for demonstrations of the user interface and subsequent behavior.

## Video Demonstrations

Demonstration of manually entered PWM duty cycle:

https://user-images.githubusercontent.com/58793794/214755435-ea6e3e60-29b0-498c-be0c-5b86ec19d457.mp4



Demonstration of the estimated effect of user-inputted PI gains on a cubic trajectory:




Demonstration of the effect of user-inputted PI gains on a custom trajectory:
