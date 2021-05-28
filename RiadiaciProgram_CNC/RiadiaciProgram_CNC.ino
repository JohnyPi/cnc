//Riadiaci program na ovládanie CNC zariadenia.
// Vytvoril Ján Parilla, 2021

#define stepsPerRevolution 200          //vytvorenie a inicializácia
#define dutyCycle 0.35                  //potrebných konštánt a premenných
#define pitch 8
int cycle = 255 * dutyCycle;            //Výpočet duty cycle
int MenuFreq;
long int MenuPeriod;

int stepperX[] = {2, 3, 5, 6};          //vytvorenie polí pinov ovládačov
int stepperY[] = {7, 8, 9, 10};         //jednotlivých krokových motorov
int stepperZ[] = {11, 12, 44, 45};

char in_char;
String mm;
String mmB;
String spd;
boolean stayEnergized = false;

void setup() {
  Serial.begin(9600);                   //Nastavenie sériovej komunikácie na
  Serial.println("Načítavanie...");     //baudrate 9600

  MenuFreq = 200;
  MenuPeriod =(1/(MenuFreq*0.000001))/2;
  Serial.println();
  Serial.print("Frekvencia riadiacich impulzov: ");   //Výpis default nastavení
  Serial.print(MenuFreq);
  Serial.println(" Hz");
  Serial.print("Perióda riadiacich  impulzov: ");   
  Serial.print(MenuPeriod*2);
  Serial.println(" us");
  Serial.print("Stúpanie závitových skrutiek: ");
  Serial.print(pitch);
  Serial.println(" mm2");
  Serial.print("Duty cycle: ");
  Serial.println(dutyCycle);
  Serial.print("Počet krokov na otáčku: ");
  Serial.println(stepsPerRevolution);
  Serial.println();

  int myEraser = 7;                      //vymazanie nastavení časovačov
  TCCR1B &= ~myEraser;                   //jednotlivých PWM výsupov
  TCCR2B &= ~myEraser;
  TCCR3B &= ~myEraser;
  TCCR4B &= ~myEraser;
  TCCR5B &= ~myEraser;

  int myPrescaler = 2;
  TCCR1B |= myPrescaler;                 //nastavenie nových hodnôt
  TCCR2B |= myPrescaler;                 //časovačov na frekveciu 31kHz
  TCCR3B |= myPrescaler;
  TCCR4B |= myPrescaler;
  TCCR5B |= myPrescaler;

  pinMode(stepperX[0], OUTPUT);          //inicializácia výstupných PWM portov
  pinMode(stepperX[1], OUTPUT);
  pinMode(stepperX[2], OUTPUT);
  pinMode(stepperX[3], OUTPUT);

  pinMode(stepperY[0], OUTPUT);
  pinMode(stepperY[1], OUTPUT);
  pinMode(stepperY[2], OUTPUT);
  pinMode(stepperY[3], OUTPUT);

  pinMode(stepperZ[0], OUTPUT);
  pinMode(stepperZ[1], OUTPUT);
  pinMode(stepperZ[2], OUTPUT);
  pinMode(stepperZ[3], OUTPUT);

  Serial.println("Načítavanie ukončené!");
}

void loop() {
  Serial.println();
  Serial.println("...Zvoľte žiadanú operáciu... ");
  Serial.println("Pohyb osí : X - 1, Y - 2, Z - 3");
  Serial.println("Vykreslenie štvorca/obdĺžnika - 4");
  Serial.println("Zmena riadiacej frekvencie - 5");
  Serial.println();
  Menu();                                 //Volanie funcie Menu, pre ovládanie pohybov
  delay(500);                             //jednotlivých osí CNC zariadenia
}

void stepMove(int steps, int speed, int stepperPins[]) { //funkcia obsahujúca algoritmus
  Serial.println();                                      //spínania jednotlivých fáz motora
  if (steps > 0) {                                       //s plným krokom, pohyb v jednom smere
    Serial.println("Posun...");
    for (int i = 0; i < (steps / 4) ; i++) {
      analogWrite(stepperPins[0], cycle);
      digitalWrite(stepperPins[1], LOW);
      digitalWrite(stepperPins[2], LOW);
      analogWrite(stepperPins[3], cycle);
      delayMicroseconds (speed);
      delayMicroseconds (speed);

      digitalWrite(stepperPins[0], LOW);
      analogWrite(stepperPins[1], cycle);
      digitalWrite(stepperPins[2], LOW);
      analogWrite(stepperPins[3], cycle);
      delayMicroseconds (speed);
      delayMicroseconds (speed);

      digitalWrite(stepperPins[0], LOW);
      analogWrite(stepperPins[1], cycle);
      analogWrite(stepperPins[2], cycle);
      digitalWrite(stepperPins[3], LOW);
      delayMicroseconds (speed);
      delayMicroseconds (speed);

      analogWrite(stepperPins[0], cycle);
      digitalWrite(stepperPins[1], LOW);
      analogWrite(stepperPins[2], cycle);
      digitalWrite(stepperPins[3], LOW);
      delayMicroseconds (speed);
      delayMicroseconds (speed);

    }
  }
  if (steps < 0) {                                      //pohyb v opačnom smere
    steps = steps * (-1);
    Serial.println("Posun...");
    for (int i = 0; i < (steps / 4) ; i++) {

      analogWrite(stepperPins[0], cycle);
      digitalWrite(stepperPins[1], LOW);
      analogWrite(stepperPins[2], cycle);
      digitalWrite(stepperPins[3], LOW);
      delayMicroseconds (speed);
      delayMicroseconds (speed);

      digitalWrite(stepperPins[0], LOW);
      analogWrite(stepperPins[1], cycle);
      analogWrite(stepperPins[2], cycle);
      digitalWrite(stepperPins[3], LOW);
      delayMicroseconds (speed);
      delayMicroseconds (speed);

      digitalWrite(stepperPins[0], LOW);
      analogWrite(stepperPins[1], cycle);
      digitalWrite(stepperPins[2], LOW);
      analogWrite(stepperPins[3], cycle);
      delayMicroseconds (speed);
      delayMicroseconds (speed);

      analogWrite(stepperPins[0], cycle);
      digitalWrite(stepperPins[1], LOW);
      digitalWrite(stepperPins[2], LOW);
      analogWrite(stepperPins[3], cycle);
      delayMicroseconds (speed);
      delayMicroseconds (speed);
    }
  }
  if (stayEnergized == false) {                      //podmienka riadiaca napájanie fáz motora
    digitalWrite(stepperPins[0], LOW);               //aj po ukončení pohybu
    digitalWrite(stepperPins[1], LOW);
    digitalWrite(stepperPins[2], LOW);
    digitalWrite(stepperPins[3], LOW);
  }
  Serial.println("Posun ukončený!");
  return;
}


void Menu() {
  while (Serial.available() >= 0) {                  //Výber z menu pomocou sériovej komunikácie
    in_char = Serial.read();
    if (in_char == '1') {                            //Ovládanie motora osy X
      Serial.println("IN X");
      Serial.println("Zadajte počet mm");
      while (Serial.available() == 0) {}
      mm = Serial.readString();
      float Imm = mm.toFloat();
      int steps = mmToSteps(Imm);
      stayEnergized = false;
      stepMove(steps, MenuPeriod, stepperX);
      break;
    }
    if (in_char == '2') {                           //Ovládanie motora osy Y
      Serial.println("IN Y");
      Serial.println("Zadajte počet mm");
      while (Serial.available() == 0) {}
      mm = Serial.readString();
      float Imm = mm.toFloat();
      int steps = mmToSteps(Imm);
      stayEnergized = false;
      stepMove(steps, MenuPeriod, stepperY);
      break;
    }
    if (in_char == '3') {                           //Ovládanie motora osy Z
      Serial.println("IN Z");
      Serial.println("Zadajte počet mm");
      while (Serial.available() == 0) {}
      mm = Serial.readString();
      float Imm = mm.toFloat();
      int steps = mmToSteps(Imm);
      steps = steps * (-1);
      stayEnergized = false;
      stepMove(steps, MenuPeriod, stepperZ);
      break;
    }
    if (in_char == '4') {                           //Vykreslenie štvorca/obdĺžnika
      Serial.println("IN OBD");
      Serial.println("Zadajte veľkosť strany A v mm");
      while (Serial.available() == 0) {}
      mm = Serial.readString();
      float Imm = mm.toFloat();

      Serial.println("Zadajte veľkosť strany B v mm");
      while (Serial.available() == 0) {}
      mmB = Serial.readString();
      float ImmB = mmB.toFloat();

      int steps = mmToSteps(Imm);
      int stepsB = mmToSteps(ImmB);
      stayEnergized = false;
      stepMove(steps, MenuPeriod, stepperX);
      stepMove(stepsB, MenuPeriod, stepperY);
      stepMove(-steps, MenuPeriod, stepperX);
      stepMove(-stepsB, MenuPeriod, stepperY);
      break;
    }
    if (in_char == '5') {                          //Možnosť zmeny rýchlosti pohybu
      Serial.println("IN SPD");
      Serial.println("Zadajte žiadanú fekvenciu impulzov v rozmedzí 40 až 1100");
      while (Serial.available() == 0) {}
      spd = Serial.readString();
      int Ispd  = spd.toInt();
      if (Ispd < 40 || Ispd > 1100) {
        Serial.print("Neplatná hodnota frekvencie!");
      } else {
        MenuFreq = Ispd;
        MenuPeriod =(1/(MenuFreq*0.000001))/2;
        Serial.print("Frekvencia impulzov bola zmenená na hodnotu: ");
        Serial.println(MenuFreq);
        Serial.println(MenuPeriod*2);
      }
      break;
    }
    flushRec();
    delay(20);

  }

}

void flushRec() {                                 //funkcia na vymazanie nepotrebnej
  while (Serial.available())                      //prijímanej komunikácie
    Serial.read();
}
int mmToSteps(float mm) {                         //Funkcia na prevod žiadaného
  //1 ot = 8mm                                    //počtu milimetrov na potrebné kroky motora
  int NeededSteps = (mm * stepsPerRevolution) / pitch;
  Serial.print("Vypočítaný počet potrebných krokov: ");
  Serial.println(abs(NeededSteps));
  return NeededSteps;
}
