#define JOYSTICK1_Y A0
#define JOYSTICK1_X A1
#define JOYSTICK2_Y A2
#define JOYSTICK2_X A3
#define JOYSTICK1_SW 6 //2 !
#define JOYSTICK2_SW 3

#define R_ENCODER_SW  4
#define R_ENCODER_DT  5
#define R_ENCODER_CLK 2 //6 !

#define BUTTON1 7
#define BUTTON2 8
#define BUTTON3 9
#define BUTTON4 10

#define LED 13

unsigned char myMessage[13]; //data

signed char deltaEncoder = 0;
bool etatA;
bool dernierEtatA;
unsigned long tempsA;
bool dernierEtatSW = false;
unsigned long lastMessage = 0;

void setup() {
  Serial.begin(9600);

  pinMode(JOYSTICK1_X, INPUT);
  pinMode(JOYSTICK1_Y, INPUT);
  pinMode(JOYSTICK2_X, INPUT);
  pinMode(JOYSTICK2_Y, INPUT);
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);
  pinMode(BUTTON4, INPUT_PULLUP);
  pinMode(JOYSTICK1_SW, INPUT_PULLUP);
  pinMode(JOYSTICK2_SW, INPUT_PULLUP);
  pinMode(R_ENCODER_SW, INPUT_PULLUP);  // SW ENCODER
  pinMode(R_ENCODER_CLK, INPUT);
  pinMode(R_ENCODER_DT, INPUT);

  pinMode(LED, OUTPUT);

  // état de A au setup
  dernierEtatA = digitalRead(R_ENCODER_CLK);
  dernierEtatSW = digitalRead(R_ENCODER_SW);
  deltaEncoder = 0;
  // memorisation du temps pour eviter des erreurs de changements d'etat
  tempsA = millis();
  lastMessage = millis();
  attachInterrupt(digitalPinToInterrupt(R_ENCODER_CLK), changementA, CHANGE);
}

void loop() {

  if (millis() > lastMessage + 200)
    digitalWrite(LED, (millis()%400) < 150);
  else
    digitalWrite(LED, HIGH);

  if (!dernierEtatSW && digitalRead(R_ENCODER_SW)){
    deltaEncoder += 10;
    dernierEtatSW = true;
  }
  else if (dernierEtatSW && !digitalRead(R_ENCODER_SW))
    dernierEtatSW = false;
  
  char val[1]; //Used to store the incomming message from robot
  val[0] = 0;
  if (Serial.available() > 0)
  {
    Serial.readBytes(val, 1);
    if (val[0] == 65)
    {
      myMessage[0] = 65;
      myMessage[1] = (unsigned char) (analogRead(JOYSTICK1_X)/4);
      myMessage[2] = (unsigned char) (analogRead(JOYSTICK1_Y)/4);
      myMessage[3] = (unsigned char) (analogRead(JOYSTICK2_X)/4);
      myMessage[4] = (unsigned char) (analogRead(JOYSTICK2_Y)/4);
      myMessage[5] = digitalRead(BUTTON1);
      myMessage[6] = digitalRead(BUTTON2);
      myMessage[7] = digitalRead(BUTTON3);
      myMessage[8] = digitalRead(BUTTON4);
      myMessage[9] = digitalRead(JOYSTICK1_SW);
      myMessage[10] = digitalRead(JOYSTICK2_SW);
      myMessage[11] = deltaEncoder;
      myMessage[12] = 0x55;
      deltaEncoder = 0;

      Serial.write(myMessage,13); //Write the serial data
      lastMessage = millis();  //Remember when was the last time you recieved a mess
    }
  }
}

void changementA(){
  etatA = digitalRead(R_ENCODER_CLK);
   
  // controle du temps pour eviter des erreurs 
  if( abs(millis() - tempsA) > 50 ){
    // Si B different de l'ancien état de A alors
    if(digitalRead(R_ENCODER_DT) != dernierEtatA){
      deltaEncoder--;
    }
    else{
      deltaEncoder++;
    }
    // memorisation du temps pour A
    tempsA = millis();
  } 
  // memorisation de l'état de A
  dernierEtatA = etatA ;
}