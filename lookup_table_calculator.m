clear all
clc

%This scripts converts desired angular speed from RPM to counts/sample.
%then it scales it and rounds it to be understood by the LM629 motor controller

%Pololu RPM @ 12V = 350RPM
%Encoder lines per shaft revolution = 64
%Encoder lines per gearbox output revolution = 64*29 = 1856
%Encoder lines with per resolution of LM629 = 1856*4 = 74240
%LM629 Clock Frequency = 4MHz
%Sample time = 2048/Fclock
%LM629 PWM Resolution = 8-bits = 256 steps

%CONSTANTS

%--LM629 Frequency
LM629_freq = 4000000

%--Encoder lines per shaft Revolution
enc_lines_motor_shaft = 64

%--Encoder lines per gearbox output revolution
enc_lines_gearbox_output = enc_lines_motor_shaft * 29

%--LM629 Resolution multiplier (As per datasheet)
resolution = 4

%--Encoder lines with * Resolution
R = enc_lines_gearbox_output * resolution

%--Sample time (as per datasheet)
T = 2048/LM629_freq

%conversion factor (minutes:seconds)
C = 1/60



rpm_per_pwm_step = 40/255
pwm_duty_per_analog_step_positive = 255/15
pwm_duty_per_analog_step_negative = 255/16

a = 16:-1:9
b = a - 8
c = 0 : 7
d = c + 8
a = a * pwm_duty_per_analog_step_negative
b = b * pwm_duty_per_analog_step_negative
c = c * pwm_duty_per_analog_step_positive
d = d * pwm_duty_per_analog_step_positive

desired_rpm = [a;b;c;d]
desired_rpm = desired_rpm * rpm_per_pwm_step
%Velocity in counts/sample units
V = R * T * C * desired_rpm

%Velocity needs to be scaled in order to be entered in the trajectory profile for the LM629\
V_scaled = V * 65536

%LM629 only takes integers
V_rounded = round(V_scaled)




