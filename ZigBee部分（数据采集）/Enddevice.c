
#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"
#include <string.h>

#include "Coordinator.h"
#include "DebugTrace.h"
#include "HumiTempLight.h"

#if !defined( WIN32 )
  #include "OnBoard.h"
#endif

/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"

#define SEND_DATA_EVENT 0x0001  //���ͽڵ��ַ
#define MEASURE_EVENT   0x0002  //���
#define USER_EVENT      0x00FF  //�û��¼�

//����/�����ID�б�
const cId_t GenericApp_InputClusterList[GENERICAPP_MAX_CONTROL_CLUSTERS] =
{
  LED_ON,
  LED_OFF
};

const cId_t GenericApp_OutputClusterList[GENERICAPP_MAX_STATUS_CLUSTERS] =
{
  LED_STATUS
};

const SimpleDescriptionFormat_t GenericApp_SimpleDesc =
{
  GENERICAPP_ENDPOINT,              //  int    �˿ں�
  GENERICAPP_PROFID,                //  uint16 Profile ID
  GENERICAPP_DEVICEID,              //  uint16 �豸 ID
  GENERICAPP_DEVICE_VERSION,        //  int    �豸�汾
  GENERICAPP_FLAGS,                 //  int    �����ʶ
  GENERICAPP_MAX_CONTROL_CLUSTERS,          //  byte   ����������
  (cId_t *)GenericApp_InputClusterList,     //  byte   ���������б��ַ
  GENERICAPP_MAX_STATUS_CLUSTERS,           //  byte   ���������
  (cId_t *)GenericApp_OutputClusterList     //  byte   ��������б��ַ
};

endPointDesc_t GenericApp_epDesc;   //�˿�������

byte GenericApp_TaskID;             // ����ID
byte GenericApp_TransID;            // ������Ϣ������
devStates_t GenericApp_NwkState;    //�豸�������е�����

void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pkt );
void GenericApp_SendTheMessage(uint16 events);
extern void ctrPCA9554LED(uint8 led,uint8 operation);    //ȱ��externȥ���������ͨ��������ִ�����������������ڶ��庯��ʱʡ��extern��������Ϊ����extern
extern void ctrPCA9554FLASHLED4(uint8 FLASHnum);
extern uint8 readLED5status(void);

void GenericApp_Init( byte task_id )
{
  GenericApp_TaskID = task_id;                                                  //GenericApp����ID
  GenericApp_NwkState = DEV_INIT;
  GenericApp_TransID = 0;                                                       //���㴫����Ϣ���ļ���

  GenericApp_epDesc.endPoint = GENERICAPP_ENDPOINT;                             //����endpoint 
  GenericApp_epDesc.task_id = &GenericApp_TaskID;
  GenericApp_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&GenericApp_SimpleDesc;
  GenericApp_epDesc.latencyReq = noLatencyReqs;

  afRegister( &GenericApp_epDesc );                                             //ע��˿�������
 }

UINT16 GenericApp_ProcessEvent( byte task_id, UINT16 events )
{
  afIncomingMSGPacket_t *MSGpkt;                                                //�������ݰ�ָ��
  if ( events & SYS_EVENT_MSG )                                                 //�û��㴦������ϵͳ�¼�
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericApp_TaskID );    //��ȡ������Ϣ 
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
        case ZDO_STATE_CHANGE:                                                  //���յ����Ȼ��ִ�С�zigbeeЭ����Ϣ�Ĵ��������ַ�ʽ����Ϣ�������Ϣ���Ȳ��ޣ�����Ĵ�С���ϸ�涨
          GenericApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
          if(GenericApp_NwkState == DEV_END_DEVICE)
          {
            osal_set_event(GenericApp_TaskID,SEND_DATA_EVENT);
          }
          break;
        case AF_INCOMING_MSG_CMD:                                  //���յ����Ȼ��ִ�С�zigbeeЭ����Ϣ�Ĵ��������ַ�ʽ����Ϣ�������Ϣ���Ȳ��ޣ�����Ĵ�С���ϸ�涨
          GenericApp_MessageMSGCB( MSGpkt );
          break;
        default:                                  
          break;
      }

      osal_msg_deallocate( (uint8 *)MSGpkt );                                   //�ͷ��ڴ�
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericApp_TaskID );  //������һ���¼�
    }

    return (events ^ SYS_EVENT_MSG);
  }
  
  if( events & USER_EVENT )
  {
    GenericApp_SendTheMessage(events);
  }

  return 0;
}

void GenericApp_SendTheMessage(uint16 events)
{
  afAddrType_t my_DstAddr;
  my_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
  my_DstAddr.endPoint = GENERICAPP_ENDPOINT;
  my_DstAddr.addr.shortAddr = 0x0000;  
  
 
  if(events & SEND_DATA_EVENT)
  {
      sendmessagetype data;
      uint16 shortaddr;
      shortaddr = NLME_GetShortAddr();
      
      data.shortaddr[1] = LO_UINT16(shortaddr);
      data.shortaddr[0] = HI_UINT16(shortaddr);      
      data.head = 0xFB;
      data.length = sizeof(data);
      data.TransportID = ID_ADDR;
      
      data.status = readLED5status();
  
      data.checksum = (data.head ^ data.length^data.TransportID^data.shortaddr[0]^data.shortaddr[1]^
                          data.status);
      
      AF_DataRequest( &my_DstAddr, &GenericApp_epDesc,
                       LED_STATUS,
                       sizeof(data),  
                       (uint8*)&data,                   //ע������ǿ��ת����������������ʾ���Ͳ�ƥ��
                       &GenericApp_TransID,
                       AF_DISCV_ROUTE, AF_DEFAULT_RADIUS ); 
      osal_start_timerEx(GenericApp_TaskID,SEND_DATA_EVENT,3000);
  }

  if(events & MEASURE_EVENT)
  {
      int t;     //�¶�
      int h;     //ʪ��
      float l;   //����ǿ��

      uint16 ml;
      uint16 shortaddr;
      
      Measuredatatype mdata;
      
      th_read(&t,&h );  //��ȡ��ʪ��ֵ
      light_measure(&l);  //��ȡ����ǿ��ֵ
      ml = (uint16)(l);

            
      shortaddr = NLME_GetShortAddr();      
      mdata.shortaddr[1] = LO_UINT16(shortaddr);
      mdata.shortaddr[0] = HI_UINT16(shortaddr);  
      
      mdata.head   = 0xFB;
      mdata.length = sizeof(mdata);
      mdata.TransportID = ID_MEASURE_T_H_L;
//      mdata.signstemp = (((t) < (-t)) ? (0) : (1));
      mdata.temp = (int8)t;
      mdata.humi = (uint8)(h);
      
      mdata.light[1] = (uint8)ml;
      mdata.light[0] = (uint8)(ml>>8);
      
   
      mdata.checksum = (mdata.head^mdata.length^mdata.TransportID^mdata.shortaddr[0]^mdata.shortaddr[1]^
                        mdata.temp^mdata.humi^mdata.light[0]^mdata.light[1]);
      
      AF_DataRequest( &my_DstAddr, &GenericApp_epDesc,
                   LED_STATUS,
                   sizeof(mdata),  
                   (uint8*)&mdata,                   //ע������ǿ��ת����������������ʾ���Ͳ�ƥ��
                   &GenericApp_TransID,
                   AF_DISCV_ROUTE, AF_DEFAULT_RADIUS );
      
      osal_start_timerEx(GenericApp_TaskID,MEASURE_EVENT,2000);
  }
  
  ctrPCA9554FLASHLED4(1);                               //���л�һ��
}


void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
  uint8 command;

  osal_memcpy(&command,pkt->cmd.Data,1);
  if(pkt->clusterId == LED_ON && command == 2)
  {
      osal_stop_timerEx(GenericApp_TaskID,SEND_DATA_EVENT);
      osal_clear_event(GenericApp_TaskID,SEND_DATA_EVENT);      
      ctrPCA9554LED(4,0);
  }
  if(pkt->clusterId == LED_ON && command == 1)
  {
      osal_set_event(GenericApp_TaskID,MEASURE_EVENT);
      ctrPCA9554LED(5,1);
  }
  if(pkt->clusterId == LED_OFF && command == 0)
  {
      osal_stop_timerEx(GenericApp_TaskID,MEASURE_EVENT);
      osal_clear_event(GenericApp_TaskID,MEASURE_EVENT);      
      ctrPCA9554LED(5,0);
      ctrPCA9554LED(4,0);
  }
}


