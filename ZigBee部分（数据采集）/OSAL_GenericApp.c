/***************************************************************************************************************
* �� �� ����OSAL_GenericApp.c
��
* ��    �ܣ��ṩӦ����GenericApp����Ĳ���ϵͳ�ӿ�
*           
*
*
* ע    �⣺
*           
*           
*           
*           
*
* ��    ����V1.0
* ��    �ߣ�WU XIANHAI
* ��    �ڣ�2011.2.28
* �¶�˹������ҳ��www.ourselec.com
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
//����������
const pTaskEventHandlerFn tasksArr[] = {
  macEventLoop,                                                                 //MAC����������
  nwk_event_loop,                                                               //�������������
  Hal_ProcessEvent,                                                             //��Ӳ���������������
#if defined( MT_TASK )
  MT_ProcessEvent,                                                              //����������������ѡ��
#endif
  APS_event_loop,                                                               //Ӧ�ò������������û���ò�Ҫ�޸ģ�
#if defined ( ZIGBEE_FRAGMENTATION )
  APSF_ProcessEvent,                                                            //APS����������
#endif
  ZDApp_event_loop,                                                             //zigbee�豸Ӧ�ò��������������Ը�����Ҫ����
#if defined ( ZIGBEE_FREQ_AGILITY ) || defined ( ZIGBEE_PANID_CONFLICT )
  ZDNwkMgr_event_loop,                                                          //ZDO����������������
#endif
  GenericApp_ProcessEvent                                                       //�û�Ӧ�ò������������û��ɸ����Լ���Ҫ����
};

const uint8 tasksCnt = sizeof( tasksArr ) / sizeof( tasksArr[0] );
uint16 *tasksEvents;

/*********************************************************************
 * FUNCTIONS
 *********************************************************************/

/**************************************************************************************************
 * �������ƣ�osalInitTasks
 *
 * ������������ʼ������ϵͳ���������
 *
 * ��    ������
 *
 * �� �� ֵ����
 **************************************************************************************************/
void osalInitTasks( void )                                                      //�ڳ�ʼ������ϵͳosal_init_system()�е��� 
{
  uint8 taskID = 0;

  tasksEvents = (uint16 *)osal_mem_alloc( sizeof( uint16 ) * tasksCnt);
  osal_memset( tasksEvents, 0, (sizeof( uint16 ) * tasksCnt));

  macTaskInit( taskID++ );                                                      //MAC�������ʼ��������װ
  nwk_init( taskID++ );                                                         //����������ʼ��������װ
  Hal_Init( taskID++ );                                                         //Ӳ�������ʼ��
#if defined( MT_TASK )
  MT_TaskInit( taskID++ );                                                      //���������ʼ��
#endif
  APS_Init( taskID++ );                                                         //Ӧ��֧���Ӳ������ʼ��������װ
#if defined ( ZIGBEE_FRAGMENTATION )
  APSF_Init( taskID++ );                                                        //APS���ʼ��������װ
#endif
  ZDApp_Init( taskID++ );                                                       //�豸Ӧ�ò��ʼ��
#if defined ( ZIGBEE_FREQ_AGILITY ) || defined ( ZIGBEE_PANID_CONFLICT )
  ZDNwkMgr_Init( taskID++ );                                                    //ZDO���������γ�ʼ��
#endif
  GenericApp_Init( taskID );                                                    //�û�Ӧ�ò�ӿڳ�ʼ��
}

/*********************************************************************
*********************************************************************/
