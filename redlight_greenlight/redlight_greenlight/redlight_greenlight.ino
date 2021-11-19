//RED LIGHT, GREEN LIGHT
//How to play:
//Each player claims a Blink on the outer ring. double click to choose team colour. The middle Blink is unclaimed.
//The middle Blink will switch between 2 states- RED and GREEN.
//When the middle blink is Green, every player must spam click their blink as many times as they can. 1 click = 1 point.
//When the middle blink turns Red, every player must stop clicking.
//Clicking a blink while the middle blink is red will delete all your poitns or halve them each time the player clicks
//The winner has the most points after x amount of time/rounds

//Test: the middle blink will flicker slightly- giving a small warning before it turns red.
//Test: There is no warning for when the middle blink turns from red to green.



byte teamHues[6] = {22, 49, 82, 99, 160, 200}; //team colours
byte teamScores[6] = {0, 0, 0, 0, 0, 0}; //scores, also uses teamIndex to line up with team hues
byte teamIndex = 0;

enum gameStates {
  READY, //waiting for game to start, set teams
  REDLIGHT, //Center light, red
  GREENLIGHT, //Center light, green
  GAIN_POINTS,  //player piece, able to gain points when greenlight
  LOSE_POINTS, //player piece, loses half of total points when redlight
  WINNER //player piece, displays win
};
byte mode = READY; //start here

byte clicks;


bool isRippling;

bool isMiddleLight = false; //debug
bool isGreenLight = false;  //debug
bool doesMultiClickStartOnRed;

bool roundOver = false;


#define RED_INTERVAL_MAX 4000
#define RED_INTERVAL_MIN 2000


#define GREEN_INTERVAL_MAX 4000
#define GREEN_INTERVAL_MIN 2000


Timer lightTimer;

Timer ripplingTimer;
#define RIPPLING_INTERVAL 1000


void setup() {
  mode = READY; //start at READY
}


void loop() {


  switch ( mode ) {
    case READY:
      readyLoop();
      break;

    case REDLIGHT:
      setValueSentOnAllFaces(mode);
      redLightLoop();
      break;

    case GREENLIGHT:
      setValueSentOnAllFaces(mode);

      greenLightLoop();
      break;

    case LOSE_POINTS:
      losePointsLoop();
      break;

    case GAIN_POINTS:
      gainPointsLoop();
      break;

    case WINNER:
      break;

  }


}


Color getColorForTeam(byte t) { //returns Team Color
  return makeColorHSB(teamHues[t], 255, 255);
}

void readyLoop() {
  if (buttonDoubleClicked()) { //Change Teams
    teamIndex++;
    setColor(getColorForTeam(teamIndex)); //Set Team
    if (teamIndex >= COUNT_OF(teamHues)) {
      teamIndex = 0;
    }
  }

  if ( buttonLongPressed() ) { //long press middle blink to set to light and START GAME
    mode = REDLIGHT;
    roundOver = true;
    isGreenLight = false;
    isMiddleLight = true; //debug
  }

  FOREACH_FACE( f ) { //checks neighbors for REDLIGHT because the game starts on it. will change to LOSE_POINTS mode if it finds a red light neighbor
    if ( !isValueReceivedOnFaceExpired( f ) ) {
      byte neighbor = getLastValueReceivedOnFace( f );
      bool didNeighborJustChange = didValueOnFaceChange( f );
      if (neighbor == REDLIGHT && didNeighborJustChange) { //if there is a red light neighbor
        mode = LOSE_POINTS; //change self to losing point mode
      }
    }
  }


}

void redLightLoop() {
  if (roundOver == true && isGreenLight == false) { //roundOver is the "timer switch".
    lightTimer.set(RED_INTERVAL_MIN + random(RED_INTERVAL_MAX));
    roundOver = false;
  }
  setColor(RED);
  if (lightTimer.isExpired()) {
    roundOver = true;
    isGreenLight = true;
    mode = GREENLIGHT;
  }
}

void greenLightLoop() {
  if (roundOver == true && isGreenLight == true) {
    lightTimer.set(GREEN_INTERVAL_MIN + random(GREEN_INTERVAL_MAX));
    roundOver = false;
  }
  setColor(GREEN);
  if (lightTimer.getRemaining() > 0 && lightTimer.getRemaining() < 300) {
    ripplingTimer.set(RIPPLING_INTERVAL);
  }

  if (ripplingTimer.isExpired()) {

  }
  else {
    FOREACH_FACE(f) {
      setColorOnFace(makeColorHSB(70, 255, random(50) + 205), f); //GREEN HUE
    }
  }


  if (lightTimer.isExpired()) {
    roundOver = true;
    isGreenLight = false;
    mode = REDLIGHT;
  }
}

void losePointsLoop() {

  if (buttonSingleClicked())
  {
    teamScores[teamIndex] = 0; //add multi-clicks to array of scores that align with team colours
  }

  if (buttonDoubleClicked()) {
    teamScores[teamIndex] = 0;
  }


  if (buttonDown()) {
    teamScores[teamIndex] = 0;
  }

  if(buttonReleased()){
       teamScores[teamIndex] = 0;
    }

  if (teamScores[teamIndex] < 0) { //if value drops below 0 just round it to 0, just in case
    teamScores[teamIndex] = 0;
  }

 



  FOREACH_FACE( f ) {
    if ( !isValueReceivedOnFaceExpired( f ) ) {
      byte neighbor = getLastValueReceivedOnFace( f );
      bool didNeighborJustChange = didValueOnFaceChange( f );
      if (neighbor == GREENLIGHT && didNeighborJustChange) { //if there is a green light neighbor
        mode = GAIN_POINTS; //change mode
      }
    }
  }

  scoreDisplay();

}



void gainPointsLoop() {
  
  FOREACH_FACE( f ) {
    if ( !isValueReceivedOnFaceExpired( f ) ) {
      byte neighbor = getLastValueReceivedOnFace( f );
      bool didNeighborJustChange = didValueOnFaceChange( f );
      if (neighbor == REDLIGHT && didNeighborJustChange) { //if there is a red light neighbor
        mode = LOSE_POINTS; //change mode
      }
    }
  }


  if(buttonReleased()){
       teamScores[teamIndex] += 1;
    }
      



  scoreDisplay();

}


void scoreDisplay() {

  if (teamScores[teamIndex] > 10) {
    setColorOnFace(WHITE, 0);
  }

  else {
    setColorOnFace(getColorForTeam(teamIndex), 0);
  }
  if (teamScores[teamIndex] > 20) {
    setColorOnFace(WHITE, 1);
  }
  else {
    setColorOnFace(getColorForTeam(teamIndex), 1);
  }

  if (teamScores[teamIndex] > 30) {
    setColorOnFace(WHITE, 2);
  }
  else {
    setColorOnFace(getColorForTeam(teamIndex), 2);
  }

  if (teamScores[teamIndex] > 40) {
    setColorOnFace(WHITE, 3);
  }
  else {
    setColorOnFace(getColorForTeam(teamIndex), 3);
  }

  if (teamScores[teamIndex] > 50) {
    setColorOnFace(WHITE, 4);
  }
  else {
    setColorOnFace(getColorForTeam(teamIndex), 4);
  }
  if (teamScores[teamIndex] > 60) {
    setColorOnFace(WHITE, 5);
  }
  else {
    setColorOnFace(getColorForTeam(teamIndex), 5);
  }

}


/*Notes*/



//case: GAIN_POINTS: , checks neighbors for GREENLIGHT, counts a multi-click and adds to score
//also checks neighbors for REDLIGHT, and changes to LOSE_POINTS if it finds one

//case: LOSE_POINTS: checks neighbors for REDLIGHT, checks for clicks and will either half score or delete score
//also checks neighbors for GREENLIGHT, and changes to GAIN_POINTS if it finds one

//case: WINNER: checks all scores of blinks, finds highest, and will show winning animation





//count multi-click, use later
//byte clicks;
//if (buttonMultiClicked())
//{
//  clicks = buttonClickCount();
//}
