#define BLINKER_WIFI

#include "main.h"

//开关按键引脚
const int SwitchKey = 4;
//开关按键引脚
const int ResetKey = 5;

//状态检测引脚
const int PowerState = 12;

char auth[] = "";
char ssid[] = "";
char pswd[] = "";

BlinkerButton Button1("PowerKey");
BlinkerButton Button2("ResetKey");

/* 电源状态检测变量 */
uint8_t PowerDetState = 0;

void heartbeat(void);

void PowerSwitchOut(PowerStateTypeDef status)
{
  switch (status)
  {
    case PowerSwitch:
      digitalWrite(SwitchKey, HIGH);
      Blinker.delay(500);
      digitalWrite(SwitchKey, LOW);
      break;
    
    case PowerForceOFF:
      digitalWrite(SwitchKey, HIGH);
      Blinker.delay(5000);
      digitalWrite(SwitchKey, LOW);
      break;

    case PowerReset:
      digitalWrite(ResetKey, HIGH);
      Blinker.delay(500);
      digitalWrite(ResetKey, LOW);
      break;

    default:
      break;
  }
}

/**
  * @file    main.cpp
  * @brief   开关机按键回调函数
  * @param   unknow
  * @return  None
  * @version v1.0.0
  * @date    2022-08-15
  */
void Button1Callback(const String & state)
{
  Blinker.vibrate(500);
  if(state == "on" && digitalRead(PowerState) == 0)
  {
    /* 发送弹窗提醒 */
    Blinker.notify("Wait for the computer to start...");
    /* 开关切换 */
    PowerSwitchOut(PowerSwitch);
  }
  else if(state == "off" && digitalRead(PowerState) == 1)
  {
    /* 发送弹窗提醒 */
    Blinker.notify("Wait for computer to shut down...");
    /* 开关切换 */
    PowerSwitchOut(PowerSwitch);
  }
  else if(state == "press")  
  {
    /* 发送弹窗提醒 */
    Blinker.notify("Forced shutdown is triggered. Wait a few seconds to release the button");
    /* 强制关机 */
    digitalWrite(SwitchKey, HIGH);
  }
  else if(state == "pressup")
  {
    /* 发送弹窗提醒 */
    Blinker.notify("Operation is successful, please check the computer running status");
    digitalWrite(SwitchKey, LOW);
  }
  Blinker.delay(500);
  heartbeat();
}

/**
  * @file    main.cpp
  * @brief   重启按键回调函数
  * @param   unknow
  * @return  None
  * @version v1.0.0
  * @date    2022-08-15
  */
void Button2Callback(const String & state)
{
  /* 发送弹窗提醒 */
  Blinker.notify("Unused");
  Blinker.vibrate(500);
}

/**
  * @file    main.cpp
  * @brief   心跳包函数
  * @param   None
  * @return  None
  * @version v1.0.0
  * @date    2022-08-15
  */
void heartbeat()
{
  if (digitalRead(PowerState) == HIGH)
  {
    Button1.icon("fal fa-toggle-on");
    Button1.text("ON");
    Button1.print("on");
  }
  else if (digitalRead(PowerState) == LOW)
  {
    Button1.icon("fal fa-toggle-off");
    Button1.text("OFF");
    Button1.print("off");
  }
}


/**
  * @file    main.cpp
  * @brief   状态引脚检测中断服务函数
  * @param   None
  * @return  None
  * @version v1.0.0
  * @date    2022-08-15
  */
ICACHE_RAM_ATTR void Funcation()
{
  heartbeat();
}


/**
  * @file    main.cpp
  * @brief   初始化函数
  * @param   None
  * @return  None
  * @version v1.0.0
  * @date    2022-08-15
  */
void setup()
{
    pinMode(PowerState, INPUT);
    pinMode(SwitchKey, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    for(uint8_t i=0; i<5; i++)
    {
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
    }
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(SwitchKey, LOW);

    Blinker.begin(auth, ssid, pswd);
    Button1.attach(Button1Callback);
    Button2.attach(Button2Callback);
    Blinker.attachHeartbeat(heartbeat);

    attachInterrupt(digitalPinToInterrupt(12), Funcation, CHANGE);
}


/**
  * @file    main.cpp
  * @brief   主循环函数
  * @param   None
  * @return  None
  * @version v1.0.0
  * @date    2022-08-15
  */
void loop()
{
    Blinker.run();
}