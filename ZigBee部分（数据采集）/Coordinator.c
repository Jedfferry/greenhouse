/****************************************************************
* �����ܣ�                                         *
* ���ߣ�                                                 *
* �ص㣺                                              *
* ʱ�䣺                                            *
****************************************************************/
#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"
#include <string.h>

#include "Coordinator.h"
#include "DebugTrace.h"

#if !defined( WIN32 )
  #include "OnBoard.h"
#endif

/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"
            

//����/�����ID�б�
const cId_t GenericApp_OutputClusterList[GENERICAPP_MAX_CONTROL_CLUSTERS] =
{
  LED_ON,
  LED_OFF
};

const cId_t GenericApp_InputClusterList[GENERICAPP_MAX_STATUS_CLUSTERS] =
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
  GENERICAPP_MAX_STATUS_CLUSTERS,          //  byte   ����������
  (cId_t *)GenericApp_InputClusterList,    //  byte   ���������б��ַ
  GENERICAPP_MAX_CONTROL_CLUSTERS,         //  byte   ���������
  (cId_t *)GenericApp_OutputClusterList    //  byte   ��������б��ַ
};

endPointDesc_t GenericApp_epDesc;   //�˿�������

byte GenericApp_TaskID;             // ����ID
byte GenericApp_TransID;            // ������Ϣ������


void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pkt );
void GenericApp_SendTheMessage( void );
static void uartRxCB(uint8 port,uint8 event);


void GenericApp_Init( byte task_id )
{
  halUARTCfg_t uartConfig;
  GenericApp_TaskID = task_id;                                                  //GenericApp����ID
  GenericApp_TransID = 0;                                                       //���㴫����Ϣ���ļ���

  GenericApp_epDesc.endPoint = GENERICAPP_ENDPOINT;                             //����endpoint 
  GenericApp_epDesc.task_id = &GenericApp_TaskID;
  GenericApp_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&GenericApp_SimpleDesc;
  GenericApp_epDesc.latencyReq = noLatencyReqs;

  afRegister( &GenericApp_epDesc );                                             //ע��˿�������
 
  
  uartConfig.configured           = TRUE;
  uartConfig.baudRate             = HAL_UART_BR_115200;
  uartConfig.flowControl          = FALSE;
  uartConfig.flowControlThreshold = 48;
  uartConfig.rx.maxBufSize        = 128;
  uartConfig.tx.maxBufSize        = 256;  
  
  uartConfig.idleTimeout          = 6;   
  uartConfig.intEnable            = TRUE;              
  uartConfig.callBackFunc         = uartRxCB;
  HalUARTOpen(0,&uartConfig);
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
  return 0;
}

void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
  if(pkt->cmd.Data[2] == ID_ADDR)
  {  
     HalUARTWrite(0,(uint8*)pkt->cmd.Data,pkt->cmd.DataLength);
  }
  
  if(pkt->cmd.Data[2] == ID_MEASURE_T_H_L)
  {
     HalUARTWrite(0,(uint8*)pkt->cmd.Data,pkt->cmd.DataLength);
  }
}


static void uartRxCB(uint8 port,uint8 event)   //�⣺ȥ��static
{
  uartRX uartbuf;
  uint16 shortaddr;
  uint8 command;
  afAddrType_t my_DstAddr;

 
  if ( event != HAL_UART_TX_EMPTY )                                          //���ڽ����¼�ȷ��
  {     
    HalUARTRead(0,(uint8*)&uartbuf,sizeof(uartbuf));
  
    shortaddr = BUILD_UINT16(uartbuf.shortaddr[1],uartbuf.shortaddr[0]);
    command   = uartbuf.command;
 
    my_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
    my_DstAddr.endPoint = GENERICAPP_ENDPOINT;
    my_DstAddr.addr.shortAddr = shortaddr;
    
    
    if(command)
    {
      AF_DataRequest( &my_DstAddr, &GenericApp_epDesc,
                         LED_ON,
                         1,  
                         (uint8*)&command,                   //ע������ǿ��ת����������������ʾ���Ͳ�ƥ��
                         &GenericApp_TransID,
                         AF_DISCV_ROUTE, AF_DEFAULT_RADIUS );
    }
    else
    {
      AF_DataRequest( &my_DstAddr, &GenericApp_epDesc,
                         LED_OFF,
                         1,  
                         (uint8*)&command,                   //ע������ǿ��ת����������������ʾ���Ͳ�ƥ��
                         &GenericApp_TransID,
                         AF_DISCV_ROUTE, AF_DEFAULT_RADIUS );
    }
  }
}
