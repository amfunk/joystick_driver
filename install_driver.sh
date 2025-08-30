#!/bin/bash

make all

sudo rmmod joystick
sudo insmod joystick.ko

make clean
