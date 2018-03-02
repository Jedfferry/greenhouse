#ifndef HUMITEMPLIGHT
#define HUMITEMPLIGHT

extern void Wait(unsigned int ms);
extern void QWait(void);
extern void initIO(void);
extern char s_write_byte(unsigned char value);
extern char s_read_byte(unsigned char ack);
extern void s_transstart(void);
extern void s_connectionreset(void);
extern char s_measure( unsigned char *p_checksum, unsigned char mode);

extern void th_read(int *t,int *h ); //读取温湿度值
extern void light_measure(float *l);//读取光照强度值

#endif