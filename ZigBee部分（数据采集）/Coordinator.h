
#ifndef GENERICAPP_H
#define GENERICAPP_H

#include "ZComDef.h"

#define GENERICAPP_ENDPOINT           10                                        //端口号

#define GENERICAPP_PROFID             0x0F04                                    //Profile ID
#define GENERICAPP_DEVICEID           0x0001                                    //设备 ID
#define GENERICAPP_DEVICE_VERSION     0                                         //设备版本
#define GENERICAPP_FLAGS              0                                         //程序标识

#define GENERICAPP_MAX_CONTROL_CLUSTERS     2                                         //输入簇 ID数量
#define LED_ON                              1                                         //开灯命令
#define LED_OFF                             0                                         //关灯命令
#define GENERICAPP_MAX_STATUS_CLUSTERS      1                                         //输出簇 ID数量
#define LED_STATUS                          1                                         //LED灯开、关状态命令

#define ID_ADDR 0x1        //地址ID
#define ID_MEASURE_T_H_L 0x02  //温湿度、光强检测 ID
#define ID_COMMAND 0x3       //命令ID


typedef struct
{
  uint8 head;
  uint8 length;
  uint8 TransportID;              //会话ID
  uint8 shortaddr[2];
//  uint8 signstemp; //温度正负，1为正，0为负
  int8 temp;      //温度数值
  uint8 humi;      //相对湿度值
  uint8 light[2];     //光照强度值
  uint8 checksum; 
} Measuredatatype;
  
typedef struct
{
  uint8 head;
  uint8 length;
  uint8 TransportID;              //会话ID
  uint8 shortaddr[2];                   //若用uint16 shortaddr代替，收到的数据将不一样
  uint8 status;
  uint8 checksum; 
}sendmessagetype;  

//命令
typedef struct
{
  uint8 head;
  uint8 length;
  uint8 TransportID;              //会话ID
  uint8 shortaddr[2];
  uint8 command;
}uartRX;

extern void GenericApp_Init( byte task_id );
extern UINT16 GenericApp_ProcessEvent( byte task_id, UINT16 events );

#endif /* GENERICAPP_H */
