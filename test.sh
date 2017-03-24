#!/bin/bash
./oled c
./oled +1 "ABCDEFG"
./oled +2 "abcdefg"
./oled +3 "1234567"
./oled +4 "Hello World!!"
sudo ./oled s
echo -n "push Enter"
read input

./oled +R 1
./oled +U 3
sudo ./oled s
echo -n "push Enter"
read input

./oled c
./oled +1 Hello World!
./oled +2 Hello World!
./oled +3 Hello World!
./oled +4 Hello World!
sudo ./oled s
echo -n "push Enter"
read input

./oled P2 2
./oled P3 3
./oled P4 4
sudo ./oled s
