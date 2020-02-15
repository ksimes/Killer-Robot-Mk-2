#Explorer Sensor

This is the base template to be used to create new Arduino Nano sensor programs.
The code follows the standard Arduino layout of Setup and Loop routines but it uses
and addition library of ArduinoJSON written by Benoit Blanchon and using a
MIT License. More details and documentation can be found at arduinojson.org

This 3rd party library is used so that JSON is properly output from this lowest level 
processing, picking up data from the sensors, back to the first level controller 
written in Java. 

The embedded Messages library is here to handle serial communications comming
in from the first level controller to the microprocessor. Messages comming in
should be processed in the loop routine.  
