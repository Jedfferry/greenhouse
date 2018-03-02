/**********************************************************************************************************
* �� �� ����iic.C
* ��    �ܣ��˳�����I2C����ƽ̨������ʽ�����ƽ̨���ĵײ��C�ӳ���,�緢�����ݼ���������,Ӧ��λ����
* Ӳ�����ӣ�����CC2530��I/O��ģ��IIC��SCL��SDA
*
*           P1.0 ------ SCL
*           P1.1 ------ SDA
*           
* ��    ����V1.0
* ��    �ߣ�WUXIANHAI
* ��    �ڣ�2010.12.16
* �¶�˹������ҳ��www.ourselec.com
**************************************************************************************************************/
#include "ioCC2530.h"
#include "hal_mcu.h"

#define SCL          P1_0 
#define SDA          P1_1

#define IO_DIR_PORT_PIN(port, pin, dir)  \
   do {                                  \
      if (dir == IO_OUT)                 \
         P##port##DIR |= (0x01<<(pin));  \
      else                               \
         P##port##DIR &= ~(0x01<<(pin)); \
   }while(0)


#define IO_IN   0
#define IO_OUT  1

#define TMP275_I2CADDR  0x92

static uint8 ack;	         /*Ӧ���־λ*/

static void QWait(void);
static void Wait(unsigned int ms);
static void Start_I2c(void);
static void Stop_I2c(void);
static void  SendByte(uint8 c);
static uint8  RcvByte(void);
static void Ack_I2c(uint8 a);
static uint8 ISendByte(uint8 sla,uint8 c);
static uint8 ISendStr(uint8 sla,uint8 suba,uint8 *s,uint8 no);
void PCA9554ledInit(void);
void ctrPCA9554FLASHLED4(uint8 FLASHnum);   //LED4��˸
void ctrPCA9554FLASHLED5(uint8 FLASHnum);   //LED5��˸
uint8 readLED5status(void);
void ctrPCA9554LED(uint8 led,uint8 operation);
void ctrPCA9554Buzzer(uint8 operation);
uint8 ctrPCA9554Relay(uint8 cmd);
uint8 ctrPCA9554SRelay(uint8 cmd);
uint8 ctrPCA9554Key(void);
uint8 write24L01(uint8 *data,uint8 addr,uint8 Len);
uint8 read24L01byte(uint8 addr);

uint8 PCA9554ledstate = 0;

void QWait()     //1us����ʱ
{
    asm("NOP");asm("NOP");
    asm("NOP");asm("NOP");
    asm("NOP");asm("NOP");
    asm("NOP");asm("NOP");
    asm("NOP");asm("NOP");
    asm("NOP");

}

static void Wait(unsigned int ms)
{
                    
   unsigned char g,k;
   while(ms)
   {
      
	  for(g=0;g<=167;g++)
	   {
	     for(k=0;k<=48;k++);
	   }
      ms--;                            
   }
} 

/*******************************************************************
                     �����ߺ���               
����ԭ��: void  Start_I2c();  

��    ��: ����I2C����,������I2C��ʼ����.
  
********************************************************************/
static void Start_I2c()
{
  IO_DIR_PORT_PIN(1, 0, IO_OUT);    //����P1.0Ϊ���
  IO_DIR_PORT_PIN(1, 1, IO_OUT);    //����P1.1Ϊ���
  
  SDA=1;   /*������ʼ�����������ź�*/
  asm("NOP");
  SCL=1;
  QWait();    /*��ʼ��������ʱ�����4.7us,��ʱ*/
  QWait();
  QWait();
  QWait();
  QWait();    
  SDA=0;   /*������ʼ�ź�*/
  QWait();    /* ��ʼ��������ʱ�����4��s*/
  QWait();
  QWait();
  QWait();
  QWait();       
  SCL=0;   /*ǯסI2C���ߣ�׼�����ͻ�������� */
  asm("NOP");
  asm("NOP");
}

/*******************************************************************
                      �������ߺ���               
����ԭ��: void  Stop_I2c();  

��    ��: ����I2C����,������I2C��������.
  
********************************************************************/
static void Stop_I2c()
{
  IO_DIR_PORT_PIN(1, 0, IO_OUT);    //����P1.0Ϊ���
  IO_DIR_PORT_PIN(1, 1, IO_OUT);    //����P1.1Ϊ���
  SDA=0;  /*���ͽ��������������ź�*/
  asm("NOP");   /*���ͽ���������ʱ���ź�*/
  SCL=1;  /*������������ʱ�����4��s*/
  QWait();
  QWait();
  QWait();
  QWait();
  QWait();
  SDA=1;  /*����I2C���߽����ź�*/
  QWait();
  QWait();
  QWait();
  QWait();
}


/*******************************************************************
                 �ֽ����ݴ��ͺ���               
����ԭ��: void  SendByte(uchar c);

��    ��: ������c���ͳ�ȥ,�����ǵ�ַ,Ҳ����������,�����ȴ�Ӧ��,����
          ��״̬λ���в���.(��Ӧ����Ӧ��ʹack=0 ��)     
          ��������������ack=1; ack=0��ʾ��������Ӧ����𻵡�
********************************************************************/
static void  SendByte(uint8 c)
{
 uint8 BitCnt;
 IO_DIR_PORT_PIN(1, 0, IO_OUT);    //����P1.0Ϊ���
 IO_DIR_PORT_PIN(1, 1, IO_OUT);    //����P1.1Ϊ���
 for(BitCnt=0;BitCnt<8;BitCnt++)  /*Ҫ���͵����ݳ���Ϊ8λ*/
    {
     if((c<<BitCnt)&0x80)SDA=1;   /*�жϷ���λ*/
       else  SDA=0;                
      asm("NOP");
     SCL=1;               /*��ʱ����Ϊ�ߣ�֪ͨ��������ʼ��������λ*/
      QWait(); 
      QWait();               /*��֤ʱ�Ӹߵ�ƽ���ڴ���4��s*/
      QWait();
      QWait();
      QWait();         
     SCL=0; 
    }    
    QWait();
    QWait();
    QWait();
    SDA=1;               /*8λ��������ͷ������ߣ�׼������Ӧ��λ*/
    asm("NOP");
    IO_DIR_PORT_PIN(1, 1, IO_IN);  
    SCL=1;
    QWait();
    QWait();
    QWait();
    QWait();
    if(SDA==1)ack=0;     
    else ack=1;        /*�ж��Ƿ���յ�Ӧ���ź�*/
    SCL=0;   
    QWait();
    QWait();
    IO_DIR_PORT_PIN(1, 1, IO_OUT);
}

/*******************************************************************
                 �ֽ����ݴ��ͺ���               
����ԭ��: uchar  RcvByte();

��    ��: �������մ���������������,���ж����ߴ���(����Ӧ���ź�)��
          ���������Ӧ������  
********************************************************************/	
static uint8  RcvByte()
{
  uint8 retc;
  uint8 BitCnt;
  IO_DIR_PORT_PIN(1, 0, IO_OUT);    //����P1.0Ϊ���
  IO_DIR_PORT_PIN(1, 1, IO_OUT);    //����P1.1Ϊ���
  retc=0; 
  SDA=1;             /*��������Ϊ���뷽ʽ*/
  IO_DIR_PORT_PIN(1, 1, IO_IN);
  for(BitCnt=0;BitCnt<8;BitCnt++)
      {
        asm("NOP");          
        SCL=0;       /*��ʱ����Ϊ�ͣ�׼����������λ*/
        QWait();
        QWait();         /*ʱ�ӵ͵�ƽ���ڴ���4.7��s*/
        QWait();
        QWait();
        QWait();
        SCL=1;       /*��ʱ����Ϊ��ʹ��������������Ч*/
        QWait();
        QWait();
        retc=retc<<1;
        if(SDA==1)retc=retc+1; /*������λ,���յ�����λ����retc�� */
        QWait();
        QWait(); 
      }
  SCL=0;    
  QWait();
  QWait();
  IO_DIR_PORT_PIN(1, 1, IO_OUT);
  return(retc);
}

/********************************************************************
                     Ӧ���Ӻ���
ԭ��:  void Ack_I2c(uint a);
 
����:����������Ӧ���ź�,(������Ӧ����Ӧ���ź�)
********************************************************************/
static void Ack_I2c(uint8 a)
{
  IO_DIR_PORT_PIN(1, 0, IO_OUT);    //����P1.0Ϊ���
  IO_DIR_PORT_PIN(1, 1, IO_OUT);    //����P1.1Ϊ���
  if(a==0)SDA=0;     /*�ڴ˷���Ӧ����Ӧ���ź� */
  else SDA=1;
  QWait();
  //QWait();
  //QWait();      
  SCL=1;
  QWait();
  QWait();              /*ʱ�ӵ͵�ƽ���ڴ���4��s*/
  QWait();
  QWait();
  QWait();  
  SCL=0;                /*��ʱ���ߣ�ǯסI2C�����Ա��������*/
  QWait();
  //QWait();    
}

/*******************************************************************
                    �����ӵ�ַ���������ֽ����ݺ���               
����ԭ��: uint  ISendByte(uchar sla,ucahr c);

��    ��:  ���������ߵ����͵�ַ�����ݣ��������ߵ�ȫ����,��������ַsla.
           �������1��ʾ�����ɹ��������������

ע    �⣺ ʹ��ǰ�����ѽ������ߡ�
********************************************************************/
static uint8 ISendByte(uint8 sla,uint8 c)
{
   Start_I2c();               /*��������*/
   SendByte(sla);            /*����������ַ*/
     if(ack==0)return(0);
   SendByte(c);               /*��������*/
     if(ack==0)return(0);
  Stop_I2c();                 /*��������*/ 
  return(1);
}

/*******************************************************************
                    �����ӵ�ַ�������Ͷ��ֽ����ݺ���               
����ԭ��: uint  ISendStr(uchar sla,uchar suba,ucahr *s,uchar no);  

��    ��: ���������ߵ����͵�ַ���ӵ�ַ,���ݣ��������ߵ�ȫ����,������
          ��ַsla���ӵ�ַsuba������������sָ������ݣ�����no���ֽڡ�
          �������1��ʾ�����ɹ��������������

ע    �⣺ʹ��ǰ�����ѽ������ߡ�
********************************************************************/
static uint8 ISendStr(uint8 sla,uint8 suba,uint8 *s,uint8 no)
{
   uint8 i;

   Start_I2c();               /*��������*/
   SendByte(sla);            /*����������ַ*/
     if(ack==0)return(0);
   SendByte(suba);            /*���������ӵ�ַ*/
     if(ack==0)return(0);

   for(i=0;i<no;i++)
    {   
     SendByte(*s);               /*��������*/
       if(ack==0)return(0);
     s++;
    } 
 Stop_I2c();                 /*��������*/ 
  return(1);
}


/*******************************************************************
                    �����ӵ�ַ�������ֽ����ݺ���               
����ԭ��: uint  IRcvByte(uchar sla,ucahr *c);  

��    ��: ���������ߵ����͵�ַ�������ݣ��������ߵ�ȫ����,��������
          ַsla������ֵ��c. �������1��ʾ�����ɹ��������������

ע    �⣺ʹ��ǰ�����ѽ������ߡ�
********************************************************************/
static uint8 IRcvByte(uint8 sla,uint8 *c)
{
   Start_I2c();                /*��������*/
   SendByte(sla+1);           /*����������ַ*/
  // SendByte(sla);   //chang by wu 2011.1.13
   if(ack==0)return(0);
   *c=RcvByte();               /*��ȡ����*/
   Ack_I2c(1);               /*���ͷǾʹ�λ*/
   Stop_I2c();                  /*��������*/ 
   return(1);
}


void PCA9554ledInit()
{
  uint8 output = 0x00;
  uint8 *data = 0;
  if(ISendStr(0x40,0x03,&output,1))  //д����
  {
    output = 0xbf;
    if(ISendStr(0x40,0x01,&output,1))
    {
      if(IRcvByte(0x40,data))
      {
        PCA9554ledstate = *data;
      }
    }
  }
}

void ctrPCA9554FLASHLED4(uint8 FLASHnum)   //LED4��˸
{
  uint8 output = 0x00;
  uint8 *data = 0;
  if(ISendStr(0x40,0x03,&output,1))  //д����
  {
    while(FLASHnum)
    {
     output = PCA9554ledstate & 0x20;
     if (output)
    {
      output = PCA9554ledstate & 0xdf;
    }
    else
    {
      output = PCA9554ledstate | 0x20;
    }
     if(ISendStr(0x40,0x01,&output,1))
    {
      if(IRcvByte(0x40,data))
      {
        PCA9554ledstate = *data;
      }
    }
    FLASHnum --;
    Wait(5000);
    }
  }
}

void ctrPCA9554FLASHLED5(uint8 FLASHnum)   //LED5��˸
{
  uint8 output = 0x00;
  uint8 *data = 0;
  if(ISendStr(0x40,0x03,&output,1))  //д����
  {
    while(FLASHnum)
    {
    output = PCA9554ledstate & 0x10;
    if (output)
    {
      output = PCA9554ledstate & 0xef;
    }
    else
    {
      output = PCA9554ledstate | 0x10;
    }
     if(ISendStr(0x40,0x01,&output,1))
    {
      if(IRcvByte(0x40,data))
      {
        PCA9554ledstate = *data;
      }
    }
    FLASHnum --;
    Wait(50);
    }
  }
}

uint8 readLED5status(void)
{
  uint8 ledstate = 0;
  uint8 *data = 0;
  if(IRcvByte(0x40,data))
      {
        ledstate = *data;
      }
  if(ledstate & (1<<4))
    return 0;
  else
    return 1;
}

void ctrPCA9554LED(uint8 led,uint8 operation)
{
  uint8 output = 0x00;
  uint8 *data = 0;
  if(ISendStr(0x40,0x03,&output,1))  //д����
  {
    switch(led)
    {
      case 0:
        if (operation)
        {
          output = PCA9554ledstate & 0xfe;
        }
        else
        {
          output = PCA9554ledstate | 0x01;
        }
      break;
       case 1:
        if (operation)
        {
          output = PCA9554ledstate & 0xfd;
        }
        else
        {
          output = PCA9554ledstate | 0x02;
        }
      break;
       case 2:
        if (operation)
        {
          output = PCA9554ledstate & 0xf7;
        }
        else
        {
          output = PCA9554ledstate | 0x08;
        }
      break;
       case 3:
        if (operation)
        {
          output = PCA9554ledstate & 0xfb;
        }
        else
        {
          output = PCA9554ledstate | 0x04;
        }
      break;
       case 4:
        if (operation)
        {
          output = PCA9554ledstate & 0xdf;
        }
        else
        {
          output = PCA9554ledstate | 0x20;
        }
      break;
       case 5:
        if (operation)
        {
          output = PCA9554ledstate & 0xef;
        }
        else
        {
          output = PCA9554ledstate | 0x10;
        }
      break;

     default:break;
    }
    if(ISendStr(0x40,0x01,&output,1))
    {
      if(IRcvByte(0x40,data))
      {
        PCA9554ledstate = *data;
      }
    }
  }
}

void ctrPCA9554Buzzer(uint8 operation)
{
  uint8 output = 0x00;
  uint8 *data = 0;
  if(ISendStr(0x40,0x03,&output,1))  //д����
  {
        if (operation)
        {
          output = PCA9554ledstate | 0x40;
        }
        else
        {
          output = PCA9554ledstate & 0xbf;
        }
      if(ISendStr(0x40,0x01,&output,1))
     {
      if(IRcvByte(0x40,data))
      {
        PCA9554ledstate = *data;
      }
    }
  }
}

uint8 ctrPCA9554Relay(uint8 cmd)
{
  uint8 output = 0x00;
  uint8 *data = 0;
  if(ISendStr(0x44,0x03,&output,1))  //д����
  {
    output = cmd & 0x0f;
    if(ISendStr(0x44,0x01,&output,1))
    {
      if(IRcvByte(0x44,data))
      {
        return *data;
      }
    }
  }
  return 0;
}

uint8 ctrPCA9554SRelay(uint8 cmd)
{
  uint8 output = 0x00;
  uint8 *data = 0;
  if(ISendStr(0x48,0x03,&output,1))  //д����
  {
    output = cmd & 0x0f;
    if(ISendStr(0x48,0x01,&output,1))
    {
      if(IRcvByte(0x48,data))
      {
        return *data;
      }
    }
  }
  return 0;
}

uint8 ctrPCA9554Key()
{
  uint8 input = 0xff;
  uint8 *data = 0;
  if(ISendStr(0x42,0x03,&input,1))  //д����
  {   
    if(ISendByte(0x42,0x00))  //��������
    {
     if(IRcvByte(0x42,data))
     {
       return *data;
      }
    }
  }
  return 0;
}

uint8 write24L01(uint8 *data,uint8 addr,uint8 Len)
{
  if(ISendStr(0xa0,addr,data,Len))
  {
    return 1;
  }
  else
  {
     return 0;
  } 
}

uint8 read24L01byte(uint8 addr)
{
  uint8 data = 0;
  if(ISendByte(0xa0,addr))
  {
    Start_I2c();                /*��������*/
   SendByte(0xa1);           /*����������ַ*/
   if(ack==0)
   {
     return(0);
   }
   else
   {
   data = RcvByte();               /*��ȡ����*/
   Ack_I2c(1);               /*���ͷǾʹ�λ*/
   Stop_I2c();                  /*��������*/ 
   return data;
   }
  }
  return 0;
}
