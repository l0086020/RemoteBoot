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

Ticker timer1;
BlinkerButton Button1("PowerKey");
BlinkerButton Button2("ResetKey");

/* 电源状态检测变量 */
uint8_t PowerDetState = 0;
/* 定时器中断标志位 */
uint8_t TimerFlag = 0;

void heartbeat(void);


/**
  * @file    main.cpp
  * @brief   开关机操作函数
  * @param   status：操作指令
  * @return  None
  * @version v1.0.0
  * @date    2024-05-18
  */
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
    Blinker.notify("Wait for the computer to shutdown...");
    /* 开关切换 */
    PowerSwitchOut(PowerSwitch);
  }
  else if(state == "press")  
  {
    /* 发送弹窗提醒 */
    Blinker.notify("Forced shutdown triggered. Please wait a few seconds before releasing the button");
    /* 强制关机 */
    digitalWrite(SwitchKey, HIGH);
  }
  else if(state == "pressup")
  {
    /* 发送弹窗提醒 */
    Blinker.notify("Operation is successful, Please check the computer running status");
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
  /* GPIO模式配置 */
  pinMode(PowerState, INPUT_PULLUP);
  pinMode(SwitchKey, OUTPUT);
  pinMode(ResetKey, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  /* GPIO初始化状态 */
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(SwitchKey, LOW);
  digitalWrite(ResetKey, LOW);

  /* Blinker组件初始化 */
  Blinker.begin(auth, ssid, pswd);
  Button1.attach(Button1Callback);
  Button2.attach(Button2Callback);
  BUILTIN_SWITCH.attach(BuiltinSwitch);
  Blinker.attachHeartbeat(heartbeat);

  /* 开启外部中断功能 */
  attachInterrupt(digitalPinToInterrupt(PowerState), Funcation, CHANGE);

  /* 开启定时器中断，1Hz */
  timer1.attach(1, TIMER_PeriodElapsedCallback);
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
  /* Blinker服务主函数 */
  Blinker.run();
  
  if(TimerFlag)  /* 1Hz */
  {
    TimerFlag = 0;
    /* 连接成功后LED常亮，否则每秒闪烁1次 */
    Blinker.connected() ? digitalWrite(LED_BUILTIN, LOW) : digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
}