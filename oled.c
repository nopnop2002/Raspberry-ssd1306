/*
 SSD1606 example

 hardware spi interface
 cc -o oled oled.c fontx.c -lwiringPi -DSPI
 SSD1606   RPI
 --------------
 GND    --- Gnd
 VCC    --- 3.3V
 DO     --- SCLK(Pin#23)
 DI     --- MOSI(Pin#19)
 RES    --- GPIO18(Pin#12) (You can use Any Pin)
 DC     --- GPIO17(Pin#11) (You can use Any Pin)
 CS     --- CS0(Pin#24)

 software spi interface
 cc -o oled oled.c fontx.c -lwiringPi -DSOFT_SPI
 SSD1606   RPI
 --------------
 GND    --- Gnd
 VCC    --- 3.3V
 DO     --- SCLK(Pin#23) (You can use Any Pin)
 DI     --- MOSI(Pin#19) (You can use Any Pin)
 RES    --- GPIO18(Pin#12) (You can use Any Pin)
 DC     --- GPIO17(Pin#11) (You can use Any Pin)
 CS     --- GPIO8(Pin#24) (You can use Any Pin)

 i2c interface
 cc -o oled oled.c fontx.c -lwiringPi -DI2C
 SSD1606   RPI
 --------------
 GND    --- Gnd
 VCC    --- 3.3V
 SCK    --- SCL(Pin#5)
 SDA    --- SDA(Pin#3)

*/

#include <stdio.h>  
#include <stdint.h>
#include <stdbool.h>  
#include <stdlib.h>  
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <wiringPiI2C.h>
#include <wiringShift.h>
#include "fontx.h"

// Hardware/Software SPI
#define RST  1  // You can change
#define DC   0  // You can change

// Software SPI
#define MOSI 12 // You can change
#define SCLK 14 // You can change
#define CS   10  // You can change

//#define BITBANG    1
//#define SHIFTOUT   2
#define _BV(bit) (1<<(bit))

#define I2C_ADDRESS        0x3C

#define SSD1306_DEBUG 0
#define OLED_DEBUG    0

FontxFile fx[2];

typedef struct {
  uint8_t ank;
  uint8_t utf;
  uint8_t colum;
  uint8_t reverse;
  uint8_t enhance;
  uint8_t size;
  uint8_t ascii[16];
  uint16_t sjis[16];
} SaveData;

typedef struct {
  SaveData save[4];
} SaveFrame;

unsigned char frame[1024]; // frame buffer

void init_hardware_spi(void);
void init_software_spi(void);
void init_i2c(uint8_t i2caddr);
int drawChar(int x,int y,unsigned char chr,uint8_t reverse,uint8_t enhance);
int drawSJISChar(FontxFile *fx,int x,int y,uint16_t sjis,uint8_t reverse,
                 uint8_t enhance);
int drawUTF8Char(FontxFile *fx,int x,int y,uint8_t *utf8,uint8_t reverse,
                 uint8_t enhance);
void show_hardware_spi(int page, int offset);
void show_software_spi(int page, int offset);
void show_i2c(int page, int offset);
void usage(char *prog);
void DumpSaveFrame(SaveFrame hoge);

int i2cd;

#ifndef X32
// 128x64
unsigned char init_command[] = {
#if 0
    // OLED_CMD_SET_HORI_ADDR_MODE
    0xAE, 0xA8, 0x3F, 0xD3, 0x00, 0x40, 0xA1, 0xC8,
    0xD5, 0x80, 0xDA, 0x12, 0x81, 0xFF,
    0xA4, 0xDB, 0x40, 0x20, 0x00, 0x00, 0x10, 0x8D,
    0x14, 0x2E, 0xA6, 0xAF
#endif

#if 1
    // OLED_CMD_SET_PAGE_ADDR_MODE
    0xAE, 0xA8, 0x3F, 0xD3, 0x00, 0x40, 0xA1, 0xC8,
    0xD5, 0x80, 0xDA, 0x12, 0x81, 0xFF,
    0xA4, 0xDB, 0x40, 0x20, 0x02, 0x00, 0x10, 0x8D,
    0x14, 0x2E, 0xA6, 0xAF
#endif
};
#endif

#ifdef X32
// 128x32
unsigned char init_command[] = {
#if 0
    // OLED_CMD_SET_HORI_ADDR_MODE
    0xAE, 0xA8, 0x1F, 0xD3, 0x00, 0x40, 0xA1, 0xC8,
    0xD5, 0x80, 0xDA, 0x02, 0x81, 0xFF,
    0xA4, 0xDB, 0x40, 0x20, 0x00, 0x00, 0x10, 0x8D,
    0x14, 0x2E, 0xA6, 0xAF
#endif
#if 1
    // OLED_CMD_SET_PAGE_ADDR_MODE
    0xAE, 0xA8, 0x1F, 0xD3, 0x00, 0x40, 0xA1, 0xC8,
    0xD5, 0x80, 0xDA, 0x02, 0x81, 0xFF,
    0xA4, 0xDB, 0x40, 0x20, 0x02, 0x00, 0x10, 0x8D,
    0x14, 0x2E, 0xA6, 0xAF
#endif
};
#endif


int main(int argc, char **argv){
  int i;
  char cpath[128];
  FILE *fp;
  SaveFrame sv;

  int offset = 0;
  int page = 8;
#ifdef OFFSET
  offset = OFFSET;
#endif
#ifdef X32
  page = 4;
#endif
  //printf("page=%d\n",page);
  //printf("Your module has offset=%d\n", offset);
  
if(OLED_DEBUG)  printf("argv[0]=%s\n",argv[0]);
  strcpy(cpath, argv[0]);
  for(i=strlen(cpath);i>0;i--) {
    if (cpath[i-1] == '/') {
      cpath[i] = 0;
      break;
    }
  }
  strcat(cpath,"oled.conf");
if(OLED_DEBUG)  printf("cpath=%s\n",cpath);

  struct stat stat_buf;
  if (stat(cpath,&stat_buf) == 0) {
if(OLED_DEBUG)    printf("file found\n");
    fp = fopen(cpath,"rb");
    fread(&sv,sizeof(sv),1,fp);
    fclose(fp);
  } else {
if(OLED_DEBUG)    printf("file not found\n");
    memset(&sv,0,sizeof(sv));
  }
if(OLED_DEBUG)DumpSaveFrame(sv);

  if (argc == 1) {
    usage(basename(argv[0]));
    return 1;
  }

  // You can change font file
  Fontx_init(fx,"./fontx/ILGH16XB.FNT","./fontx/ILGZ16XB.FNT"); // 16Dot Gothic
//  Fontx_init(fx,"./fontx/ILMH16XB.FNT","./fontx/ILMZ16XB.FNT"); // 16Dot Mincyo

  int spos;
  char numc[5];
  int num;

  if ( (strcmp(argv[1],"+1") == 0) ||
       (strcmp(argv[1],"+2") == 0) ||
       (strcmp(argv[1],"+3") == 0) ||
       (strcmp(argv[1],"+4") == 0) ) {
    strcpy(numc,argv[1]);
    num = (numc[1] - '0') - 1;
      
if(OLED_DEBUG)printf("add string to line [%d]\n",num);
    sv.save[num].size = String2SJIS((unsigned char *)argv[2], strlen(argv[2]), sv.save[num].sjis, 16);
    sv.save[num].ank = 0;
    sv.save[num].utf = 1;
if(OLED_DEBUG)DumpSaveFrame(sv);
    fp = fopen(cpath,"wb");
    fwrite(&sv,sizeof(sv),1,fp);
    fclose(fp);
    return 0;
  }

  if ( (strcmp(argv[1],"+a") == 0) ||
       (strcmp(argv[1],"+b") == 0) ||
       (strcmp(argv[1],"+c") == 0) ||
       (strcmp(argv[1],"+d") == 0) ) {
    strcpy(numc,argv[1]);
    num = numc[1] - 'a';
if(OLED_DEBUG)printf("add ASCII string to line [%d]\n",num);

    spos = 0;
    char *ptr;
    ptr = argv[2];
    for (i=0;i<strlen(argv[2]);i++) {
if(OLED_DEBUG)printf("argv[2]=%c\n",*ptr);
      if (*ptr < 0x7f && spos < 16) {
        sv.save[num].ascii[spos++] = *ptr;
      }
      ptr++;
    }
    sv.save[num].ank = 1;
    sv.save[num].utf = 0;
    sv.save[num].size = spos;
if(OLED_DEBUG)DumpSaveFrame(sv);
    fp = fopen(cpath,"wb");
    fwrite(&sv,sizeof(sv),1,fp);
    fclose(fp);
    return 0;
  }

  if ( (strcmp(argv[1],"-1") == 0) ||
       (strcmp(argv[1],"-2") == 0) ||
       (strcmp(argv[1],"-3") == 0) ||
       (strcmp(argv[1],"-4") == 0) ) {
    strcpy(numc,argv[1]);
    num = (numc[1] - '0') - 1;
if(OLED_DEBUG)printf("delete string to line [%d]\n",num);
    sv.save[num].size = 0;
    sv.save[num].reverse = 0;
if(OLED_DEBUG)DumpSaveFrame(sv);
    fp = fopen(cpath,"wb");
    fwrite(&sv,sizeof(sv),1,fp);
    fclose(fp);
    return 0;
  }

  if ( (strcmp(argv[1],"+R") == 0) ||
       (strcmp(argv[1],"-R") == 0) ) {
    strcpy(numc,argv[2]);
    num = (numc[0] - '0') - 1;
if(OLED_DEBUG)printf("set/unset reverse to line [%d]\n",num);
    if (num < 0 || num > 3) return 1;
    if (strcmp(argv[1],"+R") == 0) sv.save[num].reverse = 1;
    if (strcmp(argv[1],"-R") == 0) sv.save[num].reverse = 0;
    fp = fopen(cpath,"wb");
    fwrite(&sv,sizeof(sv),1,fp);
    fclose(fp);
    return 0;
  }

  if ( (strcmp(argv[1],"+U") == 0) ||
       (strcmp(argv[1],"-U") == 0) ) {
    strcpy(numc,argv[2]);
    num = (numc[0] - '0') - 1;
if(OLED_DEBUG)printf("set/unset reverse to line [%d]\n",num);
    if (num < 0 || num > 3) return 1;
    if (strcmp(argv[1],"+U") == 0) sv.save[num].enhance = 1;
    if (strcmp(argv[1],"-U") == 0) sv.save[num].enhance = 0;
    fp = fopen(cpath,"wb");
    fwrite(&sv,sizeof(sv),1,fp);
    fclose(fp);
    return 0;
  }

  if ( strcmp(argv[1],"+L") == 0) {
    memcpy(&sv.save[0],&sv.save[1],sizeof(SaveData));
    memcpy(&sv.save[1],&sv.save[2],sizeof(SaveData));
    memcpy(&sv.save[2],&sv.save[3],sizeof(SaveData));
    memset(&sv.save[3],0,sizeof(SaveData));
    fp = fopen(cpath,"wb");
    fwrite(&sv,sizeof(sv),1,fp);
    fclose(fp);
    return 0;
  }

  if ( strcmp(argv[1],"-L") == 0) {
    memcpy(&sv.save[3],&sv.save[2],sizeof(SaveData));
    memcpy(&sv.save[2],&sv.save[1],sizeof(SaveData));
    memcpy(&sv.save[1],&sv.save[0],sizeof(SaveData));
    memset(&sv.save[0],0,sizeof(SaveData));
    fp = fopen(cpath,"wb");
    fwrite(&sv,sizeof(sv),1,fp);
    fclose(fp);
    return 0;
  }

  if ( (strcmp(argv[1],"P1") == 0) ||
       (strcmp(argv[1],"P2") == 0) ||
       (strcmp(argv[1],"P3") == 0) ||
       (strcmp(argv[1],"P4") == 0) ) {
    strcpy(numc,argv[1]);
    num = (numc[1] - '0') - 1;
    strcpy(numc,argv[2]);
    int col;
//    col = (numc[0] - '0') - 1;
    col = atoi(numc) - 1;
    if (col < 0 || col > 16) return 1;
if(OLED_DEBUG)printf("set start colum to line [%d] = %d\n",num,col);
    sv.save[num].colum = col;
    fp = fopen(cpath,"wb");
    fwrite(&sv,sizeof(sv),1,fp);
    fclose(fp);
    return 0;
  }

  if (strcmp(argv[1],"r") == 0) {
    memset(&sv,0,sizeof(sv));
if(OLED_DEBUG)DumpSaveFrame(sv);
    fp = fopen(cpath,"wb");
    fwrite(&sv,sizeof(sv),1,fp);
    fclose(fp);
    return 0;
  }

  if (strcmp(argv[1],"s") == 0) {
if(OLED_DEBUG)printf("show dislay\n");

#ifdef SPI
    init_hardware_spi();
#endif
#ifdef SOFT_SPI
    init_software_spi();
#endif
#ifdef I2C
    init_i2c(I2C_ADDRESS);
#endif
    int y;
    for (num=0;num<4;num++) {
      if (sv.save[num].size == 0) continue;
//      y = 1;
      y = sv.save[num].colum+1;
      for (i=0;i<sv.save[num].size;i++) {
         
        if (sv.save[num].ank)
          y = drawChar(num+1,y,sv.save[num].ascii[i],sv.save[num].reverse,
                       sv.save[num].enhance);
        if (sv.save[num].utf)
          y = drawSJISChar(fx,num+1,y,sv.save[num].sjis[i],sv.save[num].reverse,
                           sv.save[num].enhance);
      }
    }
#ifdef SPI
    show_hardware_spi(page, offset);	
#endif
#ifdef SOFT_SPI
    show_software_spi(page, offset);	
#endif
#ifdef I2C
    show_i2c(page, offset);	
#endif
  }

  if (strcmp(argv[1],"D") == 0) {
    DumpSaveFrame(sv);
  }
}


/*
Initialize SSD1306 (hardware spi)
*/

void init_hardware_spi(void){
  int byte;

  for(byte=0;byte<1024;byte++){
    frame[byte] = 0x00;
  }

  wiringPiSetup();
  pinMode (DC, OUTPUT) ;
  pinMode (RST, OUTPUT) ;
  wiringPiSPISetup(0, 32*1000*1000);
  digitalWrite(RST,  HIGH) ;
  delay(50);
  digitalWrite(RST,  LOW) ;
  delay(50);
  digitalWrite(RST,  HIGH) ;
  digitalWrite(DC, LOW);
  digitalWrite(CS, LOW);
  wiringPiSPIDataRW(0, init_command, sizeof(init_command));
  digitalWrite(CS, HIGH);

}

/*
Initialize SSD1306 (software spi)
*/

void init_software_spi(void){
  int byte;

  for(byte=0;byte<1024;byte++){
    frame[byte] = 0x00;
  }

  wiringPiSetup();
  pinMode (CS, OUTPUT) ;
  pinMode (DC, OUTPUT) ;
  pinMode (RST, OUTPUT) ;
  pinMode (MOSI, OUTPUT) ;
  pinMode (SCLK, OUTPUT) ;
  digitalWrite(RST,  HIGH) ;
  delay(50);
  digitalWrite(RST,  LOW) ;
  delay(50);
  digitalWrite(RST,  HIGH) ;
  digitalWrite(DC, LOW);
  digitalWrite(CS, LOW);
  for(byte=0;byte<sizeof(init_command);byte++) {
    shiftOut(MOSI, SCLK, MSBFIRST, init_command[byte]);
  }
  digitalWrite(CS, HIGH);

}


/*
Initialize SSD1306 (i2c)
*/

void init_i2c(uint8_t i2caddr){
  int byte;

  for(byte=0;byte<1024;byte++){
    frame[byte] = 0x00;
  }
	
  i2cd = wiringPiI2CSetup(i2caddr);
  int i;
  for(i=0;i<sizeof(init_command);i++) {
    unsigned int control = 0x00;    // Co = 0, D/C = 0
    wiringPiI2CWriteReg8(i2cd, control, init_command[i]);
  }

}

/*
Draw SJIS character on SSD1306
 x:1~4
 y:1~16
 sjis:SJIS character
*/

int drawSJISChar(FontxFile *fx, int x,int y,uint16_t sjis,uint8_t reverse,
                 uint8_t enhance){
  unsigned char fonts[32]; // font pattern
  uint8_t bitmap[128];
  unsigned char pw, ph;
  bool rc;

//  sjis = UTF2SJIS(utf8);
if(SSD1306_DEBUG)  printf("sjis=%x\n",sjis);

  rc = GetFontx(fx, sjis, fonts, &pw, &ph); // SJIS -> Fontパターン
if(SSD1306_DEBUG)  printf("GetFontx rc=%d pw=%d ph=%d\n",rc,pw,ph);
  if (rc) { 
    Font2Bitmap(fonts, bitmap, pw, ph, 1);
    if(enhance) UnderlineBitmap(bitmap, pw, ph);
    if(reverse) ReversBitmap(bitmap, pw, ph);
    memcpy(frame+((x-1)*256)+((y-1)*8),bitmap,pw);
    memcpy(frame+(128+(x-1)*256)+((y-1)*8),bitmap+32,pw);
  }
  return y + (pw/8);
}

/*
Draw UTF8 character on SSD1306
 x:1~4
 y:1~16
 utf8:UTF8 character
*/

int drawUTF8Char(FontxFile *fx, int x,int y,uint8_t *utf8,uint8_t reverse,
                 uint8_t enhance){
  uint16_t sjis;

  // convert UTF8 to SJIS
  sjis = UTF2SJIS(utf8);
if(SSD1306_DEBUG)  printf("sjis=%x\n",sjis);
  return drawSJISChar(fx, x, y, sjis, reverse, enhance);
}

/*
Draw ASCII character on SSD1306
 x:1~4
 y:1~16
 chr:ASCII character
*/

int drawChar(int x,int y,unsigned char chr,uint8_t reverse,uint8_t enhance){
  unsigned char a[8],b[8];
  unsigned int ascii_index;
  unsigned char num_str[95][16] = {
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//  0
	{0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x33,0x30,0x00,0x00,0x00},//! 1
	{0x00,0x10,0x0C,0x06,0x10,0x0C,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//" 2
	{0x40,0xC0,0x78,0x40,0xC0,0x78,0x40,0x00,0x04,0x3F,0x04,0x04,0x3F,0x04,0x04,0x00},//# 3
	{0x00,0x70,0x88,0xFC,0x08,0x30,0x00,0x00,0x00,0x18,0x20,0xFF,0x21,0x1E,0x00,0x00},//$ 4
	{0xF0,0x08,0xF0,0x00,0xE0,0x18,0x00,0x00,0x00,0x21,0x1C,0x03,0x1E,0x21,0x1E,0x00},//% 5
	{0x00,0xF0,0x08,0x88,0x70,0x00,0x00,0x00,0x1E,0x21,0x23,0x24,0x19,0x27,0x21,0x10},//& 6
	{0x10,0x16,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//' 7
	{0x00,0x00,0x00,0xE0,0x18,0x04,0x02,0x00,0x00,0x00,0x00,0x07,0x18,0x20,0x40,0x00},//( 8
	{0x00,0x02,0x04,0x18,0xE0,0x00,0x00,0x00,0x00,0x40,0x20,0x18,0x07,0x00,0x00,0x00},//) 9
	{0x40,0x40,0x80,0xF0,0x80,0x40,0x40,0x00,0x02,0x02,0x01,0x0F,0x01,0x02,0x02,0x00},//* 10
	{0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x1F,0x01,0x01,0x01,0x00},//+ 11
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xB0,0x70,0x00,0x00,0x00,0x00,0x00},//, 12
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01},//- 13
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,0x00,0x00},//. 14
	{0x00,0x00,0x00,0x00,0x80,0x60,0x18,0x04,0x00,0x60,0x18,0x06,0x01,0x00,0x00,0x00},/// 15
	{0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x0F,0x10,0x20,0x20,0x10,0x0F,0x00},//0 16
	{0x00,0x10,0x10,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00},//1 17
	{0x00,0x70,0x08,0x08,0x08,0x88,0x70,0x00,0x00,0x30,0x28,0x24,0x22,0x21,0x30,0x00},//2 18
	{0x00,0x30,0x08,0x88,0x88,0x48,0x30,0x00,0x00,0x18,0x20,0x20,0x20,0x11,0x0E,0x00},//3 19
	{0x00,0x00,0xC0,0x20,0x10,0xF8,0x00,0x00,0x00,0x07,0x04,0x24,0x24,0x3F,0x24,0x00},//4 20
	{0x00,0xF8,0x08,0x88,0x88,0x08,0x08,0x00,0x00,0x19,0x21,0x20,0x20,0x11,0x0E,0x00},//5 21
	{0x00,0xE0,0x10,0x88,0x88,0x18,0x00,0x00,0x00,0x0F,0x11,0x20,0x20,0x11,0x0E,0x00},//6 22
	{0x00,0x38,0x08,0x08,0xC8,0x38,0x08,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00},//7 23
	{0x00,0x70,0x88,0x08,0x08,0x88,0x70,0x00,0x00,0x1C,0x22,0x21,0x21,0x22,0x1C,0x00},//8 24
	{0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x00,0x31,0x22,0x22,0x11,0x0F,0x00},//9 25
	{0x00,0x00,0x00,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00},//: 26
	{0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x60,0x00,0x00,0x00,0x00},//; 27
	{0x00,0x00,0x80,0x40,0x20,0x10,0x08,0x00,0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x00},//< 28
	{0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x00,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x00},//= 29
	{0x00,0x08,0x10,0x20,0x40,0x80,0x00,0x00,0x00,0x20,0x10,0x08,0x04,0x02,0x01,0x00},//> 30
	{0x00,0x70,0x48,0x08,0x08,0x08,0xF0,0x00,0x00,0x00,0x00,0x30,0x36,0x01,0x00,0x00},//? 31
	{0xC0,0x30,0xC8,0x28,0xE8,0x10,0xE0,0x00,0x07,0x18,0x27,0x24,0x23,0x14,0x0B,0x00},//@ 32
	{0x00,0x00,0xC0,0x38,0xE0,0x00,0x00,0x00,0x20,0x3C,0x23,0x02,0x02,0x27,0x38,0x20},//A 33
	{0x08,0xF8,0x88,0x88,0x88,0x70,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x11,0x0E,0x00},//B 34
	{0xC0,0x30,0x08,0x08,0x08,0x08,0x38,0x00,0x07,0x18,0x20,0x20,0x20,0x10,0x08,0x00},//C 35
	{0x08,0xF8,0x08,0x08,0x08,0x10,0xE0,0x00,0x20,0x3F,0x20,0x20,0x20,0x10,0x0F,0x00},//D 36
	{0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x20,0x23,0x20,0x18,0x00},//E 37
	{0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x00,0x03,0x00,0x00,0x00},//F 38
	{0xC0,0x30,0x08,0x08,0x08,0x38,0x00,0x00,0x07,0x18,0x20,0x20,0x22,0x1E,0x02,0x00},//G 39
	{0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x20,0x3F,0x21,0x01,0x01,0x21,0x3F,0x20},//H 40
	{0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00},//I 41
	{0x00,0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0xC0,0x80,0x80,0x80,0x7F,0x00,0x00,0x00},//J 42
	{0x08,0xF8,0x88,0xC0,0x28,0x18,0x08,0x00,0x20,0x3F,0x20,0x01,0x26,0x38,0x20,0x00},//K 43
	{0x08,0xF8,0x08,0x00,0x00,0x00,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x20,0x30,0x00},//L 44
	{0x08,0xF8,0xF8,0x00,0xF8,0xF8,0x08,0x00,0x20,0x3F,0x00,0x3F,0x00,0x3F,0x20,0x00},//M 45
	{0x08,0xF8,0x30,0xC0,0x00,0x08,0xF8,0x08,0x20,0x3F,0x20,0x00,0x07,0x18,0x3F,0x00},//N 46
	{0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,0x0F,0x10,0x20,0x20,0x20,0x10,0x0F,0x00},//O 47
	{0x08,0xF8,0x08,0x08,0x08,0x08,0xF0,0x00,0x20,0x3F,0x21,0x01,0x01,0x01,0x00,0x00},//P 48
	{0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,0x0F,0x18,0x24,0x24,0x38,0x50,0x4F,0x00},//Q 49
	{0x08,0xF8,0x88,0x88,0x88,0x88,0x70,0x00,0x20,0x3F,0x20,0x00,0x03,0x0C,0x30,0x20},//R 50
	{0x00,0x70,0x88,0x08,0x08,0x08,0x38,0x00,0x00,0x38,0x20,0x21,0x21,0x22,0x1C,0x00},//S 51
	{0x18,0x08,0x08,0xF8,0x08,0x08,0x18,0x00,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00},//T 52
	{0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00},//U 53
	{0x08,0x78,0x88,0x00,0x00,0xC8,0x38,0x08,0x00,0x00,0x07,0x38,0x0E,0x01,0x00,0x00},//V 54
	{0xF8,0x08,0x00,0xF8,0x00,0x08,0xF8,0x00,0x03,0x3C,0x07,0x00,0x07,0x3C,0x03,0x00},//W 55
	{0x08,0x18,0x68,0x80,0x80,0x68,0x18,0x08,0x20,0x30,0x2C,0x03,0x03,0x2C,0x30,0x20},//X 56
	{0x08,0x38,0xC8,0x00,0xC8,0x38,0x08,0x00,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00},//Y 57
	{0x10,0x08,0x08,0x08,0xC8,0x38,0x08,0x00,0x20,0x38,0x26,0x21,0x20,0x20,0x18,0x00},//Z 58
	{0x00,0x00,0x00,0xFE,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x7F,0x40,0x40,0x40,0x00},//[ 59
	{0x00,0x0C,0x30,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x06,0x38,0xC0,0x00},//\ 60
	{0x00,0x02,0x02,0x02,0xFE,0x00,0x00,0x00,0x00,0x40,0x40,0x40,0x7F,0x00,0x00,0x00},//] 61
	{0x00,0x00,0x04,0x02,0x02,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//^ 62
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80},//_ 63
	{0x00,0x02,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//` 64
	{0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x19,0x24,0x22,0x22,0x22,0x3F,0x20},//a 65
	{0x08,0xF8,0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x3F,0x11,0x20,0x20,0x11,0x0E,0x00},//b 66
	{0x00,0x00,0x00,0x80,0x80,0x80,0x00,0x00,0x00,0x0E,0x11,0x20,0x20,0x20,0x11,0x00},//c 67
	{0x00,0x00,0x00,0x80,0x80,0x88,0xF8,0x00,0x00,0x0E,0x11,0x20,0x20,0x10,0x3F,0x20},//d 68
	{0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x22,0x22,0x22,0x22,0x13,0x00},//e 69
	{0x00,0x80,0x80,0xF0,0x88,0x88,0x88,0x18,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00},//f 70
	{0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x6B,0x94,0x94,0x94,0x93,0x60,0x00},//g 71
	{0x08,0xF8,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20},//h 72
	{0x00,0x80,0x98,0x98,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00},//i 73
	{0x00,0x00,0x00,0x80,0x98,0x98,0x00,0x00,0x00,0xC0,0x80,0x80,0x80,0x7F,0x00,0x00},//j 74
	{0x08,0xF8,0x00,0x00,0x80,0x80,0x80,0x00,0x20,0x3F,0x24,0x02,0x2D,0x30,0x20,0x00},//k 75
	{0x00,0x08,0x08,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00},//l 76
	{0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x20,0x3F,0x20,0x00,0x3F,0x20,0x00,0x3F},//m 77
	{0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20},//n 78
	{0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00},//o 79
	{0x80,0x80,0x00,0x80,0x80,0x00,0x00,0x00,0x80,0xFF,0xA1,0x20,0x20,0x11,0x0E,0x00},//p 80
	{0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x0E,0x11,0x20,0x20,0xA0,0xFF,0x80},//q 81
	{0x80,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x20,0x20,0x3F,0x21,0x20,0x00,0x01,0x00},//r 82
	{0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x33,0x24,0x24,0x24,0x24,0x19,0x00},//s 83
	{0x00,0x80,0x80,0xE0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x1F,0x20,0x20,0x00,0x00},//t 84
	{0x80,0x80,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x1F,0x20,0x20,0x20,0x10,0x3F,0x20},//u 85
	{0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x00,0x01,0x0E,0x30,0x08,0x06,0x01,0x00},//v 86
	{0x80,0x80,0x00,0x80,0x00,0x80,0x80,0x80,0x0F,0x30,0x0C,0x03,0x0C,0x30,0x0F,0x00},//w 87
	{0x00,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x31,0x2E,0x0E,0x31,0x20,0x00},//x 88
	{0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x80,0x81,0x8E,0x70,0x18,0x06,0x01,0x00},//y 89
	{0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x21,0x30,0x2C,0x22,0x21,0x30,0x00},//z 90
	{0x00,0x00,0x00,0x00,0x80,0x7C,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x3F,0x40,0x40},//{ 91
	{0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00},//| 92
	{0x00,0x02,0x02,0x7C,0x80,0x00,0x00,0x00,0x00,0x40,0x40,0x3F,0x00,0x00,0x00,0x00},//} 93
	{0x00,0x06,0x01,0x01,0x02,0x02,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//~ 94				
	};
if(SSD1306_DEBUG)  printf("drawChar chr=%x y=%d\n",chr,y);
  ascii_index = (chr-32);
  memcpy(a,num_str[ascii_index],8);
  memcpy(b,num_str[ascii_index]+8,8);

  int i;
  if (reverse) {
    for (i=0;i<8;i++) {
      a[i] = ~a[i];
      b[i] = ~b[i];
    }
  }

  if (enhance) {
    for (i=0;i<8;i++) {
      b[i] = b[i]  + 0x80;
    }
  }

  memcpy(frame+((x-1)*256)+((y-1)*8),a,8);
  memcpy(frame+((x-1)*256)+((y-1)*8)+128,b,8);
  return y+1;
}

/*
Show frame buffer to SSD1306 (spi)
*/

void show_hardware_spi(int page, int offset){
  unsigned char page_command[3];

  for (int _page=0;_page<page;_page++) {
    page_command[0] = 0x00 + offset;
    page_command[1] = 0x10;
    page_command[2] = 0xB0 + _page;
    digitalWrite(DC,  LOW);
    digitalWrite(CS, LOW);
    wiringPiSPIDataRW(0, page_command, sizeof(page_command));
    digitalWrite(CS, HIGH);

    digitalWrite(DC,  HIGH);
    digitalWrite(CS, LOW);
    wiringPiSPIDataRW(0, &frame[_page*128], 128);
    digitalWrite(CS, HIGH);
  }

#if 0
  digitalWrite(DC,  HIGH);
  digitalWrite(CS, LOW);
  wiringPiSPIDataRW(0, frame, 1024);
  digitalWrite(CS, HIGH);
#endif
}

/*
Show frame buffer to SSD1306 (software spi)
*/

void show_software_spi(int page, int offset){
  unsigned char page_command[3];

  for (int _page=0;_page<page;_page++) {
    page_command[0] = 0x00 + offset;
    page_command[1] = 0x10;
    page_command[2] = 0xB0 + _page;
    digitalWrite(DC,  LOW);
    digitalWrite(CS,  LOW);
    for(int i=0;i<sizeof(page_command);i++) {
      shiftOut(MOSI, SCLK, MSBFIRST, page_command[i]);
    }

    digitalWrite(DC,  HIGH);
    digitalWrite(CS,  LOW);
    for(int col=0;col<128;col++) {
      shiftOut(MOSI, SCLK, MSBFIRST, frame[_page*128+col]);
    }
    digitalWrite(CS,  HIGH);
  }



#if 0
  int byte;
  digitalWrite(DC,  HIGH);
  digitalWrite(CS,  LOW);
  for(byte=0;byte<1024;byte++) {
    shiftOut(MOSI, SCLK, MSBFIRST, frame[byte]);
  }
  digitalWrite(CS,  HIGH);
#endif
}

/*
Show frame buffer to SSD1306 (i2c)
*/

void show_i2c(int page,int offset){
  unsigned char page_command[3];

  for (int _page=0;_page<page;_page++) {
    page_command[0] = 0x00 + offset;
    page_command[1] = 0x10;
    page_command[2] = 0xB0 + _page;

    for(int i=0;i<sizeof(page_command);i++) {
      wiringPiI2CWriteReg8(i2cd, 0x00, page_command[i]);
    }

    for(int col=0;col<128;col++) {
      wiringPiI2CWriteReg8(i2cd, 0x40, frame[_page*128+col]);
    }
  }
#if 0
  int i;
  for (i = 0; i < 1024; i++) {
    wiringPiI2CWriteReg8(i2cd, 0x40, frame[i]);
  }
#endif
}


/*
Show Usage
*/

void usage(char *prog){
  printf("Usage : %s option\n",prog);
  printf("Options:\n");
  printf("  s             show display\n");
  printf("  r             remove all string\n");
  printf("  +1 string     add string to line#1(Ext. Font)\n");
  printf("  +2 string     add string to line#2(Ext. Font)\n");
  printf("  +3 string     add string to line#3(Ext. Font)\n");
  printf("  +4 string     add string to line#4(Ext. Font)\n");
  printf("  +a string     add string to line#1(Int. Font)\n");
  printf("  +b string     add string to line#2(Int. Font)\n");
  printf("  +c string     add string to line#3(Int. Font)\n");
  printf("  +d string     add string to line#4(Int. Font)\n");
  printf("  -1            delete string from line#1\n");
  printf("  -2            delete string from line#2\n");
  printf("  -3            delete string from line#3\n");
  printf("  -4            delete string from line#4\n");
  printf("  +R n          set reverse to line#n\n");
  printf("  -R n          unset reverse to line#n\n");
  printf("  +U n          set underline to line#n\n");
  printf("  -U n          unset underline to line#n\n");
  printf("  +L            line scroll up\n");
  printf("  -L            line scroll down\n");
  printf("  P1 n          set start colum n to line#1\n");
  printf("  P2 n          set start colum n to line#2\n");
  printf("  P3 n          set start colum n to line#3\n");
  printf("  P4 n          set start colum n to line#4\n");
}

void DumpSaveFrame(SaveFrame hoge) {
  int i,j;
  
  for(i=0;i<4;i++) {
    printf("[%d].ank=%d ",i,hoge.save[i].ank);
    printf("[%d].utf=%d ",i,hoge.save[i].utf);
    printf("[%d].colum=%d ",i,hoge.save[i].colum);
    printf("[%d].reverse=%d ",i,hoge.save[i].reverse);
    printf("[%d].enhance=%d n",i,hoge.save[i].enhance);
    printf("[%d].size=%d\n",i,hoge.save[i].size);
    for(j=0;j<hoge.save[i].size;j++) {
      if (hoge.save[i].ank == 1) {
        printf("[%d].ascii[%d]=%x\n",i,j,hoge.save[i].ascii[j]);
      }
      if (hoge.save[i].utf == 1) {
        printf("[%d].ascii[%d]=%x\n",i,j,hoge.save[i].sjis[j]);
      }
    }
  }

}
