#include  <Adafruit_LiquidCrystal.h>
// A que taxa o visor sera atualizado 
// todos os tempos estao vinculados a ela
#define frameRate (round(1000/120)) 
#define BarLength 5
#define delayMenuBtn (200)

int btnStart = 6;
int delayStartBtn = 0;
bool start = 1;
unsigned int score[2]={0,0};
int ballVelocity;
unsigned int ballChange=2;
unsigned int barra1=8;
unsigned int barra2=5;
int PositionBallScreen[2];
int bola[2] = {75,5};
unsigned int direction = 2; // 0 -↖  1-↗  2-↘  3-↙ 
byte bolaChar[8];
byte bar1charUp[8];
byte bar1charDown[8];
byte bar2charUp[8];
byte bar2charDown[8];
Adafruit_LiquidCrystal lcd(0);

void cursorWrite(int barra, byte *barChar1, byte *barChar2, int value){
  int i = 0;
  int j = 0;
   for(i=0;i<8;i++)  *(barChar1+i) = 0x00;
   for(i=0;i<8;i++)  *(barChar2+i) = 0x00;
  if(barra>7 || barra<4){
      if(barra>7){
          for(i=0;i<BarLength;i++)
            *(barChar2+((barra-8)+i)) = value;
    }
    else{
         for(i=0;i<BarLength;i++)
            *(barChar1+(barra+i)) = value;
    }
  }
  else{
      for(i=barra;i<8;i++)
        *(barChar1+i) = value; 
      for(j=0;j<(BarLength-(i-barra));j++)
       *(barChar2+j) = value;
  }
}

void bolaWrite(int *ball, int *PositionBallScreen, byte *bolaChar){
   int linha,coluna;
   int i = 0 ;
   *(PositionBallScreen) =  int((*(ball)/5));
   *(PositionBallScreen+1) =  int((*(ball+1)/8));
   coluna  =  (*(ball))%5;
   linha =  *(ball+1)%8;  

   for( i = 0; i < 8 ; i++)
     *(bolaChar+i) = 0x0;
   switch(coluna){
   case 4 : *(bolaChar+linha) = B00001;break;
   case 3 : *(bolaChar+linha) = B00010;break;
   case 2 : *(bolaChar+linha) = B00100;break;
   case 1 : *(bolaChar+linha) = B01000;break;
   case 0 : *(bolaChar+linha) = B10000;break;
  }
}

void ScreenDisplay(int PositionBallScreen[2]){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.write(byte(0));  
  lcd.setCursor(0,1);
  lcd.write(byte(1)); 
  lcd.setCursor(15,0);
  lcd.write(byte(2));
  lcd.setCursor(15,1);
  lcd.write(byte(3));
  lcd.setCursor(PositionBallScreen[0],PositionBallScreen[1]);
  lcd.write(byte(4));
}

void movBarra(unsigned int &barra, int pin ){
  int variacao;
  variacao = analogRead(pin);
  variacao = map(variacao,0,1023,0,11);
  barra = variacao;
}

int changeDirection(int *ball,unsigned int barra1,unsigned int barra2,unsigned int *direction){
   if(*(ball)==0 || *(ball)==79){
     if(*(ball)==0){
       if( *(ball+1) <= (barra1+BarLength) && *(ball+1)>= barra1 ){
         if( *(ball+1) == 0)
           *direction = 2;
         else if (*(ball+1) == 15)
           *direction = 1;
         else{  
          if(direction == 0)
             *direction = 1;
          else
            *direction =  2;
         }
          return 1;
       }
      else return 0;//perdeu
     }
     if(*(ball)==79){
        if( *(ball+1) <= (barra1+BarLength) && *(ball+1)>= barra2 ){
          if( *(ball+1) == 0)
           *direction = 3;
         else if (*(ball+1) == 15)
           *direction = 0;
          else{
           if(*direction == 2)
              *direction=3;
            else
             *direction =  0;
         }
           return 1;
       }
       else return 3;//perdeu
    }
  }
  else{
      if(*direction <= 1 && *(ball+1)==0){
          if(*direction == 1 ) 
             *direction = 2;
          else 
             *direction = 3;
      }
      else if(*direction > 1 && *(ball+1)==15 ){
         if(*direction == 2 )
            *direction = 1;
         else 
            *direction = 0;
      }
      return 1;
  }
  return 2;// a bola n mudou de direçao
}

void movBall(int *ball, int direction){
   switch (direction){
    case 0: 
        (*(ball))--;
        (*(ball+1))--;
        break;
    case 1:
        (*(ball))++;
        (*(ball+1))--;
        break;
    case 2:
        (*(ball))++;
        (*(ball+1))++;
        break;
    case 3:
        (*(ball))--;
        (*(ball+1))++;
        break;
   }
}


void menuBtn(int btn,bool &state, int &delayCurrent, int delay){
  if(digitalRead(btn) && delayCurrent>delay){
              state = !state;
              delayCurrent = 0;
  }
  else
    delayCurrent += frameRate;
}

void showScore(unsigned int score[]){
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("-----SCORE!-----");
   lcd.setCursor(7,1);
   lcd.print(String(score[0]));
   lcd.print("X");
   lcd.print(String(score[1]));
}

void showVictory(int player){
   lcd.clear();
   lcd.print("Jogador ");
   lcd.print(String(player));
   lcd.print(" Venceu");
}

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(btnStart, OUTPUT);
  lcd.begin(16, 2);
  lcd.setBacklight(1);
}

void loop() {
  if(ballChange == 0 || ballChange == 3){
    if(ballChange == 0)(score[0])++;
      else (score[1])++;
    showScore(score);   
    if(score[0]==3){showVictory(1);score[1]=0;score[0]=0;}
    else if(score[1]==3){showVictory(2);score[1]=0;score[0]=0;}
    ballChange = 2;
    bola[0]=39;bola[1] = 5;
    direction = 2;
    start = false;
  }
  menuBtn(btnStart,start,delayStartBtn,delayMenuBtn);
  if(start){
     while(ballChange != 0 && ballChange != 3){
        menuBtn(btnStart,start,delayStartBtn,delayMenuBtn);
         while(start && ballChange != 0 && ballChange != 3){ 
            menuBtn(btnStart,start,delayStartBtn,delayMenuBtn); 
            ballChange  = changeDirection(&bola[0],barra1,barra2,&direction);                  
            movBall(&bola[0],direction);
            movBarra(barra1, A0);
            movBarra(barra2, A1);
            cursorWrite(barra2 , &bar2charUp[0], &bar2charDown[0] , 0x1);
            cursorWrite(barra1 , &bar1charUp[0], &bar1charDown[0], 0x10);
            bolaWrite(&bola[0],&PositionBallScreen[0],&bolaChar[0]);
            if(PositionBallScreen[0]==0 ){
                if(PositionBallScreen[1] == 0){
                  for(int i = 0 ; i<8;i++){
                    bolaChar[i] += bar1charUp[i];
                  }
                }
                 else{
                   for(int i = 0 ; i<8;i++){
                    bolaChar[i] += bar1charDown[i];
                    Serial.println(bolaChar[i]);
                  }
                }               
             }
            else if(PositionBallScreen[0]==15){
                if(PositionBallScreen[1] == 0){
                  for(int i = 0 ; i<8;i++){
                    bolaChar[i] += bar2charUp[i];
                  }
                }
                  else{
                   for(int i = 0 ; i<8;i++){
                    bolaChar[i] += bar2charDown[i];
                  }
                }               
             }
            lcd.createChar(0,bar1charUp);
            lcd.createChar(1,bar1charDown);
            lcd.createChar(2,bar2charUp);
            lcd.createChar(3,bar2charDown);
            lcd.createChar(4,bolaChar);
            ScreenDisplay(PositionBallScreen);
            delay(frameRate);
         }
     } 
  }
}
