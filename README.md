# BEA
Bit error analyzer for testing cables used as I2C buses

## Background
Developed for the work on "An Investigation Into The Limits Of The Physical I2C Bus Length"

## How To Build
1. Copy the `bea-common` folder to your Arduino library folder or add a symlink in your Arduino
   library folder to the `bea-common` folder. On Linux, this can be done using
   `ln -s /path/to/bea-common bea-common` when run from the Arduino library folder. If you are
   unsure where it is, open the Arduino IDE and go to `File->Preferences`. It is the `libraries`
   folder under the "Sketchbook location" folder.
2. Build `bea-master` and/or `bea-slave` in the Arduino IDE by clicking "Verify".
