# ESP8266 Audio Output

This is a simple Arduino sketch demonstrating how to produce analog audio
output using the ESP8266's I2S interface.

Large portions of this code are taken from
https://janostman.wordpress.com/audio-hacking-with-the-esp8266/.

To run this, open the project in the Arduino IDE, compile and upload it to
your ESP8266. Also, provide at least one WAV/PCM file in the data folder and
use something like Tools/ESP8266 Sketch Data Upload to get it into the 
ESP8266's SPIFFS.

Files can be converted using ``sox``:

    sox in.mp3 -b 16 out.wav channels 1 rate 32000 trim 0 =30

This would take the first 30 seconds of ``in.mp3`` and convert it to 16-bit
mono 32 kHz data in ``out.wav``.

The sampling frequency can be adjusted in the code, it is currently set to 32
kHz, so the above ``sox`` command would produce a file that should work. Note
that on my ESP8266 it seems that playback is a bit slower than the given rate.
Increasing the rate slightly, for instance setting it to 32080 Hz for a 32 kHz
file, seems to fix that. However, I'm pretty sure that's not the proper fix
for this and something else is still wrong here ...