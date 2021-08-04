// Password Vault
// Firmware for the ESP8266
// Distributed under the MIT License
// © Copyright Maxim Bortnikov 2021
// For more information please visit
// https://github.com/Northstrix/Password_Vault
#include <SoftwareSerial.h>
#include <DES.h>
#include <ESP8266TrueRandom.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
SoftwareSerial mySerial(0, 2); // RX, TX
DES des;
byte key[] = { 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x4e, 0xa7, 0x0f, 0x8f, 0xcc, 0xa3, 0x70, 0x94,
                0x80, 0xdc, 0x2d, 0xfc, 0x00, 0x00, 0x00, 0x00,
              };
              
byte key_two[] = { 
                  0xf3, 0x5c, 0xe2, 0x13, 0xb4, 0xbb, 0x61, 0x77,
                  0x4c, 0x1a, 0xda, 0x2c, 0x26, 0x8f, 0x88, 0x9b,
                  0x4a, 0x18, 0x5b, 0x36, 0x51, 0x85, 0xA8, 0x4B,
                };
                
byte key_three[] = { 
                  0x00, 0x00, 0x00, 0x00, 0x17, 0xad, 0x82, 0x3a,
                  0x0c, 0xda, 0xbc, 0x61, 0x61, 0x9d, 0xee, 0xc2,
                  0x2e, 0x72, 0x3d, 0xb2, 0x5e, 0x15, 0x7a, 0x87,
                };

byte ivkey[] = { 
                  0x14, 0x72, 0x98, 0x76, 0xaf, 0x1e, 0xc3, 0x5e,
                  0x92, 0x2f, 0xb5, 0x10, 0x34, 0x6c, 0x4f, 0x02,
                  0x83, 0xcd, 0xf5, 0x37, 0x15, 0x20, 0xf5, 0x92,
                };

#include "GBUS.h"
GBUS bus(&mySerial, 3, 25);
#define TFT_CS         4
#define TFT_RST        16                                            
#define TFT_DC         5

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

int f;
bool in_s;
String stiv1;
String stiv2;
String stiv3;
String stiv4;
String dec_stuff;

struct myStruct {
  byte dt[4];
};

uint8_t Forward_S_Box[16][16] = {  
    {0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76},  
    {0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0},  
    {0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15},  
    {0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75},  
    {0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84},  
    {0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF},  
    {0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8},  
    {0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2},  
    {0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73},  
    {0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB},  
    {0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79},  
    {0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08},  
    {0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A},  
    {0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E},  
    {0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF},  
    {0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16}  
};

uint8_t Inv_S_Box[16][16] = {  
    {0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB},  
    {0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB},  
    {0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E},  
    {0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25},  
    {0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92},  
    {0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84},  
    {0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06},  
    {0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B},  
    {0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73},  
    {0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E},  
    {0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B},  
    {0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4},  
    {0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F},  
    {0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF},  
    {0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61},  
    {0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D}  
};

void Generate_IVs(){
  stiv1 = "";
  stiv2 = "";
  stiv3 = "";
  stiv4 = "";
  for(int i = 0; i < 8; i++){
    char x = char(ESP8266TrueRandom.random(33,126));
    char c = char(ESP8266TrueRandom.random(33,126));
    char v = char(ESP8266TrueRandom.random(33,126));
    char b = char(ESP8266TrueRandom.random(33,126));
    stiv1 += x;
    stiv2 += c;
    stiv3 += v;
    stiv4 += b;
  }
  encryptivs(stiv1);
  encryptivs(stiv2);
  encryptivs(stiv3);
  encryptivs(stiv4);
}

//////////// Encrypt Start ////////////

void encryptivs(String plaintext)
{
  byte in[] = {0, 0, 0, 0, 0, 0, 0, 0};
  for (int i = 0; i < 8; i++)
  {
      char x = plaintext[i];
      in[i] = int(x);
  }
  byte out[8];
  des.tripleEncrypt(out, in, ivkey);
  for(int i = 0; i<8; i++){
    if(out[i]<16)
    Serial.print("0");
    Serial.print(out[i],HEX);
  }
  }

void split_by_eight(char plntxt[8], int k, int str_len){
  char res[] = {0, 0, 0, 0, 0, 0, 0, 0};
  for (int i = 0; i < 8; i++){
      if(i+k > str_len - 1)
      break;
      res[i] = plntxt[i+k];
  }
  Forw_S_Box(res);
}

void Forw_S_Box(char first_eight[8]){
  byte aft_box[8];
  for (int count; count<4; count++){
  String strOne = "";
  String strTwo = "";
  int i = count * 2;
  int j = count * 2 + 1;
  int fir = first_eight[i];
  int sec = first_eight[j];
  if (fir < 16)
  strOne += 0;
  strOne +=  String(fir, HEX);
  if (sec < 16)
  strTwo += 0;
  strTwo +=  String(sec, HEX);  
  /*
  Serial.print(strOne);
  Serial.println("");
  Serial.print(strTwo);
  Serial.println("");
  */
  char chars_f[3];
  char chars_s[3];
  strOne.toCharArray(chars_f, 3);
  strTwo.toCharArray(chars_s, 3);
  chars_f[2] = '\0';
  chars_s[2] = '\0';
  /*
  Serial.print(chars_f[0]);
  Serial.print(chars_f[1]);
  Serial.println("");
  Serial.print(chars_s[0]);
  Serial.print(chars_s[1]);
  Serial.println("");
  */
  int flc = getNum(chars_f[0]);
  int frc = getNum(chars_f[1]);
  int slc = getNum(chars_s[0]);
  int src = getNum(chars_s[1]);
  /*
  Serial.printf("%x",Forward_S_Box[flc][frc]);
  Serial.print(" ");
  Serial.printf("%x",Forward_S_Box[slc][src]);
  Serial.print(" ");
  */
  aft_box[i] = ("%x",Forward_S_Box[flc][frc]);
  aft_box[j] = ("%x",Forward_S_Box[slc][src]);
  }
  /*
  Serial.println("");
  for(int i = 0; i<8; i++){
  Serial.printf("%x",aft_box[i]);
  Serial.print(" ");
  }
  */
  first_xor(aft_box);
}

void first_xor(byte aft_forw_box[8]){
  int stiv1_len = stiv1.length() + 1;
  char iv1[stiv1_len];
  stiv1.toCharArray(iv1, stiv1_len);
  byte aft_xor[8];
  for(int i = 0; i<8; i++){
   aft_xor[i] = aft_forw_box[i] ^ iv1[i];
  }
  /*
  Serial.println("");
  for(int i = 0; i<8; i++){
   Serial.printf("%x", aft_xor[i]);
   Serial.print(" ");
  }
  */
  first_3des(aft_xor);
}

void first_3des(byte in[8]){
  byte out[8];
  des.tripleEncrypt(out, in, key);
  /*
  for(int i = 0; i<8; i++){
   Serial.printf("%x", aft_xor[i]);
   Serial.print(" ");
  }
  */
  Forw_S_Box_two(out);
}

void Forw_S_Box_two(byte first_eight[8]){
  byte aft_box[8];
  for (int count; count<4; count++){
  String strOne = "";
  String strTwo = "";
  int i = count * 2;
  int j = count * 2 + 1;
  int fir = first_eight[i];
  int sec = first_eight[j];
  if (fir < 16)
  strOne += 0;
  strOne +=  String(fir, HEX);
  if (sec < 16)
  strTwo += 0;
  strTwo +=  String(sec, HEX);  
  char chars_f[3];
  char chars_s[3];
  strOne.toCharArray(chars_f, 3);
  strTwo.toCharArray(chars_s, 3);
  chars_f[2] = '\0';
  chars_s[2] = '\0';
  int flc = getNum(chars_f[0]);
  int frc = getNum(chars_f[1]);
  int slc = getNum(chars_s[0]);
  int src = getNum(chars_s[1]);
  aft_box[i] = ("%x",Forward_S_Box[flc][frc]);
  aft_box[j] = ("%x",Forward_S_Box[slc][src]);
  }
  /*
  Serial.println("");
  for(int i = 0; i<8; i++){
  Serial.printf("%x",aft_box[i]);
  Serial.print(" ");
  }
  */
  second_xor(aft_box);
}

void second_xor(byte aft_forw_box[8]){
  int stiv2_len = stiv2.length() + 1;
  char iv2[stiv2_len];
  stiv2.toCharArray(iv2, stiv2_len);
  byte aft_xor[8];
  for(int i = 0; i<8; i++){
   aft_xor[i] = aft_forw_box[i] ^ iv2[i];
  }
  /*
  Serial.println("");
  for(int i = 0; i<8; i++){
   Serial.printf("%x",aft_xor[i]);
   Serial.print(" ");
  }
  */
  second_3des(aft_xor);
}

void second_3des(byte in[8]){
  byte out[8];
  des.tripleEncrypt(out, in, key_two);
  /*
  for(int i = 0; i<8; i++){
   Serial.printf("%x",out[i]);
   Serial.print(" ");
  }
  */
  Forw_S_Box_three(out);
}

void Forw_S_Box_three(byte first_eight[8]){
  byte aft_box[8];
  for (int count; count<4; count++){
  String strOne = "";
  String strTwo = "";
  int i = count * 2;
  int j = count * 2 + 1;
  int fir = first_eight[i];
  int sec = first_eight[j];
  if (fir < 16)
  strOne += 0;
  strOne +=  String(fir, HEX);
  if (sec < 16)
  strTwo += 0;
  strTwo +=  String(sec, HEX);  
  char chars_f[3];
  char chars_s[3];
  strOne.toCharArray(chars_f, 3);
  strTwo.toCharArray(chars_s, 3);
  chars_f[2] = '\0';
  chars_s[2] = '\0';
  int flc = getNum(chars_f[0]);
  int frc = getNum(chars_f[1]);
  int slc = getNum(chars_s[0]);
  int src = getNum(chars_s[1]);
  aft_box[i] = ("%x",Forward_S_Box[flc][frc]);
  aft_box[j] = ("%x",Forward_S_Box[slc][src]);
  }
  /*
  Serial.println("");
  for(int i = 0; i<8; i++){
  Serial.printf("%x",aft_box[i]);
  Serial.print(" ");
  }
  */
  third_xor(aft_box);
}

void third_xor(byte aft_forw_box[8]){
  int stiv3_len = stiv3.length() + 1;
  char iv3[stiv3_len];
  stiv3.toCharArray(iv3, stiv3_len);
  byte aft_xor[8];
  for(int i = 0; i<8; i++){
   aft_xor[i] = aft_forw_box[i] ^ iv3[i];
  }
  /*
  for(int i = 0; i<8; i++){
   Serial.printf("%x", aft_xor[i]);
   Serial.print(" ");
  }
  */
  third_3des(aft_xor);
}

void third_3des(byte in[8]){
  byte out[8];
  des.tripleEncrypt(out, in, key_three);
  /*
  for(int i = 0; i<8; i++){
   Serial.printf("%x", out[i]);
   Serial.print(" ");
  }
  */
  Forw_S_Box_four(out);
}

void Forw_S_Box_four(byte first_eight[8]){
  byte aft_box[8];
  for (int count; count<4; count++){
  String strOne = "";
  String strTwo = "";
  int i = count * 2;
  int j = count * 2 + 1;
  int fir = first_eight[i];
  int sec = first_eight[j];
  if (fir < 16)
  strOne += 0;
  strOne +=  String(fir, HEX);
  if (sec < 16)
  strTwo += 0;
  strTwo +=  String(sec, HEX);  
  char chars_f[3];
  char chars_s[3];
  strOne.toCharArray(chars_f, 3);
  strTwo.toCharArray(chars_s, 3);
  chars_f[2] = '\0';
  chars_s[2] = '\0';
  int flc = getNum(chars_f[0]);
  int frc = getNum(chars_f[1]);
  int slc = getNum(chars_s[0]);
  int src = getNum(chars_s[1]);
  aft_box[i] = ("%x",Forward_S_Box[flc][frc]);
  aft_box[j] = ("%x",Forward_S_Box[slc][src]);
  }
  fourth_xor(aft_box);
  /*
  for(int i = 0; i<8; i++){
   Serial.printf("%x", aft_box[i]);
   Serial.print(" ");
  }
  */
}

void fourth_xor(byte aft_forw_box[8]){
  int stiv4_len = stiv4.length() + 1;
  char iv4[stiv4_len];
  stiv4.toCharArray(iv4, stiv4_len);
  byte aft_xor[8];
  for(int i = 0; i<8; i++){
   aft_xor[i] = aft_forw_box[i] ^ iv4[i];
  }
  /*
  Serial.println("");
  for(int i = 0; i<8; i++){
   Serial.printf("%x", aft_xor[i]);
   Serial.print(" ");
  }
  */
  
  for(int i = 0; i<8; i++){
  if(aft_xor[i]<16)
   Serial.print("0");
  Serial.print(aft_xor[i],HEX);
  }
  
  fourth_xor_dec(aft_xor);
}

//////////// Encrypt END ////////////

//////////// Decrypt Start ////////////

void split_dec(char ct[], int ct_len, int p, int n){
  int br = false;
  byte res[] = {0, 0, 0, 0, 0, 0, 0, 0};
  for (int i = 0; i < 16; i+=2){
    if(i+p > ct_len - 1){
      br = true;
      break;
    }
    if (i == 0){
    if(ct[i+p] != 0 && ct[i+p+1] != 0)
    res[i] = 16*getNum(ct[i+p])+getNum(ct[i+p+1]);
    if(ct[i+p] != 0 && ct[i+p+1] == 0)
    res[i] = 16*getNum(ct[i+p]);
    if(ct[i+p] == 0 && ct[i+p+1] != 0)
    res[i] = getNum(ct[i+p+1]);
    if(ct[i+p] == 0 && ct[i+p+1] == 0)
    res[i] = 0;
    }
    else{
    if(ct[i+p] != 0 && ct[i+p+1] != 0)
    res[i/2] = 16*getNum(ct[i+p])+getNum(ct[i+p+1]);
    if(ct[i+p] != 0 && ct[i+p+1] == 0)
    res[i/2] = 16*getNum(ct[i+p]);
    if(ct[i+p] == 0 && ct[i+p+1] != 0)
    res[i/2] = getNum(ct[i+p+1]);
    if(ct[i+p] == 0 && ct[i+p+1] == 0)
    res[i/2] = 0;
    }
  }
    if(br == false){
    if(n!=5)
    decryptivs(res,n);
    else
    fourth_xor_dec(res);
    }
    /*
    for (int i = 0; i < 8; i++){
      int x = res[i];
      Serial.print(x);
      Serial.print(" ");
    }
    */
}

void decryptivs(byte ct[], int n){
  byte out[8];
  des.tripleDecrypt(out, ct, ivkey);
  /*for (int i = 0; i < 8; i++){
     char x = out[i];
     Serial.print(x);
  }*/
  if(n == 1){
   stiv1 = "";
   for (int i = 0; i < 8; i++){
    stiv1 += char(out[i]);
   }
  }
  if(n == 2){
   stiv2 = "";
   for (int i = 0; i < 8; i++){
    stiv2 += char(out[i]);
   }
  }
  if(n == 3){
   stiv3 = "";
   for (int i = 0; i < 8; i++){
    stiv3 += char(out[i]);
   }
  }
  if(n == 4){
   stiv4 = "";
   for (int i = 0; i < 8; i++){
    stiv4 += char(out[i]);
   }
  }
}

void fourth_xor_dec(byte aft_forw_box[8]){
  int stiv4_len = stiv4.length() + 1;
  char iv4[stiv4_len];
  stiv4.toCharArray(iv4, stiv4_len);
  byte aft_xor[8];
  for(int i = 0; i<8; i++){
   aft_xor[i] = aft_forw_box[i] ^ iv4[i];
  }
  /*
  Serial.println("");
  for(int i = 0; i<8; i++){
   Serial.print(aft_xor[i]);
   Serial.print(" ");
  }
  */
  Inverse_S_Box_Four(aft_xor); // Decryption
}

void Inverse_S_Box_Four(byte aft_forw_box[8]){
  byte aft_inv_box[8];
  for (int count; count<4; count++){
  String strOne = "";
  String strTwo = "";
  int i = count * 2;
  int j = count * 2 + 1;
  int fir = aft_forw_box[i];
  int sec = aft_forw_box[j];
  if (fir < 16)
  strOne += 0;
  strOne +=  String(fir, HEX);
  if (sec < 16)
  strTwo += 0;
  strTwo +=  String(sec, HEX);  
  /*
  Serial.print(strOne);
  Serial.println("");
  Serial.print(strTwo);
  Serial.println("");
  */
  char chars_f[3];
  char chars_s[3];
  strOne.toCharArray(chars_f, 3);
  strTwo.toCharArray(chars_s, 3);
  chars_f[2] = '\0';
  chars_s[2] = '\0';
  /*
  Serial.print(chars_f[0]);
  Serial.print(chars_f[1]);
  Serial.println("");
  Serial.print(chars_s[0]);
  Serial.print(chars_s[1]);
  Serial.println("");
  */
  int flc = getNum(chars_f[0]);
  int frc = getNum(chars_f[1]);
  int slc = getNum(chars_s[0]);
  int src = getNum(chars_s[1]);
  /*
  Serial.printf("%x",Inv_S_Box[flc][frc]);
  Serial.print(" ");
  Serial.printf("%x",Inv_S_Box[slc][src]);
  Serial.print(" ");
  */
  aft_inv_box[i] = ("%x",Inv_S_Box[flc][frc]);
  aft_inv_box[j] = ("%x",Inv_S_Box[slc][src]);
  }
  /*
  Serial.println("");
  for(int i = 0; i<8; i++){
  Serial.printf("%x",aft_inv_box[i]);
  Serial.print(" ");
  }
  */
  third_3des_dec(aft_inv_box);
}

void third_3des_dec(byte in[8]){
  byte out[8];
  des.tripleDecrypt(out, in, key_three);
  /*
  for(int i = 0; i<8; i++){
   Serial.printf("%x", out[i]);
   Serial.print(" ");
  }
  */
  third_xor_dec(out);
}

void third_xor_dec(byte aft_forw_box[8]){
  int stiv3_len = stiv3.length() + 1;
  char iv3[stiv3_len];
  stiv3.toCharArray(iv3, stiv3_len);
  byte aft_xor[8];
  for(int i = 0; i<8; i++){
   aft_xor[i] = aft_forw_box[i] ^ iv3[i];
  }
  /*
  for(int i = 0; i<8; i++){
   Serial.printf("%x", aft_xor[i]);
   Serial.print(" ");
  }
  */
  Inverse_S_Box_Three(aft_xor); // Decryption
}

void Inverse_S_Box_Three(byte aft_forw_box[8]){
  byte aft_inv_box[8];
  for (int count; count<4; count++){
  String strOne = "";
  String strTwo = "";
  int i = count * 2;
  int j = count * 2 + 1;
  int fir = aft_forw_box[i];
  int sec = aft_forw_box[j];
  if (fir < 16)
  strOne += 0;
  strOne +=  String(fir, HEX);
  if (sec < 16)
  strTwo += 0;
  strTwo +=  String(sec, HEX);  
  /*
  Serial.print(strOne);
  Serial.println("");
  Serial.print(strTwo);
  Serial.println("");
  */
  char chars_f[3];
  char chars_s[3];
  strOne.toCharArray(chars_f, 3);
  strTwo.toCharArray(chars_s, 3);
  chars_f[2] = '\0';
  chars_s[2] = '\0';
  /*
  Serial.print(chars_f[0]);
  Serial.print(chars_f[1]);
  Serial.println("");
  Serial.print(chars_s[0]);
  Serial.print(chars_s[1]);
  Serial.println("");
  */
  int flc = getNum(chars_f[0]);
  int frc = getNum(chars_f[1]);
  int slc = getNum(chars_s[0]);
  int src = getNum(chars_s[1]);
  /*
  Serial.printf("%x",Inv_S_Box[flc][frc]);
  Serial.print(" ");
  Serial.printf("%x",Inv_S_Box[slc][src]);
  Serial.print(" ");
  */
  aft_inv_box[i] = ("%x",Inv_S_Box[flc][frc]);
  aft_inv_box[j] = ("%x",Inv_S_Box[slc][src]);
  }
  /*
  Serial.println("");
  for(int i = 0; i<8; i++){
   Serial.printf("%x",aft_inv_box[i]);
   Serial.print(" ");
  }
  */
  second_3des_dec(aft_inv_box);
}

void second_3des_dec(byte in[8]){
  byte out[8];
  des.tripleDecrypt(out, in, key_two);
  /*
  for(int i = 0; i<8; i++){
   Serial.printf("%x",out[i]);
   Serial.print(" ");
  }
  */
  second_xor_dec(out);
}

void second_xor_dec(byte aft_forw_box[8]){
  int stiv2_len = stiv2.length() + 1;
  char iv2[stiv2_len];
  stiv2.toCharArray(iv2, stiv2_len);
  byte aft_xor[8];
  for(int i = 0; i<8; i++){
   aft_xor[i] = aft_forw_box[i] ^ iv2[i];
  }
  /*
  Serial.println("");
  for(int i = 0; i<8; i++){
   Serial.printf("%x",aft_xor[i]);
   Serial.print(" ");
  }
  */
  Inverse_S_Box_Two(aft_xor);
}

void Inverse_S_Box_Two(byte aft_forw_box[8]){
  byte aft_inv_box[8];
  for (int count; count<4; count++){
  String strOne = "";
  String strTwo = "";
  int i = count * 2;
  int j = count * 2 + 1;
  int fir = aft_forw_box[i];
  int sec = aft_forw_box[j];
  if (fir < 16)
  strOne += 0;
  strOne +=  String(fir, HEX);
  if (sec < 16)
  strTwo += 0;
  strTwo +=  String(sec, HEX);  
  /*
  Serial.print(strOne);
  Serial.println("");
  Serial.print(strTwo);
  Serial.println("");
  */
  char chars_f[3];
  char chars_s[3];
  strOne.toCharArray(chars_f, 3);
  strTwo.toCharArray(chars_s, 3);
  chars_f[2] = '\0';
  chars_s[2] = '\0';
  /*
  Serial.print(chars_f[0]);
  Serial.print(chars_f[1]);
  Serial.println("");
  Serial.print(chars_s[0]);
  Serial.print(chars_s[1]);
  Serial.println("");
  */
  int flc = getNum(chars_f[0]);
  int frc = getNum(chars_f[1]);
  int slc = getNum(chars_s[0]);
  int src = getNum(chars_s[1]);
  /*
  Serial.printf("%x",Inv_S_Box[flc][frc]);
  Serial.print(" ");
  Serial.printf("%x",Inv_S_Box[slc][src]);
  Serial.print(" ");
  */
  aft_inv_box[i] = ("%x",Inv_S_Box[flc][frc]);
  aft_inv_box[j] = ("%x",Inv_S_Box[slc][src]);
  }
  /*
  Serial.println("");
  for(int i = 0; i<8; i++){
   Serial.printf("%x",aft_inv_box[i]);
   Serial.print(" ");
  }
  */
  first_3des_dec(aft_inv_box);
}

void first_3des_dec(byte in[8]){
  byte out[8];
  des.tripleDecrypt(out, in, key);
  /*
  for(int i = 0; i<8; i++){
   Serial.print(out[i]);
   Serial.print(" ");
  }
  */
  first_xor_dec(out);
}

void first_xor_dec(byte aft_forw_box[8]){
  int stiv1_len = stiv1.length() + 1;
  char iv1[stiv1_len];
  stiv1.toCharArray(iv1, stiv1_len);
  byte aft_xor[8];
  for(int i = 0; i<8; i++){
   aft_xor[i] = aft_forw_box[i] ^ iv1[i];
  }
  /*
  Serial.println("");
  for(int i = 0; i<8; i++){
   Serial.print(aft_xor[i]);
   Serial.print(" ");
  }
  */
  Inverse_S_Box_one(aft_xor);
}

void Inverse_S_Box_one(byte aft_forw_box[8]){
  byte aft_inv_box[8];
  for (int count; count<4; count++){
  String strOne = "";
  String strTwo = "";
  int i = count * 2;
  int j = count * 2 + 1;
  int fir = aft_forw_box[i];
  int sec = aft_forw_box[j];
  if (fir < 16)
  strOne += 0;
  strOne +=  String(fir, HEX);
  if (sec < 16)
  strTwo += 0;
  strTwo +=  String(sec, HEX);  
  /*
  Serial.print(strOne);
  Serial.println("");
  Serial.print(strTwo);
  Serial.println("");
  */
  char chars_f[3];
  char chars_s[3];
  strOne.toCharArray(chars_f, 3);
  strTwo.toCharArray(chars_s, 3);
  chars_f[2] = '\0';
  chars_s[2] = '\0';
  /*
  Serial.print(chars_f[0]);
  Serial.print(chars_f[1]);
  Serial.println("");
  Serial.print(chars_s[0]);
  Serial.print(chars_s[1]);
  Serial.println("");
  */
  int flc = getNum(chars_f[0]);
  int frc = getNum(chars_f[1]);
  int slc = getNum(chars_s[0]);
  int src = getNum(chars_s[1]);
  /*
  Serial.printf("%x",Inv_S_Box[flc][frc]);
  Serial.print(" ");
  Serial.printf("%x",Inv_S_Box[slc][src]);
  Serial.print(" ");
  */
  aft_inv_box[i] = ("%x",Inv_S_Box[flc][frc]);
  aft_inv_box[j] = ("%x",Inv_S_Box[slc][src]);
  }
  /*
  for(int i = 0; i<8; i++){
   Serial.printf("%x",aft_inv_box[i]);
   Serial.print(" ");
  }
  */
  for(int i = 0; i<8; i++){
   dec_stuff += char(aft_inv_box[i]);
  }
}

//////////// Decrypt End ////////////

int getNum(char ch)
{
    int num=0;
    if(ch>='0' && ch<='9')
    {
        num=ch-0x30;
    }
    else
    {
        switch(ch)
        {
            case 'A': case 'a': num=10; break;
            case 'B': case 'b': num=11; break;
            case 'C': case 'c': num=12; break;
            case 'D': case 'd': num=13; break;
            case 'E': case 'e': num=14; break;
            case 'F': case 'f': num=15; break;
            default: num=0;
        }
    }
    return num;
}

void setup() {
  tft.initR(INITR_BLACKTAB);     // Initialize the display
  tft.setRotation(1);
  tft.fillScreen(0x2145);  // Set the background color
  Serial.begin(115200);
  mySerial.begin(9600);
  f = 0;
  in_s = false;
  tft.fillScreen(0x2145); // Clear the screen
}

void loop() {
  if(f<4)
  bus.tick();

  if(in_s == false){
  tft.setTextColor(0xdefb, 0x2145);
  tft.setTextSize(1);
  tft.setCursor(0,5);
  tft.print("Approximate the card N1");
  }
  if (bus.gotData() && f < 4) {
    myStruct data;
    bus.readData(data);

    if(f == 3){
    key_three[0] = data.dt[0];
    key_three[1] = data.dt[1];
    key_three[2] = data.dt[2];
    key_three[3] = data.dt[3];
    f++;
    }

    else if(f == 2){
    key[20] = data.dt[0];
    key[21] = data.dt[1];
    key[21] = data.dt[2];
    key[23] = data.dt[3];
    tft.fillScreen(0x2145); // Clear the screen
    tft.setTextColor(0xdefb, 0x2145);
    tft.setTextSize(1);
    tft.setCursor(0,5);
    tft.print("Approximate the card N4");
    f++;
    }

    else if(f == 1){
    key[4] = data.dt[0];
    key[5] = data.dt[1];
    key[6] = data.dt[2];
    key[7] = data.dt[3];
    tft.fillScreen(0x2145); // Clear the screen
    tft.setTextColor(0xdefb, 0x2145);
    tft.setTextSize(1);
    tft.setCursor(0,5);
    tft.print("Approximate the card N3");
    f++;
    }

    else if(f == 0){
    key[0] = data.dt[0];
    key[1] = data.dt[1];
    key[2] = data.dt[2];
    key[3] = data.dt[3];
    tft.fillScreen(0x2145); // Clear the screen
    tft.setTextColor(0xdefb, 0x2145);
    tft.setTextSize(1);
    tft.setCursor(0,5);
    tft.print("Approximate the card N2");
    in_s = true;
    f++;
    }
    /*
    Serial.print(data.dt[0]);
    Serial.print(" ");
    Serial.print(data.dt[1]);
    Serial.print(" ");
    Serial.print(data.dt[2]);
    Serial.print(" ");
    Serial.print(data.dt[3]);
    Serial.println();
    */
  }

  if(f > 3){
    tft.fillScreen(0x2145); // Clear the screen
    tft.setTextColor(0xdefb, 0x2145);
    tft.setTextSize(1);
    tft.setCursor(0,5);
    tft.print("What do you want to do?");
    tft.setCursor(0,20);
    tft.print("1.Add record");
    tft.setCursor(0,35);
    tft.print("2.Decrypt record");
    Serial.println();
    Serial.println("What do you want to do?");
    Serial.println("1.Add record");
    Serial.println("2.Decrypt record");
    while (!Serial.available()) {}
    int x = Serial.parseInt();
    if(x == 1){
    tft.fillScreen(0x2145); // Clear the screen
    tft.setTextColor(0xdefb, 0x2145);
    tft.setTextSize(1);
    tft.setCursor(0,5);
    tft.print("Enter the website:");
    Serial.println("Enter the website:");
    String str;
    while (!Serial.available()) {}
    str = Serial.readString();
    int str_len = str.length() + 1;
    // Prepare the character array (the buffer) 
    char char_array[str_len];
    str.toCharArray(char_array, str_len);
    Generate_IVs();
    split_by_eight(char_array, 0, str_len);
    if(str_len > 9){
    Generate_IVs();
    split_by_eight(char_array, 8, str_len);
    }
    
    if(str_len > 17){
    Generate_IVs();
    split_by_eight(char_array, 16, str_len);
    }

    if(str_len > 25){
    Generate_IVs();
    split_by_eight(char_array, 24, str_len);
    }
    
    if(str_len > 33){
    Generate_IVs();
    split_by_eight(char_array, 32, str_len);
    }

    if(str_len > 41){
    Generate_IVs();
    split_by_eight(char_array, 40, str_len);
    }
    Serial.println();
    Serial.print("Encrypted:");
    Serial.println(dec_stuff);
    dec_stuff = "";
    
    tft.fillScreen(0x2145); // Clear the screen
    tft.setTextColor(0xdefb, 0x2145);
    tft.setTextSize(1);
    tft.setCursor(0,5);
    tft.print("Enter the login:");
    Serial.println("Enter the login:");
    str="";
    while (!Serial.available()) {}
    str = Serial.readString();
    int str_len1 = str.length() + 1;
    // Prepare the character array (the buffer) 
    char char_array1[str_len1];
    str.toCharArray(char_array1, str_len1);
    Generate_IVs();
    split_by_eight(char_array1, 0, str_len1);
    if(str_len1 > 9){
    Generate_IVs();
    split_by_eight(char_array1, 8, str_len1);
    }
    
    if(str_len1 > 17){
    Generate_IVs();
    split_by_eight(char_array1, 16, str_len1);
    }

    if(str_len1 > 25){
    Generate_IVs();
    split_by_eight(char_array1, 24, str_len1);
    }
    
    if(str_len1 > 33){
    Generate_IVs();
    split_by_eight(char_array1, 32, str_len1);
    }

    if(str_len1 > 41){
    Generate_IVs();
    split_by_eight(char_array1, 40, str_len1);
    }
    Serial.println();
    Serial.print("Encrypted:");
    Serial.println(dec_stuff);
    dec_stuff = "";

    tft.fillScreen(0x2145); // Clear the screen
    tft.setTextColor(0xdefb, 0x2145);
    tft.setTextSize(1);
    tft.setCursor(0,5);
    tft.print("Enter the password:");
    Serial.println("Enter the password:");
    str="";
    while (!Serial.available()) {}
    str = Serial.readString();
    int str_len2 = str.length() + 1;
    // Prepare the character array (the buffer) 
    char char_array2[str_len2];
    str.toCharArray(char_array2, str_len2);
    Generate_IVs();
    split_by_eight(char_array2, 0, str_len2);
    if(str_len2 > 9){
    Generate_IVs();
    split_by_eight(char_array2, 8, str_len2);
    }
    
    if(str_len2 > 17){
    Generate_IVs();
    split_by_eight(char_array2, 16, str_len2);
    }

    if(str_len2 > 25){
    Generate_IVs();
    split_by_eight(char_array2, 24, str_len2);
    }
    
    if(str_len2 > 33){
    Generate_IVs();
    split_by_eight(char_array2, 32, str_len2);
    }

    if(str_len2 > 41){
    Generate_IVs();
    split_by_eight(char_array2, 40, str_len2);
    }
    Serial.println();
    Serial.print("Encrypted:");
    Serial.println(dec_stuff);
    dec_stuff = "";
  }
    if(x == 2){
      tft.fillScreen(0x2145); // Clear the screen
      dec_stuff = "";
      String ct;
      tft.setTextColor(0xdefb, 0x2145);
      tft.setTextSize(1);
      tft.setCursor(0,4);
      tft.print("Website:");
      tft.setCursor(0,49);
      tft.print("Login:");
      tft.setCursor(0,94);
      tft.print("Password:");
      tft.setTextColor(0xffff, 0x2145);
      tft.setTextSize(1);
      Serial.println("Paste the ciphertext containing website:");
      while (!Serial.available()) {}
      ct = Serial.readString();
      int ct_len = ct.length() + 1;
      char ct_array[ct_len];
      ct.toCharArray(ct_array, ct_len); 
      int ext = 0;
      split_dec(ct_array, ct_len, 0+ext, 1);
      split_dec(ct_array, ct_len, 16+ext, 2);
      split_dec(ct_array, ct_len, 32+ext, 3);
      split_dec(ct_array, ct_len, 48+ext, 4);
      split_dec(ct_array, ct_len, 64+ext, 5);
      if(ct_len > 80){
        ext = 80;
        split_dec(ct_array, ct_len, 0+ext, 1);
        split_dec(ct_array, ct_len, 16+ext, 2);
        split_dec(ct_array, ct_len, 32+ext, 3);
        split_dec(ct_array, ct_len, 48+ext, 4);
        split_dec(ct_array, ct_len, 64+ext, 5);
      }
      if(ct_len > 160){
        ext = 160;
        split_dec(ct_array, ct_len, 0+ext, 1);
        split_dec(ct_array, ct_len, 16+ext, 2);
        split_dec(ct_array, ct_len, 32+ext, 3);
        split_dec(ct_array, ct_len, 48+ext, 4);
        split_dec(ct_array, ct_len, 64+ext, 5);
      }
      if(ct_len > 240){
        ext = 240;
        split_dec(ct_array, ct_len, 0+ext, 1);
        split_dec(ct_array, ct_len, 16+ext, 2);
        split_dec(ct_array, ct_len, 32+ext, 3);
        split_dec(ct_array, ct_len, 48+ext, 4);
        split_dec(ct_array, ct_len, 64+ext, 5);
      }
      if(ct_len > 320){
        ext = 320;
        split_dec(ct_array, ct_len, 0+ext, 1);
        split_dec(ct_array, ct_len, 16+ext, 2);
        split_dec(ct_array, ct_len, 32+ext, 3);
        split_dec(ct_array, ct_len, 48+ext, 4);
        split_dec(ct_array, ct_len, 64+ext, 5);
      }
      if(ct_len > 400){
        ext = 400;
        split_dec(ct_array, ct_len, 0+ext, 1);
        split_dec(ct_array, ct_len, 16+ext, 2);
        split_dec(ct_array, ct_len, 32+ext, 3);
        split_dec(ct_array, ct_len, 48+ext, 4);
        split_dec(ct_array, ct_len, 64+ext, 5);
      }
      tft.setCursor(0,19);
      tft.print(dec_stuff);
      Serial.println();
      Serial.print("Website:");
      Serial.println(dec_stuff);
      Serial.println();
      
      String enc_log;
      enc_log="";
      dec_stuff = "";
      Serial.println("Paste the ciphertext containing login:");
      while (!Serial.available()) {}
      enc_log = Serial.readString();
      int ct_two_len = enc_log.length() + 1;
      char ct_two[ct_two_len];
      enc_log.toCharArray(ct_two, ct_two_len); 
      ext = 0;
      split_dec(ct_two, ct_two_len, 0+ext, 1);
      split_dec(ct_two, ct_two_len, 16+ext, 2);
      split_dec(ct_two, ct_two_len, 32+ext, 3);
      split_dec(ct_two, ct_two_len, 48+ext, 4);
      split_dec(ct_two, ct_two_len, 64+ext, 5);
      if(ct_two_len > 80){
        ext = 80;
        split_dec(ct_two, ct_two_len, 0+ext, 1);
        split_dec(ct_two, ct_two_len, 16+ext, 2);
        split_dec(ct_two, ct_two_len, 32+ext, 3);
        split_dec(ct_two, ct_two_len, 48+ext, 4);
        split_dec(ct_two, ct_two_len, 64+ext, 5);
      }
      if(ct_two_len > 160){
        ext = 160;
        split_dec(ct_two, ct_two_len, 0+ext, 1);
        split_dec(ct_two, ct_two_len, 16+ext, 2);
        split_dec(ct_two, ct_two_len, 32+ext, 3);
        split_dec(ct_two, ct_two_len, 48+ext, 4);
        split_dec(ct_two, ct_two_len, 64+ext, 5);
      }
      if(ct_two_len > 240){
        ext = 240;
        split_dec(ct_two, ct_two_len, 0+ext, 1);
        split_dec(ct_two, ct_two_len, 16+ext, 2);
        split_dec(ct_two, ct_two_len, 32+ext, 3);
        split_dec(ct_two, ct_two_len, 48+ext, 4);
        split_dec(ct_two, ct_two_len, 64+ext, 5);
      }
      if(ct_two_len > 320){
        ext = 320;
        split_dec(ct_two, ct_two_len, 0+ext, 1);
        split_dec(ct_two, ct_two_len, 16+ext, 2);
        split_dec(ct_two, ct_two_len, 32+ext, 3);
        split_dec(ct_two, ct_two_len, 48+ext, 4);
        split_dec(ct_two, ct_two_len, 64+ext, 5);
      }
      if(ct_two_len > 400){
        ext = 400;
        split_dec(ct_two, ct_two_len, 0+ext, 1);
        split_dec(ct_two, ct_two_len, 16+ext, 2);
        split_dec(ct_two, ct_two_len, 32+ext, 3);
        split_dec(ct_two, ct_two_len, 48+ext, 4);
        split_dec(ct_two, ct_two_len, 64+ext, 5);
      }
      tft.setCursor(0,64);
      tft.print(dec_stuff);
      Serial.println();
      Serial.print("Login:");
      Serial.println(dec_stuff);
      dec_stuff = "";

      String enc_pass;
      enc_pass="";
      dec_stuff = "";
      Serial.println();
      Serial.println("Paste the ciphertext containing password:");
      while (!Serial.available()) {}
      enc_pass = Serial.readString();
      int ct_three_len = enc_pass.length() + 1;
      char ct_three[ct_three_len];
      enc_pass.toCharArray(ct_three, ct_three_len); 
      ext = 0;
      split_dec(ct_three, ct_three_len, 0+ext, 1);
      split_dec(ct_three, ct_three_len, 16+ext, 2);
      split_dec(ct_three, ct_three_len, 32+ext, 3);
      split_dec(ct_three, ct_three_len, 48+ext, 4);
      split_dec(ct_three, ct_three_len, 64+ext, 5);
      if(ct_three_len > 80){
        ext = 80;
        split_dec(ct_three, ct_three_len, 0+ext, 1);
        split_dec(ct_three, ct_three_len, 16+ext, 2);
        split_dec(ct_three, ct_three_len, 32+ext, 3);
        split_dec(ct_three, ct_three_len, 48+ext, 4);
        split_dec(ct_three, ct_three_len, 64+ext, 5);
      }
      if(ct_three_len > 160){
        ext = 160;
        split_dec(ct_three, ct_three_len, 0+ext, 1);
        split_dec(ct_three, ct_three_len, 16+ext, 2);
        split_dec(ct_three, ct_three_len, 32+ext, 3);
        split_dec(ct_three, ct_three_len, 48+ext, 4);
        split_dec(ct_three, ct_three_len, 64+ext, 5);
      }
      if(ct_three_len > 240){
        ext = 240;
        split_dec(ct_three, ct_three_len, 0+ext, 1);
        split_dec(ct_three, ct_three_len, 16+ext, 2);
        split_dec(ct_three, ct_three_len, 32+ext, 3);
        split_dec(ct_three, ct_three_len, 48+ext, 4);
        split_dec(ct_three, ct_three_len, 64+ext, 5);
      }
      if(ct_three_len > 320){
        ext = 320;
        split_dec(ct_three, ct_three_len, 0+ext, 1);
        split_dec(ct_three, ct_three_len, 16+ext, 2);
        split_dec(ct_three, ct_three_len, 32+ext, 3);
        split_dec(ct_three, ct_three_len, 48+ext, 4);
        split_dec(ct_three, ct_three_len, 64+ext, 5);
      }
      if(ct_three_len > 400){
        ext = 400;
        split_dec(ct_three, ct_three_len, 0+ext, 1);
        split_dec(ct_three, ct_three_len, 16+ext, 2);
        split_dec(ct_three, ct_three_len, 32+ext, 3);
        split_dec(ct_three, ct_three_len, 48+ext, 4);
        split_dec(ct_three, ct_three_len, 64+ext, 5);
      }
      tft.setCursor(0,109);
      tft.print(dec_stuff);
      Serial.println();
      Serial.print("Password:");
      Serial.println(dec_stuff);
      dec_stuff = "";
      Serial.println("");
      Serial.println("Enter any character and press Send to get back to the menu.");
      while (!Serial.available()) {}
    }
  }
}
