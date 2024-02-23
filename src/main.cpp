#define BLINKER_WIFI

#include "main.h"

//开关按键引脚
const int SwitchKey = 12;
//开关按键引脚
const int ResetKey = 13;
//状态检测引脚
const int PowerState = 14;

char auth[] = "282b932d6b07";
char ssid[] = "JMZN-NO.1";
char pswd[] = "JMZN@20170620";
// char ssid[] = "iPhone";
// char pswd[] = "321162955";

Ticker timer1;
BlinkerButton Button1("PowerKey");
BlinkerButton Button2("ResetKey");

/* 电源状态检测变量 */
uint8_t PowerDetState = 0;
/* 定时器中断标志位 */
uint8_t TimerFlag = 0;

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
  Blinker.notify("Wait for the system to restart");
  Blinker.vibrate(500);
  PowerSwitchOut(PowerReset);
  Blinker.delay(500);
}

void BuiltinSwitch(const String & state)
{
  if(state == "on" || state == "off")
  {
    PowerSwitchOut(PowerSwitch);
  }
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
    Button1.color("#00AA00");
    Button1.text("ON");
    Button1.print("on");
    BUILTIN_SWITCH.print("on");
  }
  else if (digitalRead(PowerState) == LOW)
  {
    Button1.color("#FF0000");
    Button1.text("OFF");
    Button1.print("off");
    BUILTIN_SWITCH.print("off");
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
  * @brief   定时器中断服务函数
  * @param   None
  * @return  None
  * @version v1.0.0
  * @date    2024-02-23
  */
void TIMER_PeriodElapsedCallback(void)
{
  TimerFlag = 1;
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
  /* GPIO初始化 */
  pinMode(PowerState, INPUT_PULLUP);
  pinMode(SwitchKey, OUTPUT);
  pinMode(ResetKey, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(SwitchKey, LOW);
  digitalWrite(ResetKey, LOW);

  /* Blinker组件初始化 */
  Blinker.begin(auth, ssid, pswd);
  Button1.attach(Button1Callback);
  Button2.attach(Button2Callback);
  BUILTIN_SWITCH.attach(BuiltinSwitch);
  Blinker.attachHeartbeat(heartbeat);

  /* 判断连接是否成功 */
  while(!Blinker.connected())
  {
    digitalWrite(LED_BUILTIN, LOW);
    Blinker.delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    Blinker.delay(200);
  }

  /* 成功后LED灯常亮 */
  digitalWrite(LED_BUILTIN, LOW);

  /* 开启外部中断功能 */
  attachInterrupt(digitalPinToInterrupt(PowerState), Funcation, CHANGE);

  /* 开启定时器中断 周期2秒 用于连接状态检测 */
  timer1.attach(2, TIMER_PeriodElapsedCallback);
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
  /* Blinker主函数 */
  Blinker.run();
  
  /* 每2秒判断一次设备连接状态 */
  if(TimerFlag)
  {
    Blinker.connected() ? digitalWrite(LED_BUILTIN, LOW) : digitalWrite(LED_BUILTIN, HIGH);
    TimerFlag = 0;
  }
}