# Raspberry-ssd1306
ssd1306 Command Line Tool for RaspberryPi.   
It also works with OrangePi / NanoPi.   

You can operate from command line.  
You can choose Hardware-SPI/Software-SPI/I2C Interface.  

# Software requirement   
- WiringPi Library   
 This project uses the wiringPiSetup() function to initialize GPIOs.   
 If you use it on a board other than the RPI board, you may need to change the WiringPi number.   
 As far as I know, there are these libraries.   
	- WiringPi for OrangePi   
	- WiringPi for BananaPi   
	- WiringPi for NanoPi   
	- WiringPi for Pine-64   

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

|OLED||RPi|Pin#|WiringPi#|
|:-:|:-:|:-:|:-:|:-:|
|Gnd|--|Gnd|||
|VCC|--|3.3V|||
|D0|--|SCLK|#23||
|D1|--|MOSI|#19||
|RES|--|GPIO18|#12|1(*)|
|DC|--|GPIO17|#11|0(*)|
|CS|--|CS0|#24||

(*)You can change to another pin.   
#define RST  1  // You can change   
#define DC   0  // You can change   

---

# Wire connection for Software SPI

|OLED||RPi|Pin#|WiringPi#|
|:-:|:-:|:-:|:-:|:-:|
|Gnd|--|Gnd||||
|VCC|--|3.3V|||
|D0|--|SCLK|#23|14(*)|
|D1|--|MOSI|#19|12(*)|
|RES|--|GPIO18|#12|1(*)|
|DC|--|GPIO17|#11|0(*)|
|CS|--|CS0|#24|10(*)|

(*)You can change to another pin.   
#define MOSI 12 // You can change   
#define SCLK 14 // You can change   
#define RST   1 // You can change   
#define DC    0 // You can change   
#define CS   10 // You can change   

---

# Wire connection for I2C

|OLED||RPi|Pin#|
|:-:|:-:|:-:|:-:|
|Gnd|--|Gnd||
|VCC|--|3.3V||
|SCK|--|SCL|#5|
|SDA|--|SDA|#3|

---

# Software requirement

WiringPi Library   

---

# Install SSD1306 for Hardware SPI (128x64)
```
git clone https://github.com/nopnop2002/Raspberry-ssd1306
cd Raspberry-ssd1306
cc -o oled oled.c fontx.c -lwiringPi -lpthread -DSPI
bash ./test.sh
```

![OLED-128x64](https://user-images.githubusercontent.com/6020549/106347419-a4723f80-6301-11eb-97cd-1f8311c0e179.JPG)

---

# Install SSD1306 for Hardware SPI (128x32)
```
git clone https://github.com/nopnop2002/Raspberry-ssd1306
cd Raspberry-ssd1306
cc -o oled oled.c fontx.c -lwiringPi -lpthread -DSPI -DX32
```

The number of lines which can be indicated is only 2 lines.   
![OLED-128x32](https://user-images.githubusercontent.com/6020549/106347297-cdde9b80-6300-11eb-9646-a7bd3b9ea466.JPG)

---

# Install SH1106 for Hardware SPI (128x64)
```
git clone https://github.com/nopnop2002/Raspberry-ssd1306
cd Raspberry-ssd1306
cc -o oled oled.c fontx.c -lwiringPi -lpthread -DSPI -DOFFSET=2
bash ./test.sh
```

Left:1.3 inch SH1106   
Right:0.96 inch SSD1306   
![OLED-SH1106](https://user-images.githubusercontent.com/6020549/106369496-5c046180-6395-11eb-8f36-376474120047.JPG)

---

# Install SSD1306 for Software SPI (128x64)
```
git clone https://github.com/nopnop2002/Raspberry-ssd1306
cd Raspberry-ssd1306
cc -o oled oled.c fontx.c -lwiringPi -lpthread -DSOFT_SPI
bash ./test.sh
```

---

# Install SSD1306 for Software SPI (128x32)
```
git clone https://github.com/nopnop2002/Raspberry-ssd1306
cd Raspberry-ssd1306
cc -o oled oled.c fontx.c -lwiringPi -lpthread -DSOFT_SPI -DX32
```

---

# Install SH1106 for Software SPI (128x64)
```
git clone https://github.com/nopnop2002/Raspberry-ssd1306
cd Raspberry-ssd1306
cc -o oled oled.c fontx.c -lwiringPi -lpthread -DSOFT_SPI -DOFFSET=2
bash ./test.sh
```


---

# Install SSD1306 for I2C (128x64)
```
git clone https://github.com/nopnop2002/Raspberry-ssd1306
cd Raspberry-ssd1306
cc -o oled oled.c fontx.c -lwiringPi -lpthread -DI2C
bash ./test.sh
```

![OLED-1](https://user-images.githubusercontent.com/6020549/82557232-50c22a80-9ba6-11ea-8636-b00e0a950b0b.JPG)

---

# Install SSD1306 for I2C (128x32)
```
git clone https://github.com/nopnop2002/Raspberry-ssd1306
cd Raspberry-ssd1306
cc -o oled oled.c fontx.c -lwiringPi -lpthread -DI2C -DX32
```

The number of lines which can be indicated is only 2 lines.   
![oled-3](https://user-images.githubusercontent.com/6020549/44244307-4a06c680-a20e-11e8-986f-6d0db5edeb2b.JPG)

---

# Install SH1106 for I2C (128x64)
```
git clone https://github.com/nopnop2002/Raspberry-ssd1306
cd Raspberry-ssd1306
cc -o oled oled.c fontx.c -lwiringPi -lpthread -DI2C -DOFFSET=2
bash ./test.sh
```

Left:1.3 inch SH1106   
Right:0.96 inch SSD1306   
![OLED-SH1106](https://user-images.githubusercontent.com/6020549/106369513-8b1ad300-6395-11eb-9653-26b470829de2.JPG)

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

# Font File Editor(FONTX Editor)   
[There](http://elm-chan.org/fsw/fontxedit.zip) is a font file editor.   
This can be done on Windows 10.   
Developer page is [here](http://elm-chan.org/fsw_e.html).   

![FontxEditor](https://user-images.githubusercontent.com/6020549/78731275-3b889800-797a-11ea-81ba-096dbf07c4b8.png)


This tool uses the following as default fonts:   
- fontx/ILGH16XB.FNT // 8x16Dot Gothic
- fontx/ILMH16XB.FNT // 8x16Dot Mincyo

Changing this file will change the font.

---

Left  : OrangePi + SPI Module  
Right : RaspberryPi + I2C Module  
![oled-2](https://user-images.githubusercontent.com/6020549/28252021-b1a9ff4a-6ac5-11e7-9265-b757e4b3cf1d.JPG)

---

![OLED-1](https://user-images.githubusercontent.com/6020549/82556597-07251000-9ba5-11ea-9658-84e21e635273.JPG)

![OLED-2](https://user-images.githubusercontent.com/6020549/82556776-5f5c1200-9ba5-11ea-914e-e94744ceebf9.jpg)

Line1-2 : External Font  
Line3-4 : Internal Font  
![OLED-4](https://user-images.githubusercontent.com/6020549/82556641-1efc9400-9ba5-11ea-9928-5b3f3988cc5f.JPG)

Underline and Invert   
![OLED-5](https://user-images.githubusercontent.com/6020549/82556679-2de34680-9ba5-11ea-9f9b-1314b84069c0.JPG)

Set start colum  
![OLED-6](https://user-images.githubusercontent.com/6020549/82556897-9c280900-9ba5-11ea-9c0b-b341931e9a98.JPG)

![OLED-7](https://user-images.githubusercontent.com/6020549/82556916-a518da80-9ba5-11ea-95c2-627ec1b96d8f.JPG)

---

RaspberryPi ZERO HAT + OLED   
![RaspberryPi-ZERO](https://user-images.githubusercontent.com/6020549/67628271-a438f800-f8a5-11e9-8af8-99964ebaae8e.JPG)

OrangePi ZERO HAT + OLED  
![zero-hat-7](https://user-images.githubusercontent.com/6020549/28547249-00f71026-7109-11e7-9a11-0bbb95423381.JPG)
