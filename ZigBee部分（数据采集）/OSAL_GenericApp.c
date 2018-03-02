/***************************************************************************************************************
* 文 件 名：OSAL_GenericApp.c
×
* 功    能：提供应用于GenericApp所需的操作系统接口
*           
*
*
* 注    意：
*           
*           
*           
*           
*
* 版    本：V1.0
* 作    者：WU XIANHAI
* 日    期：2011.2.28
* 奥尔斯电子主页：www.ourselec.com
******************************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */

#include "ZComDef.h"
#include "hal_drivers.h"
#include "OSAL.h"
#include "OSAL_Tasks.h"

#if defined ( MT_TASK )
  #include "MT.h"
  #include "MT_TASK.h"
#endif

#include "nwk.h"
#include "APS.h"
#include "ZDApp.h"
#if defined ( ZIGBEE_FREQ_AGILITY ) || defined ( ZIGBEE_PANID_CONFLICT )
  #include "ZDNwkMgr.h"
#endif
#if defined ( ZIGBEE_FRAGMENTATION )
  #include "aps_frag.h"
#endif

#include "Coordinator.h"

/*********************************************************************
 * GLOBAL VARIABLES
 */

// The order in this table must be identical to the task initialization calls below in osalInitTask.
//任务函数数组
const pTaskEventHandlerFn tasksArr[] = {
  macEventLoop,                                                                 //MAC层任务处理函数
  nwk_event_loop,                                                               //网络层任务处理函数
  Hal_ProcessEvent,                                                             //板硬件抽象层任务处理函数
#if defined( MT_TASK )
  MT_ProcessEvent,                                                              //调试任务处理函数（可选）
#endif
  APS_event_loop,                                                               //应用层任务处理函数（用户最好不要修改）
#if defined ( ZIGBEE_FRAGMENTATION )
  APSF_ProcessEvent,                                                            //APS层任务处理函数
#endif
  ZDApp_event_loop,                                                             //zigbee设备应用层任务处理函数，可以根据需要更改
#if defined ( ZIGBEE_FREQ_AGILITY ) || defined ( ZIGBEE_PANID_CONFLICT )
  ZDNwkMgr_event_loop,                                                          //ZDO网络管理层任务处理函数
#endif
  GenericApp_ProcessEvent                                                       //用户应用层任务处理函数，用户可根据自己需要生成
};

const uint8 tasksCnt = sizeof( tasksArr ) / sizeof( tasksArr[0] );
uint16 *tasksEvents;

/*********************************************************************
 * FUNCTIONS
 *********************************************************************/

/**************************************************************************************************
 * 函数名称：osalInitTasks
 *
 * 功能描述：初始化操作系统各层的任务
 *
 * 参    数：无
 *
 * 返 回 值：无
 **************************************************************************************************/
void osalInitTasks( void )                                                      //在初始化操作系统osal_init_system()中调用 
{
  uint8 taskID = 0;

  tasksEvents = (uint16 *)osal_mem_alloc( sizeof( uint16 ) * tasksCnt);
  osal_memset( tasksEvents, 0, (sizeof( uint16 ) * tasksCnt));

  macTaskInit( taskID++ );                                                      //MAC层任务初始化，被封装
  nwk_init( taskID++ );                                                         //网络层任务初始化，被封装
  Hal_Init( taskID++ );                                                         //硬件任务初始化
#if defined( MT_TASK )
  MT_TaskInit( taskID++ );                                                      //调试任务初始化
#endif
  APS_Init( taskID++ );                                                         //应用支持子层任务初始化，被封装
#if defined ( ZIGBEE_FRAGMENTATION )
  APSF_Init( taskID++ );                                                        //APS层初始化，被封装
#endif
  ZDApp_Init( taskID++ );                                                       //设备应用层初始化
#if defined ( ZIGBEE_FREQ_AGILITY ) || defined ( ZIGBEE_PANID_CONFLICT )
  ZDNwkMgr_Init( taskID++ );                                                    //ZDO网络管理层任初始化
#endif
  GenericApp_Init( taskID );                                                    //用户应用层接口初始化
}

/*********************************************************************
*********************************************************************/
