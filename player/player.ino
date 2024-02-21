#include <esp_now.h>
#include <WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ezButton.h>
#define OLED_RESET 16
Adafruit_SSD1306 display(OLED_RESET);

//id
int id;

//button
ezButton button1(4);
ezButton button2(5);

//variables
int currentState = 0;

uint8_t broadcastAddress[] = {0x3C, 0x61, 0x05, 0x03, 0x69, 0x64};
uint8_t CoinAddress[] = {0x3C, 0x61, 0x05, 0x03, 0x69, 0x64};

uint8_t player1MacAddress[] = {0x3C, 0x71, 0xBF, 0x10, 0x5C, 0x3C};

bool isReady = false;
bool betPlaced = false;
bool pickStand = false;
int cardCount = 2;
int MyCredit = 0;

typedef struct game_state_message {
  int state;
  int id;
  char message[32];
  bool is_ready;
  int bet_amount;
  bool hit;
  int WithdrawCredit;
} game_state_message;

typedef struct dealer_message {
  char message[32]; //message
  int player_state;  //state number 0 = init , 1=bet , 2 = playing, 3 = waiting
  int player1_result; //result number 0 = lose, 1 = win, 2 = draw
  int player2_result; //result number 0 = lose, 1 = win, 2 = draw
  int player1_card[5];
  int player2_card[5];
  int dealer_card[5];
  int FromWho; //if 0 = Dealer, 1 = Coin master 
  int DepositCredit;
} dealer_message;

game_state_message gameStateMessage;
dealer_message dealerMessage;

esp_now_peer_info_t peerInfo;

//Start display
void InitDisplay(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(15,8);
  display.println("Press any button"); 
  display.setCursor(35,16);
  display.println("to start");
  display.display();
}

void MyCardDisplay() {
  for (int i; i < 5; i++) {
    int myCard;
    if (id == 1) {
      myCard = dealerMessage.player1_card[i];
    } else {
      myCard = dealerMessage.player2_card[i];
    }

    if (myCard == 1) {
      display.print("A"); 
    } else if (myCard <= 10 && myCard >= 2) {
      display.print(myCard); 
    } else if (myCard == 11) {
      display.print("J"); 
    } else if (myCard == 12) {
      display.print("Q"); 
    } else if (myCard == 13) {
      display.print("K"); 
    }
    display.print(" ");
  }
  display.println("");
}

void WaitingForJoinDisplay(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Waiting for others to join");
  display.display();
}

void PlaceYourBetDisplay(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Place your bet");
  display.display();
}

void WaitingForOthersBetDisplay(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Waiting for others betting");
  display.display();
}

void DecisionDisplay(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  display.setCursor(0,0);
  MyCardDisplay();

  display.setCursor(0,8);
  display.println("Hit or Stand ?");
  display.display();
}

void StandPickedDisplay(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0,0);
  MyCardDisplay();

  display.setCursor(0,8);
  display.println("You picked Stand");
  display.display();
}

void GameResultDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  MyCardDisplay();

  display.setTextSize(2);
  display.setCursor(0,8);
  if (id == 1) {
    switch(dealerMessage.player1_result) {
      case 0:
        display.println("You Lose");
        break;
      case 1:
        display.println("You Win");
        break;
      case 2:
        display.println("You Draw");
        break;
    }
  } else {
    switch(dealerMessage.player2_result) {
      case 0:
        display.println("You Lose");
        break;
      case 1:
        display.println("You Win");
        break;
      case 2:
        display.println("You Draw");
        break;
    }

  }
  display.display();
}

//State recieve from dealer
void OnStateRecieve(const uint8_t * mac, const uint8_t *incomingData, int len){
  memcpy(&dealerMessage, incomingData, sizeof(dealerMessage));
  currentState = dealerMessage.player_state;
  if (dealerMessage.player_state == 0) {
    ResetGame();
  }
  if (dealerMessage.FromWho == 1) {
    MyCredit += dealerMessage.DepositCredit;
  }
}

void handlePlayerIdleState();
void handlePlayerPlaceBetState();
void handlePlayerPlayingState();
void handleDealerPlayState();

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void SendStateToDealer() {
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &gameStateMessage, sizeof(gameStateMessage));
  while(result != ESP_OK)
  {
    result = esp_now_send(broadcastAddress, (uint8_t *) &gameStateMessage, sizeof(gameStateMessage));
  }
  return;
}

void ResetGame() {
  isReady = false;
  betPlaced = false;
  pickStand = false;
  cardCount = 2;
  gameStateMessage.state = 0;
  gameStateMessage.bet_amount = 100;
  gameStateMessage.hit = true;
}

void setup() {
  button1.setDebounceTime(400); 
  button2.setDebounceTime(400);

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializeing ESP-NOW");
    return;
  }
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c); //สั่งให้จอ OLED เริ่มทำงานที่ Address 0x3C
  display.clearDisplay();
  InitDisplay();

  uint8_t myMacAddress[6];
  WiFi.macAddress(myMacAddress);

  if (memcmp(myMacAddress, player1MacAddress, 6) == 0) {
    Serial.println("This is player 1");
    id = 1;
  } else {
    Serial.println("This is player 2");
    id = 2;
  }

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  ////Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Fail to add peer");
    return;
  }

  //Register send callback
  esp_now_register_send_cb(OnDataSent);

  //Register receive callback
  esp_now_register_recv_cb(OnStateRecieve);
}

void loop () {
  button1.loop();
  button2.loop();
  switch(currentState) {
    case 0:
      handlePlayerIdleState();
      break;
    case 1:
      handlePlayerPlaceBetState();
      break;
    case 2:
      handlePlayerPlayingState();
      break;
    case 3:
      handleDealerPlayState();
      break;
    }
}

void handlePlayerIdleState() {
  if(!isReady) {
    InitDisplay();
    if(button1.isPressed()){ //press to ready
      Serial.println("state 0 button pressed");
      gameStateMessage.state = currentState;
      gameStateMessage.id=id;
      gameStateMessage.is_ready=1;
      isReady = true;
      SendStateToDealer();
    }
  } else if (button2.isPressed()){
      Serial.println("Withdraw all credit");
      gameStateMessage.WithdrawCredit = MyCredit;
      MyCredit = 0;
      gameStateMessage.id=id;
      SendStateToDealer();
  } else {
    WaitingForJoinDisplay();
  }
}

void handlePlayerPlaceBetState() {
  if (!betPlaced) {
    PlaceYourBetDisplay();
    if(button1.isPressed()){ // press to place bet
      Serial.println("state 1 button pressed");
      gameStateMessage.state = currentState;
      gameStateMessage.id=id;
      gameStateMessage.is_ready=1;
      gameStateMessage.bet_amount=100;
      betPlaced = true;
      SendStateToDealer();
    }
  } else {
    WaitingForOthersBetDisplay();
  }
}

void handlePlayerPlayingState() {
  if(button1.isPressed()){ //if hit
    Serial.println("You Pick Hit!");
    gameStateMessage.state=currentState;
    gameStateMessage.id=id;
    gameStateMessage.hit=true;
    SendStateToDealer();
    cardCount++;
    if (cardCount == 5) {
      pickStand=true;
    }
  }
  else if(button2.isPressed()){ // if stand
    Serial.println("You Pick Stand!");
    gameStateMessage.state=currentState;
    gameStateMessage.id=id;
    gameStateMessage.hit=false;
    pickStand=true;
    SendStateToDealer();
  }
  if (!pickStand) {
    DecisionDisplay();
  } else {
    StandPickedDisplay();
  }
}

void handleDealerPlayState() {
  GameResultDisplay();
}