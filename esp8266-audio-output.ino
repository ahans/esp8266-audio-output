#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <i2s.h>
#include <i2s_reg.h>

#include <FS.h>
int16_t buffer[512]; 

void setup(void) {
    Serial.begin(9600);
    Serial.println("");
    Serial.println("putting WiFi to sleep");
    WiFi.forceSleepBegin(); // turn off ESP8266 RF
    delay(1); // give RF section time to shutdown
    
    Serial.println("Starting SPIFFS");
    SPIFFS.begin();

    Serial.println("Setup finished");
}

// Pulse Density Modulated 16-bit I2S DAC
uint32_t i2sACC;
uint16_t err;

void writeDAC(uint16_t DAC) {
    for (uint8_t i = 0; i < 32; ++i) {
        i2sACC = i2sACC << 1;
        if (DAC >= err) {
            i2sACC |= 1;
            err += 0xFFFF - DAC;
        } else {
            err -= DAC;
        }
    }
    i2s_write_sample(i2sACC);
}


int rate = 32000;

void play() {
    Serial.println(F("Playing hill32.wav"));
    File f = SPIFFS.open("/hill32.wav", "r");
    if (!f) {
        Serial.println("file open failed");
        return;
    } //- See more at: http://www.esp8266.com/viewtopic.php?f=29&t=8194#sthash.u5P6kDr6.ycI23aTr.dpuf
    
    i2s_begin();
    i2s_set_rate(rate);
    
    while (f.position() < (f.size()-1)) {
        int numBytes = _min(sizeof(buffer), f.size() - f.position() - 1);
        f.readBytes((char*)buffer, numBytes);
        for (int i = 0; i < numBytes / 2; i++) {
            writeDAC(buffer[i] + (0xffff >> 1));
        }
    }
    
    f.close();
    i2s_end();
}

void loop() {
    play();
    #if 0
    while (Serial.available() > 0) {
        char in = Serial.read();
        Serial.print("read ");
        Serial.println(in);
        switch(in) {
            case '+':
                rate += 5;
                break;
            case '-':
                rate -= 5;
                break;
            case 'p':
                play();
                break;
        }
        Serial.print("rate = ");
        Serial.println(rate);
    }
    #endif
}
