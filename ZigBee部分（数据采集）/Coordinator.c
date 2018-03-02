/****************************************************************
* 程序功能：                                         *
* 作者：                                                 *
* 地点：                                              *
* 时间：                                            *
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
            

//输入/输出簇ID列表
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
  GENERICAPP_ENDPOINT,              //  int    端口号
  GENERICAPP_PROFID,                //  uint16 Profile ID
  GENERICAPP_DEVICEID,              //  uint16 设备 ID
  GENERICAPP_DEVICE_VERSION,        //  int    设备版本
  GENERICAPP_FLAGS,                 //  int    程序标识
  GENERICAPP_MAX_STATUS_CLUSTERS,          //  byte   输入命令数
  (cId_t *)GenericApp_InputClusterList,    //  byte   输入命令列表地址
  GENERICAPP_MAX_CONTROL_CLUSTERS,         //  byte   输出命令数
  (cId_t *)GenericApp_OutputClusterList    //  byte   输出命令列表地址
};

endPointDesc_t GenericApp_epDesc;   //端口描述符

byte GenericApp_TaskID;             // 任务ID
byte GenericApp_TransID;            // 传输信息量计数


void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pkt );
void GenericApp_SendTheMessage( void );
static void uartRxCB(uint8 port,uint8 event);


void GenericApp_Init( byte task_id )
{
  halUARTCfg_t uartConfig;
  GenericApp_TaskID = task_id;                                                  //GenericApp任务ID
  GenericApp_TransID = 0;                                                       //清零传输信息量的计数

  GenericApp_epDesc.endPoint = GENERICAPP_ENDPOINT;                             //配置endpoint 
  GenericApp_epDesc.task_id = &GenericApp_TaskID;
  GenericApp_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&GenericApp_SimpleDesc;
  GenericApp_epDesc.latencyReq = noLatencyReqs;

  afRegister( &GenericApp_epDesc );                                             //注册端口描述符
 
  
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
  afIncomingMSGPacket_t *MSGpkt;                                                //定义数据包指针
  if ( events & SYS_EVENT_MSG )                                                 //用户层处理函数的系统事件
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericApp_TaskID );    //获取数据信息 
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
        case AF_INCOMING_MSG_CMD:                                  //接收到命令，然后执行。zigbee协议信息的传递有两种方式：消息和命令，消息长度不限，命令的大小则严格规定
          GenericApp_MessageMSGCB( MSGpkt );
          break;
        default:                                  
          break;
      }

      osal_msg_deallocate( (uint8 *)MSGpkt );                                   //释放内存
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericApp_TaskID );  //接收下一个事件
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


static void uartRxCB(uint8 port,uint8 event)   //测：去掉static
{
  uartRX uartbuf;
  uint16 shortaddr;
  uint8 command;
  afAddrType_t my_DstAddr;

 
  if ( event != HAL_UART_TX_EMPTY )                                          //串口接收事件确认
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
                         (uint8*)&command,                   //注：不用强制转换，将会编译出错，提示类型不匹配
                         &GenericApp_TransID,
                         AF_DISCV_ROUTE, AF_DEFAULT_RADIUS );
    }
    else
    {
      AF_DataRequest( &my_DstAddr, &GenericApp_epDesc,
                         LED_OFF,
                         1,  
                         (uint8*)&command,                   //注：不用强制转换，将会编译出错，提示类型不匹配
                         &GenericApp_TransID,
                         AF_DISCV_ROUTE, AF_DEFAULT_RADIUS );
    }
  }
}
