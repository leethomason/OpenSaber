** Welcome to Open Saber! **

The goal of open saber is provide source code, circuit diagrams, and 
documentation for making an Arduino (and compatible) based lightsaber.
The focus is on the internals of the saber, not how to build the case.

Why? For the joy of the problem, working together, and learning / playing
with Arduinos. There are certainly commercial solutions to saber electronics,
but if you want to build and contribute to the code and internals, this site
is for you!

FIXME saber image

The Open Saber site / repo is 3 things:
- Overview of the (electrical) design of a saber.
- Source code for blade ignition, color, sound, etc. that runs on
  arduino class micro-controllers.
- Documentation & circuit diagrams for circuits and components.

The example platform is the "Gecko" saber, which is a SaberZ Sentris body (the
empty hilt option) with Open Saber electronics.

## Features:
- Support for 3 LED RGB "any color" saber 
- Flash on clash
- Motion and impact detection
- Sound font support
- Constant (average) current to LEDs
- Battery level tracking and indicator
- Command set to program saber with computer
- Recharge port / kill key port

## Resources

I am not affiliated with any of these, but have found them all useful.

### General
- The Custom Saber Shop at http://www.thecustomsabershop.com
  has all things saber. Often just abbreviated TCSS.

### How To
- Genesis Custom Sabers Shop Secrets: 
  https://www.youtube.com/playlist?list=PLBZx-DMVqx7druzwlBkXoZZhd30_0LXNS
  Lots of good info on wiring and techniques.
- "So you want to build a lED saber?" Different design than Open Saber,
  but still a nice overview: 
  http://forums.thecustomsabershop.com/showthread.php?2025-Step-2-I-have-decided-to-make-a-LED-saber

### Saber bodies
- The Custom Saber Shop (http://www.thecustomsabershop.com)
- UltraSabers http://www.ultrasabers.com/, in the "Build your own
  Lightsaber" section.

### LEDs
- LEDSupply. http://www.ledsupply.com/ has some nice RGB high power LEDs.

### Electronics
- Adafruit. I'm a huge Adafruit fan; great electronics and documentation.
  http://adafruit.com/
- Sparkfun. https://www.sparkfun.com/
- Mouser. http://www.mouser.com/ A huge selection of everything electronic.
- DigiKey. http://www.digikey.com/ A huge selection of everything electronic.

## Architecture

An overview of the parts of a saber, with specific examples of the Gecko. 
(But you can & should use your own parts, and send a Pull Request to 
update for other designs!)

Technically speaking, to organize you electrical thoughts, a saber is a *very* 
fancy flashlight. 

FIXME: add photo diagram

In roughly front to back order, an open saber is:

- A **blade holder**. (1" is standard.)
- A **lens** to focus the LED. The Gecko uses a 10511 Carclo Lens.
- 3 (or 4) **LEDs** in the 1-Watt range. Red, green, blue, and sometimes white or
  another color. (Gecko-Sentris uses the  The Gecko uses a "Luxeon Rebel - 
  Endor Star RGB High Power LED"  which fits nicely in the case.)
- A **heat sink** for cooling the LED. Often part of the saber body. 
  TCSS provides them as well.
- **Emitter electronics** bridge from the microcontroller to the high power 
  LEDs. MOSFETs, resister, and capacitors. 
- **Power and Auxillary** switch (momentary on, typically 12mm).
- **Power port**. Typically 2.1 mm; there's are pros and cons of this design. 
  (See below.)
- **Micro controller** which is the "brain" of the saber. Gecko uses an
  Adafruit Pro Trinket 3.3V. Any 3.3V arduino compatible (that fits in the case)
  with enough RAM should work.
- **Accelerometer**. Gecko-Sentris uses an "Adafruit LIS3DH Triple-Axis 
  Accelerometer."
- **Audio output microcontroller**. Gecko uses an Adafruit Audio FX ID 2217, 
  although I'm interested in switching this to a "generic" microcontoller.
- 3.7v Li-Ion **battery**. TCSS and Adafruit both sell good, small, long lasting
  batteries. Need something rated 2000mAh or better.
- **Speaker**. So hard to get a good, small, speaker not broken in shipment. 
  TCSS and Adafruit carry them, as well as Mouser and Digikey.

## Directory Organization

- root
  - src - ardruito source code
    - saber - code for the saber itself
    - lcdController - code for the controller box (completely optional)
  - circuits
    - components - circuit diagrams (as text files) of the different
      sub-systems.
    - pcb - printed circurit board diagrams

## Wiring

### Power Bus
The saber uses a common ground. There are 2 positive power voltages:
- Vbat, the battery power supply. Ranges from about 4.2 - 3.5V. Vbat delivers
  a **lot** of power, up to about 1A (1000mA) to the audio and LEDs. It also
  powers the micro-controllers.
- Vcc, a 3.3V, regulated, constant, low-amp supply. Powers the switch LEDs and 
  other indicators. 

It's very important to not "cross" the power supplies or use the wrong one. 
Vbat is high-power, inconsistent voltage, and Vcc is low-power consistent 
voltage. 

#### Power Port
Convention, and the current design, uses a 2.1mm recharge jack. Wiring of the
jack is such that:
1. the battery charges when the charger is plugged in
2. the circuit is dead when there is a non-conductive kill key plugged in
3. the circuit is live when nothing is plugged in

This is useful, and convention for almost all saber designs, but a little 
wierd. For it to work (safely) for case #1, the grounded sleeve of the recharge 
port should be isolated from "true ground" and the case of the saber. You need 
to be careful with a metal port, or use a insulated plastic one. (I went with
an insulated plastic one.)

#### 3V or 7V
Why a 3.7v supply? I assume you want rechargable batteries. 4 AAA may be 
workable for a single LED (1 watt) but even then, traditional batteries
have low power lifetimes.

Given Li-Ion rechargable battery packs, the question is then 3.7v or 7.4v?

3.7v
- Smaller
- Efficient for resistors on LEDs. A typical LED forward voltage is 2-3 volts,
  so overall efficiency is about 80%.

7.4v
- Efficient for buck-puck drivers. A PWM buck needs a 7V power supply.
  Generally 90% efficient.
- 5V logic is sometimes a little easier to work with if you are stating out
  on a proto/dev board. (Although this is changing, and I find myself 
  prototyping on 3V systems.)

In the end, 3.7v wins primarily for size. (I'd like to try a 7.4V circuit
with bucks, but haven't yet. Sparkfun makes some nice, affordable, low profile 
converters.)

Note that in either case, the actual battery voltage varies significantly, 
and the control circuit has to account for that.

### Emitter Electronics
Basic, but hard to miniturize, circuit. It is documented in LEDControl.txt.
I had it custom printed at PCBExpress - a fab file emmitter.pcb is provided.

FIXME image of circuit

The parts of the circuit are:
1. Capacitor for buffering power to the LEDs.
2. PWM boost from logic current to power current for controller the LED color.
   A MOSFET, de-ring resister, tie down resister, and LED power control 
   resistor for each channel.
3. Switch tie down resistors.
4. Voltage divider for driving the voltage meter. The micro-controller
   will account for Vbat above 3.7 by reducing the modulation on the LEDs,
   thereby maintaining constant average current.

### LED to Emitter Electronics
The LED uses a common anode, as well as 3 control lines: Red, Green, and
Blue. They 4 wires are connected from the LED to the front side of the 
emitter electronics.

### Micro-controller
The 'saber' source code contains the 'pins.h' file, which documents all
the pin connections. The connection groups are:

- Logic connection to the emitter electronics, for controlling the color of the
  LEDs.
- Connection to the (divided) voltage of Vbat, to adjust the LED power.
- Connection to the switch.
- Connection to the switch LEDs. (Indicator lights.)
- Connection to the audio card.
- Vbat / ground in.
- Vcc out.
- LED for battery level.

In the Gecko, the micro-controller FTDI header is exposed
so that the code can be updated, fixed, and debugged.

### Switches

The code assumes 2 switches.
- A: the main switch. Hold for power, tap for battery level indicator.
- B: the auxillary switch. When the blade is on, tap for "blaster effect",
  hold for "lockup effect". When the blade is off, hold to toggle sound.

You may also (when the blade is on) hold auxillary and then tap the main
switch to cycle through the blade color / sound palettes.

The switch plate on the Gecko-Sentris is removable. It has a (regrettably 
complex) set of connections to allow the switch to disconnect. Resistors for
the indicator LEDs are "inlined" in the switch, so they don't need to be part
of the Emitter circuit.

### Audio Board

The audio board is an Adafruit breakout that is run in UART mode. (I 
sometimes feel like I'm over-stretching this board.) It (currently) supports one
sound font, and can recognize the default or crystal focus naming scheme.
Sounds are loaded directly to the board by connecting a micro-usb, and it
mounds as a hard drive.

All the names can be postfixed with a number for random variation: `SWING` or
`SWING01` for example. Note that the card / code is limited to 8.3 filenames.
You can't mix names in a group; using both BLDON and POWERON in the same
sound font will confuse the sound output.

- "BLDON", "POWERON" blade ignition
- "BLDOFF", "POWEROFF" blade retraction
- "IDLE", "HUM" the background drone
- "IMPACT", "CLASH" is the sound of impacting another saber
- "MOTION", "SWING" is the basic saber motion sound
- "USRHOLD", "LOCKUP" is a user defined long sound, typically the sound 
  when 2 sabers are held together.
- "USRTAP", "BLASTER" is a user defined short sound, typically the sound
  of parrying blaster fire.

## Code

The code is really the heart of this project. It is designed around an
Adafruit Trinket Pro 3V driving an Adafruit Audio FX board. 

First you will need to set up libraries. Then there are 2 files you should 
start with to adapt to your saber, 'pins.h' and 'electrical.h'. The code
is set up to compile in the Arduino IDE.

### Libraries

Regrettably, the 'saber' source code uses some forked libraries. They are
all available on the github pages. I regret having to fork - it adds complexity
and overhead - but there are issues I haven't otherwise been able to work
around.

#### Button

The button library provides debounce and hold detection. It's a solid, 
general purpose button library. I wish Button (or something similar) would
become part of the standard Arduino distribution.

https://github.com/leethomason/Button

#### Accelerometer

FIXME: new accelerometer

### AudioFX

The standard library uses quite a bit of memory and is limited to 25 files.
For Open Saber usage, this doesn't work. This fork of the library 
addresses those limitations.

https://github.com/leethomason/Adafruit_Soundboard_library

### General Utility

Just some (simple) code to streamline Arduino programming.

https://github.com/leethomason/Grinliz_Arduino_Util

### Config Files

#### 'pins.h'

'pins.h' contains the wiring pinout, documentation, and features switches.
The tricky #define is the SERIAL_DEBUG macro. If on, you can connect
the FTDI header and debug the saber. However, without an FTDI header, **the 
saber won't work if SERIAL_DEBUG = 1**. The serial port won't be found,
the timeouts kick in, and the saber essentially "locks up". It's a great to
be able to debug, but remember to SERIAL_DEBUG=0 and upload that sketch
when you are done.

Pins contains a bunch of other macros, which you can disable when you 
build the saber, and turn on one by one.

#### 'electrical.h'

'electrical.h' contains the description of the LED: forward voltage, 
resistor values, etc. You need to set this for your particular build.

Note that there is an equation for UVOLT_MULT which controls the volt
meter. There's some variability; once the saber is assembled, I suggest
adjusting this value by checking computed vs. measured values. (Type 'vcc' on 
the command line to get the current computed value.)

### Command line

If you connect an FTDI header, you can open the COM port to the saber and
issue commands. This works irrespective of SERIAL_DEBUG.

The saber has 8 palettes. The palette is a combination of:
- blade color
- impact color
(In the future, sound palette will also be supported.) 

- `pal` returns the current palette. `pal <0-7>` sets the current palette.
- `bc` will return the current blade color for current palette. `bc #rrggbb`
  will set the blade color for the current palette.
- `ic` gets / sets the impact color or "blade flash".
- `aud` and `aud <0-1>` gets and sets audio on/off. Due to some limitations
  of the Audio FX interface, *This only works when the blade is off.*
- `vol` and `vol <0-204>` gets and sets the current volume. *This only works
  when the blade is off.*
- `amp` is the power draw of the blade color (for the current palette) in
  milli-amps. Note that the actual current draw tends to be less - something
  I'm still working out. Nevertheless, you should probably keep the current
  draw at 1000mA or less.
- `vcc' is the current battery level, in milli-volts. 4200 is fresh, 3700 is 
  nominal, 3500 is low. This is read from the Vmeter of the emitter circuit.
- `util` is the current utilization of each channel. It is computed from Vcc.
  `90 90 76` means that the saber will use 90% red, 90% green, and 76% of
  the blue channel. This keeps the average current through the LED constant.
  When Vcc drops to 3.7V, these will all be 100%.
- `pwm` is the current Pulse Width Modulation of the red, green, and blue LEDs.
- `mot` and `mot <1.1+>` gets and sets the threshold, in g-force, that
  the saber will detect as motion, and play the motion sound effects.
- `imp` and `imp <1.1+>` gets and sets the threshold, in g-force, that
  the saber will dectect as impact, and play the impact sound effects.
- `stat` will display all of the current saber settings.
- `reset` will reset the palettes to their default values.

## Future Direction

In general direction, cleaning up the existing solution and adding support
for more chipsets is desirable. Specificially some ideas that are interesting:

- 4 channel LED support. Some LEDs are RGBW, which would be nice to support.
  This is pretty straightforward, just need to adjust the code.
- 1 channel LED support. A more efficient saber - by a lot - only has one
  bright, single color, efficient LED.
- Double blade support. Again, straightforward. Minor code adjustments.
- Twirl detection. The saber can detect motion and impact, but it would be
  great to also be able to detect the twirl / spin motion.
- Improved sound handling. This may mean moving away from the Audio FX (which
  generally is a great little card) to something else. 
  - Sounds fonts. It would be nice to be able to choose between distinctive
    sounds for each palette.
  - Crossfade to remove any "clicks" when switching sounds.
  - Volume adjustment when the blade is on.
  - MicroSD card support. 
- UI guestures. Some sabers detect particular motions for changing palettes,
  going into UI modes, etc. Would be nice to support one-button sabers with
  all the features.

