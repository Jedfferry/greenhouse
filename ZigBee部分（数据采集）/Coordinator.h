
#ifndef GENERICAPP_H
#define GENERICAPP_H

#include "ZComDef.h"

#define GENERICAPP_ENDPOINT           10                                        //�˿ں�

#define GENERICAPP_PROFID             0x0F04                                    //Profile ID
#define GENERICAPP_DEVICEID           0x0001                                    //�豸 ID
#define GENERICAPP_DEVICE_VERSION     0                                         //�豸�汾
#define GENERICAPP_FLAGS              0                                         //�����ʶ

#define GENERICAPP_MAX_CONTROL_CLUSTERS     2                                         //����� ID����
#define LED_ON                              1                                         //��������
#define LED_OFF                             0                                         //�ص�����
#define GENERICAPP_MAX_STATUS_CLUSTERS      1                                         //����� ID����
#define LED_STATUS                          1                                         //LED�ƿ�����״̬����

#define ID_ADDR 0x1        //��ַID
#define ID_MEASURE_T_H_L 0x02  //��ʪ�ȡ���ǿ��� ID
#define ID_COMMAND 0x3       //����ID


typedef struct
{
  uint8 head;
  uint8 length;
  uint8 TransportID;              //�ỰID
  uint8 shortaddr[2];
//  uint8 signstemp; //�¶�������1Ϊ����0Ϊ��
  int8 temp;      //�¶���ֵ
  uint8 humi;      //���ʪ��ֵ
  uint8 light[2];     //����ǿ��ֵ
  uint8 checksum; 
} Measuredatatype;
  
typedef struct
{
  uint8 head;
  uint8 length;
  uint8 TransportID;              //�ỰID
  uint8 shortaddr[2];                   //����uint16 shortaddr���棬�յ������ݽ���һ��
  uint8 status;
  uint8 checksum; 
}sendmessagetype;  

//����
typedef struct
{
  uint8 head;
  uint8 length;
  uint8 TransportID;              //�ỰID
  uint8 shortaddr[2];
  uint8 command;
}uartRX;

extern void GenericApp_Init( byte task_id );
extern UINT16 GenericApp_ProcessEvent( byte task_id, UINT16 events );

#endif /* GENERICAPP_H */
