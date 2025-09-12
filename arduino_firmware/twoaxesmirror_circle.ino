#include "Wire.h"
#include "MCP4725.h"

#define PARSE_AMOUNT 3         // число значений в массиве, который хотим получить
#define start_com '[' 
#define end_com ']' 
#define delCom '=' 

MCP4725 MCP_X(0x60);
MCP4725 MCP_Y(0x61);

int intData[PARSE_AMOUNT];
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
//void loop()
//{
//  if (Serial.available()) 
//    {
//      parseCommand(Serial.readStringUntil("\r\n"));
//    }
//}

void parseCommand(String com) 
{
  //Добавил переменную длины команды.
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
  Serial.println("Enter the command: [RECORD=numSteps pause(ms) angle]");  
  ifGreeting = 1;
}

void parsing() {
  if (Serial.available() > 0) {
    char incomingByte = Serial.read();        // обязательно ЧИТАЕМ входящий символ
    if (getStarted) {                         // если приняли начальный символ (парсинг разрешён)
      if (incomingByte != ' ' && incomingByte != end_com) {   // если это не пробел И не конец
        string_convert += incomingByte;       // складываем в строку
      } else {                                // если это пробел или ; конец пакета
        intData[index] = string_convert.toInt();  // преобразуем строку в int и кладём в массив
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

void record(int *Array){
  for (byte i = 0; i < PARSE_AMOUNT; i++) { // выводим элементы массива
      Serial.print(Array[i]); Serial.print(" ");
    } Serial.println();

  for (int i=0; i < Array[0]; i++){
    //Serial.print(i);
    //digitalWrite(MOS_PIN1, HIGH);
    //digitalWrite(MOS_PIN1, LOW);
    MCP_X.setValue(Array[2]*(i+1));
    delay(Array[1]);
  }
  MCP_X.setValue(0);
}

void loop() {
  if (ifGreeting == 0) {greeting();};
  parsing();       // функция парсинга
  if (recievedFlag) {                           // если получены данные
    recievedFlag = false;
  
  if (commandName.equalsIgnoreCase("RECORD")){
    Serial.println("RECORD");
    record(intData);
  } else {
    Serial.println("No such command. Try RECORD");
  }
    ifGreeting=0;
    commandName = "";
  }
}