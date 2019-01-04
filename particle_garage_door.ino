//
// Automated Garage Door
// Harbert.io - harbert.brad@gmail.com
// 11/02/2018
// Version 1.0
//


// PIN Variables
int relayMain = A0;                         // Analog pin A0 for Main Garage Door Relay
int reedDoorClosed = D2;                    // Digital pin D2 for Door Closed Reed Switch (Bottom)
int reedDoorOpen = D4;                      // Digital pin D4 for Door Open Reed Switch (Top)

//State Variables
int stateReedClosed = HIGH;                 // Initial State of Closed Reed Switch (Bottom)
int stateReedOpen = LOW;                    // Initial State of Open Reed Switch (Top)

String garageState = "";                    // Particle Variable to reflect current state of Garage Door

// Timing Variables
int stateReportTime = 1000;                 // Set Time (milliseconds) to send out a debug message
int doorMovementDelay = 13000;              // Time it takes for door to either open or close + 1 second

// State Machine Variables
int stateTime = 10;                         // Time between successive calls to a state function (10 milliseconds)
int elapsedStateTime = 0;                   // Number of milliseconds spent in the current state
int stateReportTimer = 0;                   // Number of milliseconds spent in the current state

// NOTE: We will start in the DOOR CLOSED state.
int currentState = 0;                     // This is the current state of the state machine

// STATE MACHINE:
//
// A State Machine will be implemented to control the operation of the Garage Door.  
// This will eliminate the need for 'delay' statements and will allow the Garage Door to 
// reverse part way through its opening or closing operations
//
// The states of the state machine are as follows (the number is the identifier of the state)
//
// 0 - DOOR CLOSED
// 1 - DOOR RAISING
// 2 - DOOR OPEN
// 3 - DOOR LOWERING
//
// STATE DETAILS:
//
// 0 - DOOR CLOSED
// This will be the initial state of the Garage Door when the system starts. The reedDoorClosed switch
// will be Closed and the reedDoorOpen switch will be Open
//
// 1 - DOOR RAISING
// When in this state, the door will be raising.  
//
// 2 - DOOR OPEN
// This state is when the Garage Door is Open.  The reedDoorClosed switch
// will be Open and the reedDoorOpen switch will be Closed.
// 
// 3 - DOOR LOWERING
// When in this state, the door will be lowering.  
//
 
void setup() {

  pinMode(relayMain, OUTPUT);                                       // Declare RELAY MAIN as output

  pinMode(reedDoorClosed, INPUT_PULLDOWN);                          // Declare Reed Switch for Closed Door as Input
  pinMode(reedDoorOpen, INPUT_PULLDOWN);                            // Declare Reed Switch for Open Door as Input
  
  Particle.variable("garageState", garageState);                    // Particle Variable for Current State of the Garage Door
  Particle.function("gBtnPress", fxGBtnPress);                      // Particle Function to Open or Close the Garage Door

}

void loop(){
    
// Begin by Reading Values from Sensors
  stateReedClosed = digitalRead(reedDoorClosed);                    // Read value from Reed Switch Closed (BOTTOM)
  stateReedOpen = digitalRead(reedDoorOpen);                        // Read value from Reed Switch Open (TOP)

// Execute the current state 
  switch(currentState) {
    case 0:                                                         // 0 - DOOR CLOSED
      stateDoorClosed();
      break;
    case 1:                                                         // 1 - DOOR RAISING
      stateDoorRaising();
      break;
    case 2:                                                         // 2 - DOOR OPEN
      stateDoorOpen();;
      break;
    case 3:                                                         // 3 - DOOR LOWERING
      stateDoorLowering();
      break;
    default:                                                        // Invalid state, lower the door to get us back to DOOR CLOSED
      branchToState(0);
      break;
  }
  
  delay(stateTime);                                                 // Delay for the state delay time

  elapsedStateTime += stateTime;                                    // Increment the time in the state
  stateReportTimer += stateTime;

  if(stateReportTimer > stateReportTime) {                          // See if it is time to report the state
    stateReportTimer = 0;                                           // Reset timer
  }


}

//////////////////////// STATES ////////////////////////////

//////////////// 0 - DOOR CLOSED ///////////////////
void stateDoorClosed(void)
{
    
    if (garageState != "Door Closed") {                             // Publish Event only if it is different from previous State
      
        Particle.publish("garageState", "Door Closed");
        garageState = "Door Closed";
        
    }

    if (stateReedClosed == HIGH) {                                    // If the Reed Switch Door circuit is Open, start the door raising
      
      branchToState(1);
      
    }

}

//////////// 1 - DOOR RAISING //////////////////////
void stateDoorRaising(void) {
    
    if (garageState != "Door Raising") {                             // Publish Event only if it is different from previous State
      
        Particle.publish("garageState", "Door Raising");
        garageState = "Door Raising";
        
    }
    
    delay(doorMovementDelay);                                       // Delay to allow for the Door to Open or Close
    
    branchToState(2);                                               // Move to Door Open State

}

//////////// 2 - DOOR OPEN /////////////////////////
void stateDoorOpen(void) {
    
    if (garageState != "Door Open") {                               // Publish Event only if it is different from previous State
      
        Particle.publish("garageState", "Door Open");
        garageState = "Door Open";
        
    }
    
    if (stateReedOpen == HIGH) {                                      // If the Reed Switch Door Open circuit is Open, start the door lowering

        branchToState(3);                                           // Move to Door Lowering State

    }

}

//////////// 3 - DOOR LOWERING ////////////////////
void stateDoorLowering(void) {
    
    if (garageState != "Lowering Door") {                            // Publish Event only if it is different from previous State
      
        Particle.publish("garageState", "Lowering Door");
        garageState = "Lowering Door";
        
    }
    
    delay(doorMovementDelay);                                       // Delay to allow for the Door to Open or Close
    
    branchToState(0);                                               // Move to Door Closed State

}

//////////////////////// PARTICLE FUNCTIONS/////////////////

int fxGBtnPress(String command) {
    
    if (command.equalsIgnoreCase("press")) {
        
        digitalWrite(relayMain, HIGH);                              // Simulate Garage Door Button Press for 1 second
        delay(1000);
        digitalWrite(relayMain, LOW);
        
        Particle.publish("garageState", "Button was Pressed");        // Publish to Particle that the Remote Button was Pressed
        
        return 1;
        
    }
    
    return -1;
    
}

///////////////////// HELPER FUNCTIONS /////////////////////

void branchToState(int state) {
  
  currentState = state;                                             // Set the next state
  elapsedStateTime = 0;                                             // Reset the elapsed state time
  
}
