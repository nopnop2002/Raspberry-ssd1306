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
./oled +1 "Hello World!"
./oled +2 "Hello World!"
./oled +3 "Hello World!"
./oled +4 "Hello World!"
sudo ./oled s
echo -n "push Enter"
read input

./oled P2 2
./oled P3 3
./oled P4 4
sudo ./oled s
echo -n "push Enter"
read input

./oled c
./oled +1 "Line 4"
./oled +2 "Line 3"
./oled +3 "Line 2"
./oled +4 "Line 1"
sudo ./oled s

for i in {5..20}
do
  ./oled -L
  cmd="Line "$i
  ./oled +1 "$cmd"
  sleep 1
  sudo ./oled s
done
echo -n "push Enter"
read input

./oled c
./oled +1 "Line 1"
./oled +2 "Line 2"
./oled +3 "Line 3"
./oled +4 "Line 4"
sudo ./oled s

for i in {5..20}
do
  ./oled +L
  cmd="Line "$i
  ./oled +4 "$cmd"
  sleep 1
  sudo ./oled s
done
echo -n "push Enter"
read input

./oled c
./oled +2 "Thank you!!"
./oled P2 3
./oled +R 2
sudo ./oled s
