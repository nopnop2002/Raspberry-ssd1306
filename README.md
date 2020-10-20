# Raspberry-ssd1306
ssd1306 Command Line Tool for RaspberryPi / OrangePi

You can operate from command line.  
You can choose Hardware-SPI/Software-SPI/I2C Interface.  

# Command line parameters
+1 String : String for #1 line(with External Font)  
+2 String : String for #2 line(with External Font)  
+3 String : String for #3 line(with External Font)  
+4 String : String for #4 line(with External Font)  
+a String : String for #1 line(with Internal Font)  
+b String : String for #2 line(with Internal Font)  
+c String : String for #3 line(with Internal Font)  
+d String : String for #4 line(with Internal Font)  
-1 : delete #1 line  
-2 : delete #2 line  
-3 : delete #3 line  
-4 : delete #4 line  
+R n : Set inverse mode #n Line  
-R n : Unset inverse mode #n Line  
+U n : Set underline mode #n Line  
-U n : Unset underline mode #n Line  
+L   : Scroll Up 1Line  
-L   : Scroll Down 1Line  
P1 n : Set start colum n to line#1  
P2 n : Set start colum n to line#2  
P3 n : Set start colum n to line#3  
P4 n : Set start colum n to line#4  
r  : remove all string  
s  : show display  

You can use within script.  
#!/bin/bash  
./oled r  
./oled +1 "ABCDEFG"  
./oled +2 "abcdefg"  
./oled +3 "1234567"  
./oled +4 "Hello World!!"  
sudo ./oled s  

---

# Wire connection for Hardware SPI

|OLED||RPi/OPi|
|:-:|:-:|:-:|
|Gnd|--|Gnd|
|VCC|--|3.3V|
|D0|--|SCLK(Pin#23)|
|D1|--|MOSI(Pin#19)|
|RES|--|GPIO2(Pin#3) (*)|
|DC|--|GPIO4(Pin#7) (*)|
|CS|--|CS0(Pin#24)|

(*)You can change to another pin.   
#define RST  8  // You can change   
#define DC   7  // You can change   

---

# Wire connection for Software SPI

|OLED||RPi/OPi|
|:-:|:-:|:-:|
|Gnd|--|Gnd|
|VCC|--|3.3V|
|D0|--|GPIO11(Pin#23) (*)|
|D1|--|GPIO10(Pin#19) (*)|
|RES|--|GPIO2(Pin#3) (*)|
|DC|--|GPIO4(Pin#7) (*)|
|CS|--|GPIO8(Pin#24) (*)|

(*)You can change to another pin.   
#define MOSI 12 // You can change   
#define SCLK 14 // You can change   
#define RST   8 // You can change   
#define DC    7 // You can change   
#define CS   10 // You can change   

---

# Wire connection for I2C

|OLED||RPi/OPi|
|:-:|:-:|:-:|
|Gnd|--|Gnd|
|VCC|--|3.3V|
|SCK|--|SCL(Pin#5)|
|SDA|--|SDA(Pin#3)|

---

# Software requirement

WiringPi Library   

---

# Install for Hardware SPI
```
git clone https://github.com/nopnop2002/Raspberry-ssd1306
cd Raspberry-ssd1306
cc -o oled oled.c fontx.c -lwiringPi -lpthread -DSPI
bash ./test.sh
```

---

# Install for Software SPI
```
git clone https://github.com/nopnop2002/Raspberry-ssd1306
cd Raspberry-ssd1306
cc -o oled oled.c fontx.c -lwiringPi -lpthread -DSOFT_SPI
bash ./test.sh
```

---

# Install for I2C (128X64)
```
git clone https://github.com/nopnop2002/Raspberry-ssd1306
cd Raspberry-ssd1306
cc -o oled oled.c fontx.c -lwiringPi -lpthread -DI2C
bash ./test.sh
```

---

# Install for I2C (128X32)
```
git clone https://github.com/nopnop2002/Raspberry-ssd1306
cd Raspberry-ssd1306
cc -o oled oled.c fontx.c -lwiringPi -lpthread -DI2C -DX32
```

The number of lines which can be indicated is only 2 lines.   

---

# Change font style   
You can choose the font style.   
Please change here.   

```
  // You can change font file
  Fontx_init(fx,"./fontx/ILGH16XB.FNT","./fontx/ILGZ16XB.FNT"); // 16Dot Gothic
//  Fontx_init(fx,"./fontx/ILMH16XB.FNT","./fontx/ILMZ16XB.FNT"); // 16Dot Mincyo
```

---

# Font File   
You can add your original fonts.   
The format of the font file is the FONTX format.   
Your font file is put in fontx directory.   

Please refer [this](http://elm-chan.org/docs/dosv/fontx_e.html) page about FONTX format.   

---

# FONTX Editor   
[There](http://elm-chan.org/fsw/fontxedit.zip) is a font file Editor.   
This can be done on Windows 10.   
Developer page is [here](http://elm-chan.org/fsw_e.html).   

![FontxEditor](https://user-images.githubusercontent.com/6020549/78731275-3b889800-797a-11ea-81ba-096dbf07c4b8.png)


This tool uses the following as default fonts:   
- font/ILGH16XB.FNT // 8x16Dot Gothic
- font/ILMH16XB.FNT // 8x16Dot Mincyo

Changing this file will change the font.

---

Left  : OrangePi + SPI Module  
Right : RaspberryPi + I2C Module  

![oled-2](https://user-images.githubusercontent.com/6020549/28252021-b1a9ff4a-6ac5-11e7-9265-b757e4b3cf1d.JPG)

---

![OLED-1](https://user-images.githubusercontent.com/6020549/82556597-07251000-9ba5-11ea-9658-84e21e635273.JPG)

![OLED-2](https://user-images.githubusercontent.com/6020549/82556776-5f5c1200-9ba5-11ea-914e-e94744ceebf9.jpg)

![OLED-4](https://user-images.githubusercontent.com/6020549/82556641-1efc9400-9ba5-11ea-9928-5b3f3988cc5f.JPG)

Line1-2 : External Font  
Line3-4 : Internal Font  

![OLED-5](https://user-images.githubusercontent.com/6020549/82556679-2de34680-9ba5-11ea-9f9b-1314b84069c0.JPG)

---

Set start colum  
![OLED-6](https://user-images.githubusercontent.com/6020549/82556897-9c280900-9ba5-11ea-9c0b-b341931e9a98.JPG)

![OLED-7](https://user-images.githubusercontent.com/6020549/82556916-a518da80-9ba5-11ea-95c2-627ec1b96d8f.JPG)

---

128 X 64 i2c Module
![OLED-1](https://user-images.githubusercontent.com/6020549/82557232-50c22a80-9ba6-11ea-8636-b00e0a950b0b.JPG)

128 X 32 i2c Module   
![oled-3](https://user-images.githubusercontent.com/6020549/44244307-4a06c680-a20e-11e8-986f-6d0db5edeb2b.JPG)

---

RaspberryPi ZERO HAT + OLED   
![RaspberryPi-ZERO](https://user-images.githubusercontent.com/6020549/67628271-a438f800-f8a5-11e9-8af8-99964ebaae8e.JPG)

OrangePi ZERO HAT + OLED  
![zero-hat-7](https://user-images.githubusercontent.com/6020549/28547249-00f71026-7109-11e7-9a11-0bbb95423381.JPG)
