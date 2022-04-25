
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>


// COIN ACCEPTOR
// Digital pin 2 = 10k Ohms Resistor + COIN PIN of COIN ACCEPTOR (10k OHMS RESISTOR connected to 5V arduino) PULLUP RESISTOR
// 12DC = 12V DC SUPPLY
// GND = GND ARDUINO

// GSM MODULE
// 3.7V-4V LM2562 = GSM MODULE VCC
// GND ARDUINO AND LM2562 = GSM MODULE GND
// DIGITAL PIN 14 (TX) = GSM MODULE PIN (RX)
// DIGITAL PIN 15 (RX) = GSM MODULE PIN (TX)

// LCD 16x2
// SCL = DIGITAL PIN 21 SCL
// SDA = DIGITAL PIN 20 SDA
// VCC = 5V arduino
// GND = GND

//BUZZER 
// DIGITAL PIN 3 =
const int buzzer = 3; 
// SERVO MOTOR
// DIGITAL PIN 4 = 
// DIGITAL PIN 5 =
// DIGITAL PIN 6 =
// DIGITAL PIN 7 =
// DIGITAL PIN 8 =
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;
LiquidCrystal_I2C lcd(0x27, 16, 2);
int defaultAngle = 0;

const int ServoItem1 = 4;
const int ServoItem2 = 5;
const int ServoItem3 = 6;
const int ServoItem4 = 7;
const int ServoItem5 = 8;

// PUSH BUTTON
// DIGITAL PIN 27 =
// DIGITAL PIN 28 =
// DIGITAL PIN 29 =
// DIGITAL PIN 30 =
// DIGITAL PIN 31 =
const int PushBtnItem1 = 27;
const int PushBtnItem2 = 28;
const int PushBtnItem3 = 29;
const int PushBtnItem4 = 30;
const int PushBtnItem5 = 31;

// IR SENSOR 
const int IRsensor = 32;

int PushbtnItem1Value;
int PushbtnItem2Value;
int PushbtnItem3Value;
int PushbtnItem4Value;
int PushbtnItem5Value;
int IRsensorVal;

int PotentioValue;
int ItemCount;

int NumberOfItemChoosen;
int ItemCost1=10;
int ItemCost2=20;
int ItemCost3=15;
int ItemCost4=5;
int ItemCost5=5;
int TotalItemCost=0;
int InsertedAmount=0;
int RemainingCost=0;

int inventoryItem1=5;
int inventoryItem2=5;
int inventoryItem3=5;
int inventoryItem4=5;
int inventoryItem5=5;
bool invItemHasbeenChecked1=false;
bool invItemHasbeenChecked2=false;
bool invItemHasbeenChecked3=false;
bool invItemHasbeenChecked4=false;
bool invItemHasbeenChecked5=false;
bool isZeroItem1 =false;
bool isZeroItem2 =false;
bool isZeroItem3 =false;
bool isZeroItem4 =false;
bool isZeroItem5 =false;

bool transactionFinished=false;

String SmsString ="";
String number = "+639265556648"; //-> change with your number

// variable use to measuer the intervals inbetween impulses
int i=0;
// Number of impulses detected
int impulsCount=0;
// Sum of all the coins inseted
float total_amount=0;
// coin inserted
float coin_inserted=0;
//buffer for sms
String _buffer;
void setup() {
 // pinMode(2, INPUT_PULLUP);
  Serial.begin(4800);
  Serial3.begin(4800);
  delay(7000); //delay for 7 seconds to make sure the modules get the signal
  _buffer.reserve(200);
 // Interrupt connected to PIN D2 executing IncomingImpuls function when signal goes from HIGH to LOW
 attachInterrupt(0,incomingImpuls, FALLING);
 //EEPROM.get(0, total_amount);


 pinMode(buzzer, OUTPUT); // Set buzzer - pin 3 as an output
 pinMode(PushBtnItem1, INPUT_PULLUP); // set btn as input_pullup - pin 27
 pinMode(PushBtnItem2, INPUT_PULLUP); // set btn as input_pullup - pin 28
 pinMode(PushBtnItem3, INPUT_PULLUP); // set btn as input_pullup - pin 29
 pinMode(PushBtnItem4, INPUT_PULLUP); // set btn as input_pullup - pin 30
 pinMode(PushBtnItem5, INPUT_PULLUP); // set btn as input_pullup - pin 31
 pinMode(IRsensor, INPUT); // set ir sensor as input - pin 32

 lcd.init(); // Initialize the LCD
 lcd.backlight(); // Turn on the LCD backlight
 delay(3000); // wait for console opening
 
 RecieveMessage();

 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("Vending Machine");

 delay(3000);
 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("Please select ");
 lcd.setCursor(0,1);
 lcd.print("item");


 noTone(buzzer);
}

void incomingImpuls()
{
  impulsCount=impulsCount+1;
  i=0;
}

void loop() {
  
  GetSMSText();
  CheckInventoryQuantity();
  
  //Check if Supply 1 (main supply) is up //if main supply is dead then send notification and do alarm
  int isSupply1 = analogRead(A1);
  if(isSupply1 == 0){
    SendMessage("The main power ac adaptor was cut off! Now using the Secondary Supply!");
    tone(buzzer,1000);
    delay(5000);
    noTone(buzzer);
  }
  

  PushbtnItem1Value=digitalRead(PushBtnItem1);
  PushbtnItem2Value= digitalRead(PushBtnItem2);
  PushbtnItem3Value= digitalRead(PushBtnItem3);
  PushbtnItem4Value = digitalRead(PushBtnItem4);
  PushbtnItem5Value =digitalRead(PushBtnItem5); 
  IRsensorVal =digitalRead(IRsensor); 
  Serial.println(IRsensorVal);
  Serial.println(PushBtnItem1);
  Serial.println(PushBtnItem2);
  Serial.println(PushBtnItem3);
  Serial.println(PushBtnItem4);
  Serial.println(PushBtnItem5);
  if(IRsensorVal == 0){
    tone(buzzer,1000);
    String Message="Someone has forced open the vending machine";
    SendMessage(Message);
    delay(60000);
    noTone(buzzer);
  }

  if(PushbtnItem1Value == 0){
    PushbtnItem1Value=digitalRead(PushBtnItem1);
    while(PushbtnItem1Value == 1){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("how many on that");
      PushbtnItem1Value=digitalRead(PushBtnItem1);
      PotentioValue = analogRead(A0);
      ItemCount = floatMap(PotentioValue, 0, 1023, 0, inventoryItem1);
      lcd.setCursor(0,1);
      lcd.print("item?" + String(ItemCount+1));
      delay(500);
      if (PushbtnItem1Value==0){
        NumberOfItemChoosen = ItemCount+1;
        PushbtnItem1Value=digitalRead(PushBtnItem1);
        lcd.clear();
        while(PushbtnItem1Value == 1){
          TotalItemCost = NumberOfItemChoosen * ItemCost1;
          PushbtnItem1Value=digitalRead(PushBtnItem1);
          lcd.setCursor(0,0);
          String totalcost = "Total cost: "+ String(TotalItemCost);
          lcd.print(totalcost);
          lcd.setCursor(0,1);
          lcd.print("Inst Coin: "+String(InsertedAmount));
          ReadInsertedCoin();
          if(InsertedAmount >= TotalItemCost){
            lcd.clear();
            CheckIfSufficientBalance("Item 1",1,NumberOfItemChoosen);
            break;
          }
        }
      }
      if(transactionFinished){
        transactionFinished=false;
        break;
      }
    }
  }
  
  if(PushbtnItem2Value == 0){
    PushbtnItem2Value=digitalRead(PushBtnItem2);
    while(PushbtnItem2Value == 1){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("how many on that");
      PushbtnItem2Value=digitalRead(PushBtnItem2);
      PotentioValue = analogRead(A0);
      ItemCount = floatMap(PotentioValue, 0, 1023, 0, inventoryItem2);
      lcd.setCursor(0,1);
      lcd.print("item?" + String(ItemCount+1));
      delay(500);
      if (PushbtnItem2Value==0){
        NumberOfItemChoosen = ItemCount+1;
        PushbtnItem2Value=digitalRead(PushBtnItem2);
        lcd.clear();
        while(PushbtnItem2Value == 1){
          TotalItemCost = NumberOfItemChoosen * ItemCost2;
          PushbtnItem2Value=digitalRead(PushBtnItem2);
          lcd.setCursor(0,0);
          String totalcost = "Total cost: "+ String(TotalItemCost);
          lcd.print(totalcost);
          lcd.setCursor(0,1);
          lcd.print("Inst Coin: "+String(InsertedAmount));
          ReadInsertedCoin();
          if(InsertedAmount >= TotalItemCost){
            lcd.clear();
            CheckIfSufficientBalance("Item 2",2,NumberOfItemChoosen);
            break;
          }
        }
      }
      if(transactionFinished){
        transactionFinished=false;
        break;
      }
    }
  }
  
  if(PushbtnItem3Value == 0){
    PushbtnItem3Value=digitalRead(PushBtnItem3);
    while(PushbtnItem3Value == 1){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("how many on that");
      PushbtnItem3Value=digitalRead(PushBtnItem3);
      PotentioValue = analogRead(A0);
      ItemCount = floatMap(PotentioValue, 0, 1023, 0, inventoryItem3);
      lcd.setCursor(0,1);
      lcd.print("item?" + String(ItemCount+1));
      delay(500);
      if (PushbtnItem3Value==0){
        NumberOfItemChoosen = ItemCount+1;
        PushbtnItem3Value=digitalRead(PushBtnItem3);
        lcd.clear();
        while(PushbtnItem3Value == 1){
          TotalItemCost = NumberOfItemChoosen * ItemCost3;
          PushbtnItem3Value=digitalRead(PushBtnItem3);
          lcd.setCursor(0,0);
          String totalcost = "Total cost: "+ String(TotalItemCost);
          lcd.print(totalcost);
          lcd.setCursor(0,1);
          lcd.print("Inst Coin: "+String(InsertedAmount));
          ReadInsertedCoin();
          if(InsertedAmount >= TotalItemCost){
            lcd.clear();
            CheckIfSufficientBalance("Item 3",3,NumberOfItemChoosen);
            break;
          }
          if(transactionFinished){
            break;
          }
        }
      }
      if(transactionFinished){
        transactionFinished=false;
        break;
      }
    }    
  }
  if(PushbtnItem4Value == 0){
    PushbtnItem4Value=digitalRead(PushBtnItem4);
    while(PushbtnItem4Value == 1){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("how many on that");
      PushbtnItem4Value=digitalRead(PushBtnItem4);
      PotentioValue = analogRead(A0);
      ItemCount = floatMap(PotentioValue, 0, 1023, 0, inventoryItem4);
      lcd.setCursor(0,1);
      lcd.print("item?" + String(ItemCount+1));
      delay(500);
      if (PushbtnItem4Value==0){
        NumberOfItemChoosen = ItemCount+1;
        PushbtnItem4Value=digitalRead(PushBtnItem4);
        lcd.clear();
        while(PushbtnItem4Value == 1){
          TotalItemCost = NumberOfItemChoosen * ItemCost4;
          PushbtnItem4Value=digitalRead(PushBtnItem4);
          lcd.setCursor(0,0);
          String totalcost = "Total cost: "+ String(TotalItemCost);
          lcd.print(totalcost);
          lcd.setCursor(0,1);
          lcd.print("Inst Coin: "+String(InsertedAmount));
          ReadInsertedCoin();
          if(InsertedAmount >= TotalItemCost){
            lcd.clear();
            CheckIfSufficientBalance("Item 4",4,NumberOfItemChoosen);
            break;
          }
        }
      }
      if(transactionFinished){
        transactionFinished=false;
        break;
      }
    }     
  }
  if(PushbtnItem5Value == 0){
    PushbtnItem5Value=digitalRead(PushBtnItem5);
    while(PushbtnItem5Value == 1){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("how many on that");
      PushbtnItem5Value=digitalRead(PushBtnItem5);
      PotentioValue = analogRead(A0);
      ItemCount = floatMap(PotentioValue, 0, 1023, 0, inventoryItem5);
      lcd.setCursor(0,1);
      lcd.print("item?" + String(ItemCount+1));
      delay(500);
      if (PushbtnItem5Value==0){
        NumberOfItemChoosen = ItemCount+1;
        PushbtnItem5Value=digitalRead(PushBtnItem5);
        lcd.clear();
        while(PushbtnItem5Value == 1){
          TotalItemCost = NumberOfItemChoosen * ItemCost5;
          PushbtnItem5Value=digitalRead(PushBtnItem5);
          lcd.setCursor(0,0);
          String totalcost = "Total cost: "+ String(TotalItemCost);
          lcd.print(totalcost);
          lcd.setCursor(0,1);
          lcd.print("Inst Coin: "+String(InsertedAmount));
          ReadInsertedCoin();
          if(InsertedAmount >= TotalItemCost){
            lcd.clear();
            CheckIfSufficientBalance("Item 5",5,NumberOfItemChoosen);
            break;
          }
        }
      }
      if(transactionFinished){
        transactionFinished=false;
        break;
      }
    }         
  }
  
}

void RecieveMessage()
{
  Serial.println ("Serial3800L Read an SMS");
  delay (1000);
  Serial3.println("AT+CNMI=2,2,0,0,0"); // AT Command to receive a live SMS
  delay(1000);
  Serial.write ("Unread Message done");
}
String _readSerial() {
  if (Serial3.available()>0) {
    return Serial3.readString();
  }
}

void GetSMSText(){
    if (Serial3.available() > 0){
      SmsString =Serial3.readString(); 
      Serial.println(SmsString);
      if(SmsString.indexOf("+CMT:") > 0){
        if(SmsString.indexOf("How many")>=0){
          String msgItem1 = "Quantity of Item 1="+String(inventoryItem1)+"\n";
          String msgItem2 = "Quantity of Item 2="+String(inventoryItem2)+"\n";
          String msgItem3 = "Quantity of Item 3="+String(inventoryItem3)+"\n";
          String msgItem4 = "Quantity of Item 4="+String(inventoryItem4)+"\n";
          String msgItem5 = "Quantity of Item 5="+String(inventoryItem5);
          String quantityMsg = msgItem1+msgItem2+msgItem3+msgItem4+msgItem5;
          SendMessage(quantityMsg);
        }
        if(SmsString.indexOf("Total amount")>=0){
          String MessageTotalAmount = "Total Amount Inserted to Vending Machine is P"+String(total_amount);
          SendMessage(MessageTotalAmount);
        }
        if(SmsString.indexOf("Fully")>=0){
          inventoryItem1=5;
          inventoryItem2=5;
          inventoryItem3=5;
          inventoryItem4=5;
          inventoryItem5=5;
          isZeroItem1=false;
          isZeroItem2=false;
          isZeroItem3=false;
          isZeroItem4=false;
          isZeroItem5=false;
          invItemHasbeenChecked1= false;  
          invItemHasbeenChecked2= false;  
          invItemHasbeenChecked3= false;  
          invItemHasbeenChecked4= false;  
          invItemHasbeenChecked5= false;  
          String MessageFullyRestock = "Vending Machine Inventory has been fully restocked";
          SendMessage(MessageFullyRestock);
        }
      }
    }
}

//Send Message funtion that will send SMS message when called
void SendMessage(String in)
{
  //Serial.println ("Sending Message");
  Serial3.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);
  //Serial.println ("Set SMS Number");
  Serial3.println("AT+CMGS=\"" + number + "\"\r"); //Mobile phone number to send message
  delay(1000);
  String SMS = in;
  Serial3.println(SMS);
  delay(100);
  Serial3.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
  _buffer = _readSerial();
}

// function to convert potentiometer value
int floatMap(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void CheckIfSufficientBalance(String item, int servoNumber, int quantity){
    String Message = item+" has been purchased (no of items " + String(NumberOfItemChoosen) + ")";
    SendMessage(Message);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Dispensing");
    lcd.setCursor(0,1);
    lcd.print("Pls wait");
    delay(2000);
    if (servoNumber ==1){
      inventoryItem1=inventoryItem1-quantity;
      servo1.attach(ServoItem1);
      servo1.write(180);
      DelayFunc(quantity);
      servo1.detach();
    } else if (servoNumber ==2){
      inventoryItem2=inventoryItem2-quantity;
      servo2.attach(ServoItem2);
      servo2.write(180);
      DelayFunc(quantity);
      servo2.detach();
    }else if (servoNumber ==3){
      inventoryItem3=inventoryItem3-quantity;
      servo3.attach(ServoItem3);
      servo3.write(180);
      DelayFunc(quantity);
      servo3.detach();
    }else if (servoNumber ==4){
      inventoryItem4=inventoryItem4-quantity;
      servo4.attach(ServoItem4);
      servo4.write(180);
      DelayFunc(quantity);
      servo4.detach();
    }else if (servoNumber ==5){
      inventoryItem5=inventoryItem5-quantity;
      servo5.attach(ServoItem5);
      servo5.write(180);
      DelayFunc(quantity);
      servo5.detach();
    }
    delay(5000);
    String InventoryUpdate = item+" has been bought with a quantity of " + String(NumberOfItemChoosen);
    SendMessage(InventoryUpdate);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Thank you ");
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("for buying");
    delay(1000);
    lcd.clear(); 
    lcd.setCursor(0,0);
    lcd.print("Vending Machine");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Please select ");
    lcd.setCursor(0,1);
    lcd.print("item");
    InsertedAmount=0;
    RemainingCost=0;
    lcd.clear();
    transactionFinished=true;
    lcd.setCursor(0,0);
    lcd.print("Vending Machine");
    delay(3000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Please select ");
    lcd.setCursor(0,1);
    lcd.print("item");

}

void ReadInsertedCoin(){

  i=i+1;
  
  
  Serial.print("i=");
  Serial.print(i);
  Serial.print(" Impulses:");
  Serial.print(impulsCount);
  Serial.print(" Total:");
  Serial.println(total_amount);
  
 // bagong 5pesos
  if (i>=10 and impulsCount==1){
    total_amount=total_amount+5;
    InsertedAmount = InsertedAmount +5;
    impulsCount=0;
    //EEPROM.put(0, total_amount);
  }
  // lumang 5 pesos
   if (i>=10 and impulsCount==2){
    total_amount=total_amount+5;
    InsertedAmount = InsertedAmount +5;
    impulsCount=0;
    //EEPROM.put(0, total_amount);
  }
  // bagong 10 pesos
   if (i>=10 and impulsCount==3){
    total_amount=total_amount+10;
    InsertedAmount = InsertedAmount +10;
    impulsCount=0;
    //EEPROM.put(0, total_amount);
  }
  // lumang 10 pesos
   if (i>=10 and impulsCount==4){
    total_amount=total_amount+10;
    InsertedAmount = InsertedAmount +10;
    impulsCount=0;
    //EEPROM.put(0, total_amount);
  }
}

void CheckInventoryQuantity(){

  if(inventoryItem1==2 && !invItemHasbeenChecked1){
    String Message = "Item 1 iventory to low. Please restock. Current remaining items in Item 1: " + String(inventoryItem1);
    SendMessage(Message);   
    invItemHasbeenChecked1= true; 
  }
  if(inventoryItem2==2 && !invItemHasbeenChecked2){
    String Message = "Item 2 iventory to low. Please restock. Current remaining items in Item 2: " + String(inventoryItem2);
    SendMessage(Message);   
    invItemHasbeenChecked2= true;  
  }

  if(inventoryItem3==2 && !invItemHasbeenChecked3){
    String Message = "Item 3 iventory to low. Please restock. Current remaining items in Item 3: " + String(inventoryItem3);
    SendMessage(Message);   
    invItemHasbeenChecked3= true;  
  }

  if(inventoryItem4==2 && !invItemHasbeenChecked4){
    String Message = "Item 4 iventory to low. Please restock. Current remaining items in Item 4: " + String(inventoryItem4);
    SendMessage(Message);  
    invItemHasbeenChecked4= true;   
  }

  if(inventoryItem5==2 && !invItemHasbeenChecked5){
    String Message = "Item 5 iventory to low. Please restock. Current remaining items in Item 5: " + String(inventoryItem5);
    SendMessage(Message);   
    invItemHasbeenChecked5= true;  
  }

  if(inventoryItem1<=0 && !isZeroItem1){
    String Message = "Item 1 iventory to low. Please restock. Current remaining items in Item 1: " + String(inventoryItem1);
    SendMessage(Message);   
    invItemHasbeenChecked1= true; 
    isZeroItem1= true;
  }
  if(inventoryItem2<=0 && !isZeroItem2){
    String Message = "Item 2 iventory to low. Please restock. Current remaining items in Item 2: " + String(inventoryItem2);
    SendMessage(Message);   
    invItemHasbeenChecked2= true;  
    isZeroItem2= true;
  }

  if(inventoryItem3<=0 && !isZeroItem3){
    String Message = "Item 3 iventory to low. Please restock. Current remaining items in Item 3: " + String(inventoryItem3);
    SendMessage(Message);   
    invItemHasbeenChecked3= true;  
    isZeroItem3= true;
  }

  if(inventoryItem4<=0 && !isZeroItem4){
    String Message = "Item 4 iventory to low. Please restock. Current remaining items in Item 4: " + String(inventoryItem4);
    SendMessage(Message);  
    invItemHasbeenChecked4= true;   
    isZeroItem4= true;
  }

  if(inventoryItem5<=0 && !isZeroItem5){
    String Message = "Item 5 iventory to low. Please restock. Current remaining items in Item 5: " + String(inventoryItem5);
    SendMessage(Message);   
    invItemHasbeenChecked5= true;  
    isZeroItem5= true;
  }

  
}

void DelayFunc(int numberofItems){
  if(numberofItems==1){
    delay(1500);
  }else if (numberofItems==2){
    delay(3000);
  }else if (numberofItems==3){
    delay(4500);
  }else if (numberofItems==4){
    delay(6000);
  }else if (numberofItems==5){
    delay(7500);
  }
}