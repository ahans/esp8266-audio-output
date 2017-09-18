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

// Taken from https://github.com/espressif/ESP8266_MP3_DECODER/blob/master/mp3/user/user_main.c
// 2nd order delta-sigma DAC
// See http://www.beis.de/Elektronik/DeltaSigma/DeltaSigma.html for a nice explanation
int sampToI2sDeltaSigma(short s) {
    int x;
    int val = 0;
    int w;
    static int i1v = 0, i2v = 0;
    static int outReg = 0;
    for (x = 0; x < 32; x++) {
        val <<= 1; //next bit
        w = s;
        if (outReg > 0) w -= 32767; else w += 32767; //Difference 1
        w += i1v; i1v = w; //Integrator 1
        if (outReg > 0) w -= 32767; else w += 32767; //Difference 2
        w += i2v; i2v = w; //Integrator 2
        outReg = w;   //register
        if (w > 0) val |= 1; //comparator
    }
    return val;
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
            i2s_write_sample(sampToI2sDeltaSigma(buffer[i]));
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
