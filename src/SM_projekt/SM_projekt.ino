#include <SPI.h>
#include <Adafruit_GFX.h>
#include <gfxfont.h>

#include <Adafruit_PCD8544.h>
#include "Macros.h"

Adafruit_PCD8544 display = Adafruit_PCD8544(SCLK, DN, DC, SCE, RST);

/*
 * 0 - Menu
 * 1 - Game
 */
int gameState = 0;
int menuHighLight = 0;
float gameSpeed = 250;
bool ledState = false;

bool debounceAButton(){
  static bool clicked = false;
  if(digitalRead(A_BUTTON)==1){
    if(clicked){
      return false;
    }else{
      clicked = true;
      return true;
    }
  }else{
    clicked = false;
    return false;
  }
}


bool debounceBButton(){
  static bool clicked = false;
  if(digitalRead(B_BUTTON)==1){
    if(clicked){
      return false;
    }else{
      clicked = true;
      return true;
    }
  }else{
    clicked = false;
    return false;
  }
}

class Food{
  public:
  int posx;
  int posy;
  Food(){
    posx  = random(20)*4+2;
    posy  = random(11)*4+2;
  }

  void Draw(){
    display.drawRect(posx,posy,4,4,BLACK);
  }
};

class Snake
{
  
  public:
      Snake(){
      first = new element();
      last = first;
      first->posx= 42;
      first->posy= 22;
      first->next = NULL;
      Direction = 1;
    }

    void setDirection(int dir){
      if(dir>3 || dir<0)
        return;
      Direction = dir;
    }

    int getDirection(){
      return Direction;
    }
    
    void Move(){
      last->next = first;
      first->posx = last->posx;
      first->posy = last->posy;
      first = first->next;
      last = last->next;
      last->next = NULL;
      switch(Direction){
        case 0:
          goUp();
          break;
        case 1:
          goDown();
          break;
        case 2:
          goLeft();
          break;
        case 3:
          goRight();
          break;
      }
    }

     void Add(){
      element* add = new element();
      add->next = first;
      first = add;
    }

    void Clear(){
      element* ptr = first;
      while(ptr != NULL){
        element* pom = ptr->next;
        delete ptr;
        ptr = pom;
      }
      first = NULL;
      last = NULL;
    }

    void Draw(){
      element* ptr = first;
      while(ptr!= last){
        // Narysuj kwadraty
        int x = ptr->posx;
        int y = ptr->posy;
        display.fillRect(x,y ,snakeSize ,snakeSize ,BLACK);
        ptr = ptr->next;
      }
      int x = ptr->posx;
      int y = ptr->posy;
      display.fillCircle(x+1,y+1,2,BLACK);
    }

    bool isCollidingWith(Food* p){
      if(p->posx == last->posx && p->posy == last->posy)
        return true;
      return false;
      
    }

    bool isCollidingWithItself(){
      if(first == last)
        return false;
      element* ptr = first->next;
      while(ptr != last){
        if(ptr->posx == last->posx && ptr->posy ==last->posy){
          return true;
        }
        ptr = ptr->next;
      }
      return false;
    }
    
  private:
  static const int snakeSize = 4;
  int Direction;
  
  struct element{
    int posx;
    int posy;
    element* next;
  };

  element* first;
  element* last;

     void goUp(){
      last->posy-=snakeSize;
      if(last->posy <0)
        last->posy = 42;
    }

    void goDown(){

      last->posy+=snakeSize;
      if(last->posy > 42)
        last->posy = 2;
      
    }

    void goLeft(){
      

        last->posx-=snakeSize;
      if(last->posx < 0 )
        last->posx = 78;
    }

    void goRight(){
      last->posx+=snakeSize;
      if(last->posx > 78)
        last->posx = 2;
    }    
};

Food* food;
Snake* snake;

void menuEvents(){
  if(debounceAButton()){
    gameState = 1;
    snake = new Snake();
    food = new Food();
    gameSpeed = 250;
  }
  if(debounceBButton()){
    ledState = !ledState;
    digitalWrite(LED_DISPLAY, ledState);
  }
}

void drawMenu(){
  
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(30,15);
  display.print("Graj");
  display.drawRect(25 , 10, 40, 20, BLACK);
}

void gameEvents(){
  if(digitalRead(UP_BUTTON) == 1){
    if(snake->getDirection()!=1)
      snake->setDirection(0);
  }

  if(digitalRead(DOWN_BUTTON) == 1){
    if(snake->getDirection()!=0)
      snake->setDirection(1);
  }

  if(digitalRead(LEFT_BUTTON) == 1){
    if(snake->getDirection()!=3)
      snake->setDirection(2);
  }

  if(digitalRead(RIGHT_BUTTON) == 1){
    if(snake->getDirection()!=2)
      snake->setDirection(3);
  }
  
  if(digitalRead(A_BUTTON) == 1){
    
  }

  if(debounceBButton()){
    ledState = !ledState;
    digitalWrite(LED_DISPLAY, ledState);
  }
}

void Collisions(){

  if(snake->isCollidingWithItself()){
    snake->Clear();
    delete snake;
    delete food;
    snake = NULL;
    food = NULL;
    gameState = 2; 
    gameSpeed = 250;
    return;
  }

  if(snake->isCollidingWith(food)){
    snake->Add();
    delete food;
    food = new Food();
    if(gameSpeed > 75)
      gameSpeed-=5;
  }
  
  
}

void Draw(){
  display.drawRect(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,BLACK);
  if(snake != NULL) snake->Draw();
  if(food != NULL) food->Draw();
}

void gameOverEvents(){
  if(debounceAButton()){
    gameState = 0;
  }
  if(debounceBButton()){
    ledState = !ledState;
    digitalWrite(LED_DISPLAY, ledState);
  }
}

void drawGameOver(){
  display.drawRect(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,BLACK);
  display.setCursor(0,1);
  display.setTextSize(2);
  display.setTextColor(BLACK);
  display.println("GAME");
  display.print("OVER");
  display.setCursor(1,30);
  display.setTextSize(1);
  display.println("wcisnij A aby kontynuowac");
 
}

void Move(){
  if(snake != NULL) snake->Move();
}

void setup() {
  pinMode(UP_BUTTON,INPUT);
  pinMode(DOWN_BUTTON,INPUT);
  pinMode(LEFT_BUTTON,INPUT);
  pinMode(RIGHT_BUTTON,INPUT);
  pinMode(A_BUTTON,INPUT);
  pinMode(B_BUTTON,INPUT);
  pinMode(A2,INPUT);
  Serial.begin(9600);
  display.begin();
  display.clearDisplay();
  display.setContrast(55);
  display.setRotation(2);
  snake = new Snake();
  food = new Food(); 
  randomSeed(analogRead(2));
  pinMode(LED_DISPLAY,OUTPUT);
}

void loop() {
  switch(gameState){
    case 0:{
      menuEvents();
      drawMenu();
      break;
    }
    case 1:{
      gameEvents();
      Collisions();
      Move();
      Draw();
      break;
    }
    case 2:{
      gameOverEvents();
      drawGameOver();
      break;
    }
  }
  display.display();
  //delay(((int)(40/gameSpeed)));
  delay(gameSpeed);
  display.clearDisplay();
}
