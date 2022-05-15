
/*
 * I was playing around with your code and did some more googling over the weekend...
 * 
 * PROJECT UPDATES & CHANGES 
 * 1. To simpify the project, I will put the code into an excel sheet that I can then export to .txt (and subsequently change to .ide) so that I will no longer need to import a "preferences.txt".
 * 2. I have also tested the motors and do not need END_Stops on "AspectRatio" or "Altitude" as I set these manually at the begnning of each project. No physical damage will occur if these motors hit the ends either.
 * 3. The end stops on "Latitude" will be the only end stops I use for the entire project.
 * 4. I found <AccelStepper.h>. it appears to be a bit more Ben friendly than the other library and I was able to test the motors. I just need you to get a few more things in it working (see below)
 * 5. I Haven't got a MachineCode.txt file example but I'll want something like the following:
 *      Rotary Index number:        Latitude:                              Longitude:
 *      Value 1,                    Move by set Value Y or nothing,        Move by set Value Z or nothing,
 *      Value 2,                    Move by set Value Y or nothing,        Move by set Value Z or nothing,
 *      Value 3,                    Move by set Value Y or nothing,        Move by set Value Z or nothing,
 */ 

#include <AccelStepper.h> 
// http://edge.rit.edu/edge/P14215/public/Software/P14215_Nick/Code/Arduino%20Library%20Folder/AccelStepper/doc/classAccelStepper.html
// https://www.airspayce.com/mikem/arduino/AccelStepper/

// steppers
// maybe the ENA_Pins can be included in this bit somehow? You'll have to read the library. I don't know if it's possible.
AccelStepper ArcRotate  (AccelStepper::DRIVER, 7, 6);       // Needs some work. What is the difference between "DRIVER" and "FULL2WIRE"? Both seem to work?
// ? #define ArcRotate_ENAPIN        20             // LED Arc Motor Lock
AccelStepper Latitude   (AccelStepper::FULL2WIRE,10,11);    // Needs some work.
// ? #define Longitude_ENAPIN        21             // Longitude Motor Lock
AccelStepper Longitude  (AccelStepper::FULL2WIRE,8,9);      // Needs some work. "Longitude.move(1000)" incremental move not working. Maybe ENAPIN needs to be LOW
// ? #define Latitude_ENAPIN         22             // Latitude Motor Lock
AccelStepper AspectRatio(AccelStepper::FULL2WIRE,12,28);    // Works perfectly to numbers and settings provided
// ? #define AspectRatio_ENAPIN      23             // AspectRatio Motor Lock
AccelStepper Altitude   (AccelStepper::FULL2WIRE,24,25);    // Works perfectly to numbers and settings provided 
// ? #define Altitude_ENAPIN         29             // Altitude Motor Lock


#define Latitude_SOUTH                 // END Stop

int pos = 40000; // only used if ArcRotate is to spin a defined number of times (see void loop) but I don't want to have to use it. I just want ArcRotate to spin 

// Definitions
#define ArcRotateMaxSpeed           400   // 
#define ArcRotateAcceleration       300   //
#define ArcRotateSpeed              400   //
#define ArcRotate_ENAPIN            20    // Needs to spin freely

#define LatitudeMaxSpeed            2000  //
#define LatitudeAcceleration        100   //
#define Latitude_Move               1000  // Increment by which I want it to move
#define Latitude_South              14    // Latitude Min Stop
#define Latitude_North              15    // Latitude Max Stop

#define LongitudeMaxSpeed           2000  //
#define LongitudeAcceleration       1000  //
#define Longitude_Move              1000  // Increment by which I want it to move

#define AspectRatioMaxSpeed         2000  //
#define AspectRatioAcceleration     1000  //
#define AspectRatioLANDSCAPE        0     // Starting position
#define AspectRatio_Move            4000  // Fixed increment so it moves 90 degrees

#define AltitudeMaxSpeed            20000 // 
#define AltitudeSpeed               1500  //
#define AltitudeAcceleration        10000 //
#define Altitude_0mm                0     // START position (where I moved the camera to)
   
void setup()
{ 
    ArcRotate.setMaxSpeed(ArcRotateMaxSpeed);
    ArcRotate.setAcceleration(ArcRotateAcceleration);

    Latitude.setMaxSpeed(LatitudeMaxSpeed);
    Latitude.setAcceleration(LatitudeAcceleration);
    Latitude.moveTo(Latitude_SOUTH);
    Latitude.move(Latitude_Move);
//    Latitude.setToNewPosition(0);  // ?
        
    Longitude.setMaxSpeed(LongitudeMaxSpeed);
    Longitude.setAcceleration(LongitudeAcceleration);
    Longitude.moveTo(Longitude_Move); 

    AspectRatio.setMaxSpeed(AspectRatioMaxSpeed);
    AspectRatio.setAcceleration(AspectRatioAcceleration);
    AspectRatio.moveTo(AspectRatioLANDSCAPE); 
    AspectRatio.move(AspectRatio_Move); 
//    AspectRatio.setToNewPosition(0);  // ?

    Altitude.setMaxSpeed(AltitudeMaxSpeed);
    Altitude.setAcceleration(AltitudeAcceleration);
    Altitude.moveTo(Altitude_0mm);
    Altitude.move(Altitude_Move);
//    Altitude.setToNewPosition(0);  // ?
}

void loop()
{
// not sure about how to just get ArcRotate to spin at a constant speed while the rest of the program is running. 
// Below are two examples. One rotates until "int pos = 40000;" is reached. the latter just makes it spin. 
// Both only if they are the only things in "void loop()"
// I need ArcRotate to run for the entire project until the end where MachineCode.txt tells some motors to move back to 0 before shutting them all down
/* ArcRotate works 
    if (ArcRotate.distanceToGo() == 0)
  {
    delay(500);
    pos = -pos;
    ArcRotate.moveTo(pos);\
  }
  ArcRotate.run();
/*
// or
/* 
   ArcRotate.runSpeed();
*/

   
// Latitude end stops NORTH and SOUTH need to prevent the motor going further at either extremity
// The natural starting position "0" will be when longitude is touching SOUTH. I will have the arm here at the start of every project
// I can't seem to get "Longitude.move(500)" to move by this increment. ".moveTo()" works as does ".runToNewPosition()" but I don't want these. is ENAPIN HIGH?

/*              
AspectRatio.setToNewPosition(0); // ?
Longitude.move(500);
*/


// Longitude just goes round and round in small increments throughout the scan
// I can't seem to get "Longitude.move(1000);" to work. Maybe ENAPin is High or something? 
// "moveTo" works but I don't want that.
/*         
Longitude.move(1000);
*/


// AspectRatio is ok to not have end stops. 
// ZERO will be where I manually position the camera to start off with
// Below is an example of how the camera would move between Landscape and Portrait 
/*
AspectRatio.runToNewPosition(0);      // Landscape position
AspectRatio.runToNewPosition(10000);  // Portrait position
*/

// Altitude is ok to not have end stops. 
// ZERO will be where I manually position the camera to start off with
// Below is an example of how the camera would move sequentially to 4 different positions
/*   Altitude.runToNewPosition(0); 
Altitude.runToNewPosition(50);
Altitude.runToNewPosition(100);
Altitude.runToNewPosition(150);
*/


}
