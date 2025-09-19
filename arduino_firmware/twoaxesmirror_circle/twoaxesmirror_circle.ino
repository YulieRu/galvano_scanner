#include "Wire.h"
#include "MCP4725.h"

#define PARSE_AMOUNT 5         // число значений в массиве, который хотим получить
#define start_com '[' 
#define end_com ']' 
#define delCom '=' 

unsigned long timing=0;
unsigned long start_time=0;
unsigned long next=0;

MCP4725 MCP_X(0x60);
MCP4725 MCP_Y(0x61);
MCP4725 MCP_user(0x0);

double commandsData[PARSE_AMOUNT];
boolean getStarted;
boolean recievedFlag;
byte index;
int values[1];
int ifGreeting=0;
String string_convert="";
String commandName = "";

void setup(){
  Serial.begin(115200);
  Serial.setTimeout(100);
  
  Wire.begin();
  MCP_X.begin();
  MCP_X.setValue(0);
  MCP_Y.begin();
  MCP_Y.setValue(0);
}

void parseCommand(String com) 
{
  String commandName=com.substring(com.indexOf("[")+1, com.indexOf("="));
  String commandValue=com.substring(com.indexOf("=")+1,com.indexOf("]"));
  int commandValLen = commandValue.length();
  if(com.indexOf("=")<0)
    {
      commandName=com.substring(com.indexOf("[")+1, com.indexOf("]"));
      commandValLen = 0;
    }

  if (commandName.equalsIgnoreCase("mcp_x")) 
    {
      if (commandValLen <= 0)
        {
          Serial.print("Current position: ");
          Serial.println(MCP_X.getValue());
        }
      else
        {
          ParseArray(values, commandValue);
          MCP_X.setValue(values[0]);
          //while (!MCP_X.ready()) {};
          delayMicroseconds(10);
          Serial.println("Ready");
        }
    }
  if (commandName.equalsIgnoreCase("mcp_y")) 
    {
      if (commandValLen <= 0)
        {
          Serial.print("Current position: ");
          Serial.println(MCP_Y.getValue());
        }
      else
        {
          ParseArray(values, commandValue);
          MCP_Y.setValue(values[0]);
          //while (!MCP_Y.ready()) {};
          delayMicroseconds(10);
          Serial.println("Ready");
        } 
    }    
}

void ParseArray(int *Array, String str)
  {
    int i=0;
    while(str.indexOf(",")>0)
      {
        Array[i]=str.substring(0, str.indexOf(",")).toInt();
        str=str.substring(str.indexOf(",")+1);
        i++;
      }
    Array[i]=str.toInt();
  }

void greeting() {
  Serial.println("Enter the command: [RECORD=numSteps pause(ms) angle pauseScan(ms) axis(0 (x) or 1 (y)]");  
  ifGreeting = 1;
}

void parsing() {
  if (Serial.available() > 0) {
    char incomingByte = Serial.read();        // обязательно ЧИТАЕМ входящий символ
    if (getStarted) {                         // если приняли начальный символ (парсинг разрешён)
      if (incomingByte != ' ' && incomingByte != end_com) {   // если это не пробел И не конец
        string_convert += incomingByte;       // складываем в строку
      } else {                                // если это пробел или ; конец пакета
        commandsData[index] = string_convert.toDouble();  // преобразуем строку в int и кладём в массив
        string_convert = "";                  // очищаем строку
        index++;                              // переходим к парсингу следующего элемента массива
      }
    } else {
      commandName += incomingByte;
    }
    if (incomingByte == delCom) {             // если это '='
      getStarted = true;                      // поднимаем флаг, что можно парсить
      index = 0;                              // сбрасываем индекс
      string_convert = "";                    // очищаем строку 
    }

    if (incomingByte == end_com) {                // если таки приняли ] - конец парсинга
      getStarted = false;                     // сброс
      recievedFlag = true;                    // флаг на принятие
      commandName=commandName.substring(commandName.indexOf("[")+1, commandName.indexOf("="));
    }
  }
}

void loop() {
  if (ifGreeting == 0) {greeting();};
  parsing();       // функция парсинга
  if (recievedFlag) {                           // если получены данные
    recievedFlag = false;
  
  if (commandName.equalsIgnoreCase("RECORD")){
    Serial.println("RECORD");
    
    if (commandsData[4] == 0){
      MCP_user = MCP_X;
    } else {
      MCP_user = MCP_Y;
    }
    commandsData[1] *= 1000;
    commandsData[3] *= 1000;

    int i=0;
    start_time=micros();
    while (i<commandsData[0]){
      MCP_user.setValue(commandsData[2]*i);
      delayMicroseconds(10);
      next = start_time+i*commandsData[1];
      //if (millis() - timing -next > commandsData[1]){
      //  timing = millis();
      //  i++;
      //}
      i++;
      delayMicroseconds(next-micros());
    }
  
    /*while (true) {
      if (millis() - timing > commandsData[3]){ // Вместо 10000 подставьте нужное вам значение паузы 
        timing = millis(); 
        break;
      }
    }*/
    //delay(commandsData[3]);
  
    i=commandsData[0]-2;
    start_time=micros();
    while (i>=0){
      MCP_user.setValue(commandsData[2]*i);
      delayMicroseconds(10);
      next = start_time+(commandsData[0]-2-i)*commandsData[1];
      //timing = millis();
      //if (millis() - timing > commandsData[1]){
      //  timing = millis();
      //  i--;
      //}
      i--;
      delayMicroseconds(next-micros());
    }
  } else {
    Serial.println("No such command. Try RECORD");
  }
    ifGreeting=0;
    commandName = "";
  }
}