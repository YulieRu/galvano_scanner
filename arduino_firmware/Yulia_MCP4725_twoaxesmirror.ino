
#include "Wire.h"
#include "MCP4725.h"


MCP4725 MCP_X(0x60);
MCP4725 MCP_Y(0x61);

int values[1];


void setup(){
  Serial.begin(115200);
  Serial.setTimeout(100);
  
  Wire.begin();
  MCP_X.begin();
  MCP_X.setValue(0);
  MCP_Y.begin();
  MCP_Y.setValue(0);
}
void loop()
{
  if (Serial.available()) 
    {
      parseCommand(Serial.readStringUntil("\r\n"));
    }
}



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
