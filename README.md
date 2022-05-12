# BenjaminFoster-TeensyDevelopment
To develop and test teensy firmware remotely

## Client shared requirements #1

/////////////////////////////////// 

Backgound information

///////////////////////////////////

info rotary encoder https://au.mouser.com/datasheet/2/678/S22_Kit_Encoder_DS102-1289131.pdf
info "WWW" SK9822 LED https://cdn-shop.adafruit.com/product-files/2351/SK9822_datasheet_SHIJI.pdf
info similar rig https://www.youtube.com/watch?v=UgPQRAZ5OPI

/////////////////////////////////// 

Action List

///////////////////////////////////

<A> // Stepper Calibration
<B> // Stepper Pins
<C> // Pins
<D> // Index Calibration
<E> // Camera trigger point on rotary encoder
<F> // LED Calibration
<G> // Aspect ratio position
<H> // "Image_Cycle"
<I> // "Altitude positions"
<J> // "GeoPosition_Sequence" 
  
/////////////////////////////////// 
  
INITIALISATION PROCESS
  
///////////////////////////////////

00 - From the SD card, load "Preferences.txt" (or .csv) containing all the variables I can easily copy and paste from an excel sheet (From A to J)
00a - From the SD card, load images defined in "Preferences.txt" example "blank.txt,.01.txt,04.txt,07.txt,09.txt". This list varies depending on the 3D scan I want to perform.
NOTE: these files may be too large to be all uploaded on the Teensy all at once. We may have to load them up agian and again throughout the entire scan process, or read them as we go from the SD card. 
00b - "GeoPosition_Sequence" is provided in the "Preferences.txt"
00d - Range "Altitude" from minimum (0mm "00000") to maximum range (200mm "20000") using Altitude_Min and Altitude_Max STOP pins to define range
00d - "AspectRatio" MIN is 0 Altitude_Min if pin HIGH (Landscape). MAX is 1 if Altitude_Max pin HIGH (portrait)
00e - Turn off all LEDs
Note: Other stepper motors will not need software-end calibration like 00d 00e but we do need to incorporate all defined pins listed in "preferences.txt" for particular functions throughout the project
  
/////////////////////////////////// 

INITIALISATION PROCESS

///////////////////////////////////

Setup

01 - Move "Latitude" to "Latitude_South", and set as zero position. ie, when Latitude_South pin reads HIGH
02 - Move "AspectRatio" to "Landscape" or "Portrait". ie, when AspectRatio_Landscape pin or AspectRatio_Portrait reads "HIGH"
03 - Begin spinning arc "Arc" to a continuous defined set speed
04 - Count "RotaryEncoder_Index" to 5 to allow "Arc" to reach nominal speed
05 - Reset "ArcIndex" count to 0 (it's still spinning)
 
///////////////////////////////////

Begin the image capture process

///////////////////////////////////
01 - Count up "RotaryEncoder_Index" and follow "Image_Cycle", "Altitude positions","GeoPosition_Sequence" instructions
02 - Run all the way through sequence "GeoPosition_Sequence"
02a- If in "Landscape" or "Portrait" mode end program and LOW all motors, cameras buttons etc.
02b- if in "LandscapePortrait" mode, this is something I need to work out, but possibly will reverse longitude back to zero, and start the whole process one more time before switching off.
// the end

## Client shared requirements #2

So I'm building a variation of what you saw in the video.
- I will be using one camera and it will swing north to south in a big arc "Latitide" motion (The cameras seen in the video are static)
- my "Longitude" is the post that holds the sunglasses
- "Altitude" refers to a motor that will hold the camera and move it closer or further awaw from the subject
- "Aspect ratio" puts the camera in landscape or portrait position. A motor spins it to either 0 degrees or 90 degrees
- Physical End stops (buttons) are placed at the limits of "Latitide", Aspect ratio" and "altitude". If they signal HIGH then limit is reached. Limits are labelled "Min" or "Max" on the pin allocation
- MCU or IDE? I don't know the difference.
- "3D scan"... For the time being, I am only interested in capturing photos on to the Camera SD card... like a normal digital camera. These are taken elsewhere once the image capturing has been done for batch processing. A completely separate process for now.
- "Geoposition sequence" moves the camera (in preferences.txt) using "latitude" and "longitude" stepper motors. I will list how much (always in the same direction) at specific rotary encoder index values.
- "arcindex" refers to the LED arc swinging around. +1 equals one full rotation. Output at pin 3
- Camera button is #define CameraTrigger 5 // Camera Button. Simply put, the High signal presses the button, Low signal lets go.
- Camera is triggered on or off when the rotary encoder reaches these values
#define TRIG_END 020 // Button Release
#define TRIG_START 980
And while in the "blank.txt.
thinking about it now, i may need to have a startblank.txt a blank.txt and a endblank.txt so that I don't take blank photos

## Complete picture

The prooject is a 3-axes controlled system i.e. Altitude, Latitude and Longitude controls. Each axis is controlled with stepper driver TB6600 amd stepper motor. The system has rotating LED arc that has ~137 RGB LEDs. There are also limiting switches whose purpose is to homing the axis to zero positions. The MCU has SD card attached to it, inside which there are text files that control LED controlling (inensity and color) at each unique encoder position, we call these text files image file. There are also some other text files whose main purpose is to trigger camera on/off at specified encoder position. Client also wants to control latiude, longitude and altitude at each unique encoder position, this will also be probably supplied thorugh SD card (will be inquired in this regard from client). For sample files please go through client requirements.

![image](https://user-images.githubusercontent.com/98542270/168011770-c25e2eac-437b-4c9a-bef3-7e565da65ec9.png)
