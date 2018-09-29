#include "Grinliz_Arduino_Util.h"
#include "Grinliz_Util.h"

static const int SERIAL_SPEED = 115200;
CStr<200> line;
bool firstLine = true;
int size = 0;
int nRead = 0;
int lastPercent = 0;

void setup()
{
    Serial.begin(115200);  // still need to turn it on in case a command line is connected.
    while (!Serial) {
        delay(200);
    }
    Log.attachSerial(&Serial);

    Log.p("Serial started at ").p(SERIAL_SPEED).eol();
    Log.p("Waiting for file.").eol();
}

void parseSize() {
    Log.p("Linesize=").p(line.size()).eol();
    int mult = 1;
    for(int i=1; i<line.size(); ++i)
        mult *= 10;
    for(int i=0; i<line.size(); ++i) {
        size += mult * (line[i] - '0');
        mult /= 10;
    }
    Log.p("Size=").p(size).eol();
}

void loop()
{
    while(Serial.available()) {
        int b = Serial.read();
        if (b == '\n' || b=='\r') {
            if (!line.empty()) {
                if (firstLine) {
                    parseSize();
                    firstLine = false;
                }
                else {
                    for(int i=0; i<line.size(); i+=2) {
                        int v = hexToDec(line[i]) * 16;
                        v += hexToDec(line[i+1]);
                        Log.p(v).eol();
                        ++nRead;
                        if (nRead == size) {
                            lastPercent = 10;
                            Log.p("100%. Size=").p(nRead).p(" bytes read.").eol();
                        }
                        else {
                            int percent = nRead * 10 / size;
                            if (percent > lastPercent) {
                                lastPercent = percent;
                                Log.p(percent * 10).p("% done.").eol();
                            }
                        }
                    }
                }
            }
            line.clear();
        }
        else {
            line.append(b);
        }
    }
}