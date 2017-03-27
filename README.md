# ssd1306_rpi
ssd1306 Command Line Tool for Raspberry Pi / Orange Pi

128 X 64のOLEDをコマンドラインから操作できるツールです。  
SPI/I2Cどちらのインターフェースにも対応しています。  
SPIはハードウェアSPI/ソフトウェアSPIのどちらも使うことができます。
I2CもハードウェアI2C/ソフトウェアI2Cのどちらも使うことができます。

実行時の引数の指定は以下の通りです。  
+1   : 1行目に表示する文字の指定(外部フォントで表示）  
+2   : 2行目に表示する文字の指定(外部フォントで表示）  
+3   : 3行目に表示する文字の指定(外部フォントで表示）  
+4   : 4行目に表示する文字の指定(外部フォントで表示）  
+a   : 1行目に表示する文字の指定(内部フォントで表示）  
+b   : 2行目に表示する文字の指定(内部フォントで表示）  
+c   : 3行目に表示する文字の指定(内部フォントで表示）  
+d   : 4行目に表示する文字の指定(内部フォントで表示）  
-1   : 1行目の文字を削除  
-2   : 2行目の文字を削除  
-3   : 3行目の文字を削除  
-4   : 4行目の文字を削除  
+R n : n行目を反転で表示  
-R n : n行目の反転を解除  
+U n : n行目をアンダーライン付きで表示  
-U n : n行目のアンダーラインを解除  
+L   : 1行分スクロールアップ  
-L   : 1行分スクロールダウン  
P1 n : 1行目の表示開始位置をn文字目に設定  
P2 n : 2行目の表示開始位置をn文字目に設定  
P3 n : 3行目の表示開始位置をn文字目に設定  
P4 n : 4行目の表示開始位置をn文字目に設定  
c    : 全ての文字を削除  
s    : 表示を反映  

---

You can operate from command line.  
You can choose Hardware-SPI/Software-SPI/Hardware-I2C/Software-I2C Interface.  
Software-I2C Libray is here  
https://github.com/electronicayciencia/wPi_soft_i2c  

Command line parameters:  
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
c  : delete all string  
s  : show display  

You can use within script.  
#!/bin/bash  
./oled c  
./oled +1 "ABCDEFG"  
./oled +2 "abcdefg"  
./oled +3 "1234567"  
./oled +4 "Hello World!!"  
sudo ./oled s  

---

Wire connection for Hardware SPI

OLED---Raspberry  
Gnd----Gnd  
VCC----3.3V  
SCL----SCLK(Pin#23)  
SDA----MOSI(Pin#19)  
RST----GPIO2 *  
D/C----GPIO4 *  

(*)You can change any pin.  

---

Wire connection for Software SPI

OLED---Raspberry  
Gnd----Gnd  
VCC----3.3V  
SCL----GPIO11 *  
SDA----GPIO10 *  
RST----GPIO2 *  
D/C----GPIO4 *  

(*)You can change any pin.  

---

Wire connection for Hardware I2C

OLED---Raspberry  
Gnd----Gnd  
VCC----3.3V  
SCK----SCL(Pin#5)  
SDA----SDA(Pin#3)  

---

Wire connection for Software I2C

OLED---Raspberry  
Gnd----Gnd  
VCC----3.3V  
SCK----GPIO8 *  
SDA----GPIO7 *  

(*)You can change any pin.  

---

Install for Hardware SPI  
git clone https://github.com/nopnop2002/ssd1306_rpi.git  
cd ssd1306_rpi/  
cc -o oled oled.c fontx.c -lwiringPi -DSPI  
bash ./test.sh  

---

Install for Software SPI  
git clone https://github.com/nopnop2002/ssd1306_rpi.git  
cd ssd1306_rpi/  
cc -o oled oled.c fontx.c -lwiringPi -DSOFT_SPI  
bash ./test.sh  

---

Install for Hardware I2C  
git clone https://github.com/nopnop2002/ssd1306_rpi.git  
cd ssd1306_rpi/  
cc -o oled oled.c fontx.c -lwiringPi -DI2C  
bash ./test.sh  

---

Install for Software I2C  
git clone https://github.com/nopnop2002/ssd1306_rpi.git  
git clone https://github.com/electronicayciencia/wPi_soft_i2c  
cd ssd1306_rpi/  
cp ../wPi_soft_i2c/soft_i2c.* ./  
cc -o oled oled.c fontx.c soft_i2c.c -lwiringPi -DSOFT_I2C  
bash ./test.sh  

---

![oled-1](https://cloud.githubusercontent.com/assets/6020549/24071131/782737a8-0c0e-11e7-9312-44ec00312f52.JPG)

![oled-2](https://cloud.githubusercontent.com/assets/6020549/24293142/b6cb6334-10d3-11e7-9439-1f6993341606.jpg)

![oled-4](https://cloud.githubusercontent.com/assets/6020549/24076582/e037998c-0c77-11e7-9e48-525e27c478a3.JPG)

Line1-2 : External Font  
Line3-4 : Internal Font  


![oled-5](https://cloud.githubusercontent.com/assets/6020549/24293214/f627fefc-10d3-11e7-9304-bcfd58d59d92.JPG)

---

Set start colum  

![oled-6](https://cloud.githubusercontent.com/assets/6020549/24125179/b3b09bfe-0e0a-11e7-83bc-9dbd7b26db18.JPG)


![oled-7](https://cloud.githubusercontent.com/assets/6020549/24125192/d2c40328-0e0a-11e7-8a6a-884c0600059e.JPG)
