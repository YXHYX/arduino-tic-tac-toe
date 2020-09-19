#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>


// Color definitions
#define   BLACK   0x0000
#define   BLUE    0x001F
#define   RED     0xF800
#define   GREEN   0x07E0
#define   CYAN    0x07FF
#define   MAGENTA 0xF81F
#define   YELLOW  0xFFE0  
#define   WHITE   0xFFFF


struct vec2i
{
     int x, y;
     vec2i(){};
     vec2i(const vec2i& copy) : x(copy.x), y(copy.y) {}
     vec2i(int x, int y) { this->x = x; this->y = y;}
     bool operator!=(const vec2i& other) const {
          return this->x != other.x || this->y != other.y;
     }
     bool operator==(const vec2i& other) const {
          return this->x == other.x && this->y == other.y;
     }
};

class Pad
{
private:
     int checked; // -1 for unchecked, 0 for O, 1 for X
     vec2i position;
     bool selected;
     TFT_ILI9163C *tft;
public:
     bool filled;
     //constructor/destructor
     Pad(TFT_ILI9163C *tft): tft(tft), filled(false)
     {
          this->selected = false;
          this->position = vec2i(0, 0);
          this->checked = -1;
     }
     virtual ~Pad()
     {
          delete this->tft;
     }

     //Setters
     void setPosition(vec2i pos)
     {
          this->position = pos;
     }
     
     void setSelect(bool select)
     {
          this->selected = select;
          if(this->selected == true)
          {
               //render outline
               this->tft->drawRect(this->position.x + 3, this->position.y + 3, (128 / 3) - 4, (128 / 3) - 5, WHITE);
          }
          else
          {
               //remove the outline
               this->tft->drawRect(this->position.x + 3, this->position.y + 3, (128 / 3) - 4, (128 / 3) - 5, BLACK);
          }
     }

     void setChecked(int checked = -1)
     {
          this->checked = checked;
     }
     
     //Getters
     int getChecked()
     {
          return this->checked;
     }
     
     //functions
     void update()
     {
          
     }
     void render()
     {
          if(this->filled)
               return;
          //render the pad
          switch(checked)
          {
               //0 for O
               case 0:
                    this->tft->fillCircle(this->position.x + ((128/3)/2), this->position.y + ((128/3)/2), ((128/3)/2) - 3, RED);
                    this->tft->fillCircle(this->position.x + ((128/3)/2), this->position.y + 2 + ((128/3)/2) - 2, ((128/3)/2) - 8, BLACK);
                    this->filled = true;
                    break;

               //1 for X
               case 1:
                    this->tft->drawLine(this->position.x + 5, this->position.y + 5, this->position.x + (128/3) - 5, this->position.y + (128/3) - 5, BLUE);
                    this->tft->drawLine(this->position.x + (128/3) - 5, this->position.y + 5, this->position.x + 5, this->position.y + (128/3) - 5, BLUE);
                    
                    this->filled = true;
                    break;
          }
     }
};

class Board
{
private:
     TFT_ILI9163C* tft;

     bool xWin; // player X win
     bool oWin; // player O win
     bool draw;
     
     Pad *pads[3][3];

     int VRx; //joystick input X
     int VRy; //joystick input Y
     int SW; //joystick input button

     bool turn; //0 for turn O and 1 for turn X
     
     vec2i selectedPadPos;
     vec2i previousSelectedPadPos;
     
public:
     //Constructor/Destructor
     Board(TFT_ILI9163C* tft, int VRx, int VRy, int SW)
          : tft(tft), VRx(VRx), VRy(VRy), SW(SW),
          xWin(false), oWin(false), turn(0)
     {
          this->selectedPadPos = vec2i(0, 0);
          this->previousSelectedPadPos = vec2i(0, 0);
          
          for(int i = 0; i < 3; i++)
               for(int j = 0; j < 3; j++)
                    this->pads[i][j] = new Pad(this->tft);
     }
     virtual ~Board()
     {
          delete this->tft;
          for(int i = 0; i < 3; i++)
               for(int j = 0; j < 3; j++)
                    delete this->pads[i][j];
     }

     //Getters
     bool getOWin(){
          return this->oWin;
     }
     
     bool getXWin(){
          return this->xWin;
     }
     
     bool getDraw(){
          return this->draw;
     }

     //Functions

     void init()
     {
          //run the tft
          for(int i = 0; i < 3; i++)
               for(int j = 0; j < 3; j++)
                    this->pads[i][j]->setPosition(vec2i((128 / 3) * i, (128 / 3) * j));
                    
          //render the outline
          this->tft->clearScreen(BLACK);
          
          for(int i = 1; i < 3; i++)
               this->tft->drawLine(i * (127 / 3), 0, i * (127 / 3), 127, WHITE);
          
          for(int j = 1; j < 3; j++)
               this->tft->drawLine(0, j * (127 / 3), 127, j * (127 / 3), WHITE);
     }

     void updateInput()
     {
          //get the joystick input
          vec2i Jpos(map(analogRead(this->VRx), 0, 1023, -50, 50),
                     map(analogRead(this->VRy), 0, 1023, -50, 50));

          this->previousSelectedPadPos = this->selectedPadPos;

          //increase or decrease the position depending on the joystick input
          if(Jpos.x > 30)
               this->selectedPadPos.x++;
          if(Jpos.x < -30)
               this->selectedPadPos.x--;
          if(Jpos.y > 30)
               this->selectedPadPos.y++;
          if(Jpos.y < -30)
               this->selectedPadPos.y--;

          //sets the position if its out of the position
          if(selectedPadPos.y < 0)
               selectedPadPos.y = 2;
               
          if(selectedPadPos.y > 2)
               selectedPadPos.y = 0;
          
          if(selectedPadPos.x < 0)
               selectedPadPos.x = 2;
          
          if(selectedPadPos.x > 2)
               selectedPadPos.x = 0;

          //get the button input
          if(digitalRead(SW) != HIGH)
          {
               if(this->pads[selectedPadPos.x][selectedPadPos.y]->getChecked() == -1)
               {
                    this->pads[selectedPadPos.x][selectedPadPos.y]->setChecked(turn);
                    turn = !turn; //turn for the other player
               }
          }
     }

     void gameOver()
     {
          //draw gameOver
          this->tft->clearScreen();

          if(xWin)
          {
               this->tft->drawLine((128/3) + 5, (128/3) + 5, (128/3) + (128/3) - 5, (128/3) + (128/3) - 5, BLUE);
               this->tft->drawLine((128/3) + (128/3) - 5, (128/3) + 5, (128/3) + 5, (128/3) + (128/3) - 5, BLUE);

               
               
               this->tft->setCursor(12, 90);
               this->tft->setTextSize(4);
               this->tft->setTextColor(BLUE);
               this->tft->print("WINS!");
          }

          else if(oWin)
          {
               this->tft->fillCircle((128/3) + ((128/3)/2), (128/3) + ((128/3)/2), ((128/3)/2) - 3, RED);
               this->tft->fillCircle((128/3) + ((128/3)/2), (128/3) + 2 + ((128/3)/2) - 2, ((128/3)/2) - 8, BLACK);
               
               this->tft->setCursor(12, 90);
               this->tft->setTextSize(4);
               this->tft->setTextColor(RED);
               this->tft->print("WINS!");
          }
          else if(draw)
          {
               
               this->tft->drawLine(5, (128/3) + 5, (128/3) - 5, (128/3) + (128/3) - 5, BLUE);
               this->tft->drawLine((128/3) - 5, (128/3) + 5, 5, (128/3) + (128/3) - 5, BLUE);

               this->tft->fillCircle((128/3)*2 + ((128/3)/2), (128/3) + ((128/3)/2), ((128/3)/2) - 3, RED);
               this->tft->fillCircle((128/3)*2 + ((128/3)/2), (128/3) + 2 + ((128/3)/2) - 2, ((128/3)/2) - 8, BLACK);
               
               this->tft->setCursor(12, 90);
               this->tft->setTextSize(4);
               this->tft->setTextColor(BLUE);
               this->tft->print("DR");
               this->tft->setTextColor(WHITE);
               this->tft->print("A");
               this->tft->setTextColor(RED);
               this->tft->print("W!");
          }
          bool pressed = true;
          while(pressed){//waiting for button press
               
               pressed = digitalRead(this->SW);
          }

          if(!pressed)
          {
               //reset the game
               this->tft->clearScreen(BLACK);
               
               for(int i = 1; i < 3; i++)
                    this->tft->drawLine(i * (127 / 3), 0, i * (127 / 3), 127, WHITE);
               
               for(int j = 1; j < 3; j++)
                    this->tft->drawLine(0, j * (127 / 3), 127, j * (127 / 3), WHITE);

               for(int i = 0; i < 3; i++){
                    for(int j = 0; j < 3; j++){
                         this->pads[i][j]->setChecked();
                         this->pads[i][j]->filled = false;
                    }
               }

               this->xWin = 0;
               this->oWin = 0;
               this->draw = 0;
          }
          
     }
     
     void update()
     {
          this->updateInput();

          if(this->previousSelectedPadPos != this->selectedPadPos)
          {
               //if the player changed the pad
               //unselect the previous one 
               this->pads[this->previousSelectedPadPos.x][this->previousSelectedPadPos.y]->setSelect(false);
               
               //select the current one
               this->pads[this->selectedPadPos.x][this->selectedPadPos.y]->setSelect(true);

          }

          //check if the X player has won
          bool rowFullX = false;
          for(int i = 0; i < 3; i++){
               //check the rows
               if(this->pads[0][i]->getChecked() == 1 &&
                  this->pads[1][i]->getChecked() == 1 &&
                  this->pads[2][i]->getChecked() == 1 ||
                  //check diagonally
                  this->pads[0][0]->getChecked() == 1 &&
                  this->pads[1][1]->getChecked() == 1 &&
                  this->pads[2][2]->getChecked() == 1 ||
                  
                  this->pads[2][0]->getChecked() == 1 &&
                  this->pads[1][1]->getChecked() == 1 &&
                  this->pads[0][2]->getChecked() == 1 ||
                  //check columns
                  this->pads[i][0]->getChecked() == 1 &&
                  this->pads[i][1]->getChecked() == 1 &&
                  this->pads[i][2]->getChecked() == 1)
               {
                    rowFullX = true;
                    break;
               }
          }
          this->xWin = rowFullX;
          
          //check if the O player has won
          bool rowFullO = false;
          for(int i = 0; i < 3; i++){
               //check the rows
               if(this->pads[0][i]->getChecked() == 0 &&
                  this->pads[1][i]->getChecked() == 0 &&
                  this->pads[2][i]->getChecked() == 0 ||
                  //check diagonally
                  this->pads[0][0]->getChecked() == 0 &&
                  this->pads[1][1]->getChecked() == 0 &&
                  this->pads[2][2]->getChecked() == 0 ||
                  
                  this->pads[2][0]->getChecked() == 0 &&
                  this->pads[1][1]->getChecked() == 0 &&
                  this->pads[0][2]->getChecked() == 0 ||
                  //check columns
                  this->pads[i][0]->getChecked() == 0 &&
                  this->pads[i][1]->getChecked() == 0 &&
                  this->pads[i][2]->getChecked() == 0)
               {
                    rowFullO = true;
                    break;
               }
          }
          this->oWin = rowFullO;

          bool boardFull = true;
          for(int i = 0; i < 3; i++)
               for(int j = 0; j < 3; j++)
                    if(this->pads[i][j]->getChecked() == -1)
                         boardFull = false;
          this->draw = boardFull;
     }
     
     void render()
     {
          for(int i = 0; i < 3; i++)
               for(int j = 0; j < 3; j++)
                    this->pads[i][j]->render();
     }
};

/*
Teensy3.x and Arduino's
You are using 4 wire SPI here, so:
 MOSI:  11//Teensy3.x/Arduino UNO (for MEGA/DUE refere to arduino site)
 MISO:  12//Teensy3.x/Arduino UNO (for MEGA/DUE refere to arduino site)
 SCK:   13//Teensy3.x/Arduino UNO (for MEGA/DUE refere to arduino site)
 the rest of pin below:
 */
#define __CS 53
#define __DC 9

TFT_ILI9163C tft = TFT_ILI9163C(__CS, __DC);


Board board(&tft, A1, A2, 7);

bool gameOver = false;

int interval = 200;
long currentMillis = 0;
long previousMillis = 0;

void setup() {
     Serial.begin(9600);
     tft.begin();
     tft.fillScreen();
     
     for(int i = 1; i < 4; i++)
     {
          tft.setRotation(i);
          tft.fillScreen();
     }

     pinMode(A1, INPUT);
     pinMode(A2, INPUT);
     pinMode(7, INPUT_PULLUP);
     
     pinMode(6, OUTPUT);

     board.init();
}

void loop(){
     while(!gameOver)
     {
          currentMillis = millis();
          if(currentMillis - previousMillis >= interval)
          {
               board.update();
               board.render();

               if((!board.getOWin() != !board.getXWin()) || board.getDraw() == true) //xor
                    gameOver = true;

               previousMillis = currentMillis;
          }
     }
     if(gameOver)
     {
          board.gameOver();
          if(!board.getOWin() && !board.getXWin() && !board.getDraw())
          {
               gameOver = false;
          }
     }
     
}
