/*******************************************************************************
*                 
*                 		       普中科技
--------------------------------------------------------------------------------
* 实 验 名		 : RS232串口试验
* 实验说明       : 单片机将接收到的数据以10进制显示在LCD1602上面。
* 连接方式       : 见连接图
* 注    意		 : 
*******************************************************************************/

#include<reg51.h>
#include"lcd.h"
#include <intrins.h>
#define Data_SIZE 10

sbit Key_Receive=P3^2;
sbit Key_Send=P3^3;

sbit dula=P2^6;
sbit wela=P2^7;
//--定义全局变量--//
unsigned char code table[14] = "Received data:";
unsigned flag,a;

char RevBuf[Data_SIZE];      //数据接收缓冲区
char temp[Data_SIZE];        //防数据抵消缓冲区
unsigned char flished_flag=0; //数据接收符合要求标志
int data_count=0;    //数据长度
int temp_length;    //数据长度
int data_flished_count = 0;  //
char data_flished;           //	

unsigned char states=0;
//unsigned char   temp1;
//unsigned char  dis_buf;            //显示缓存
//unsigned char   num;

//--声明全局函数--//	   
void UsartConfiguration();
void SendData();
void SendString(unsigned char *p);
void SendChar(unsigned char Char);
//unsigned char  key_scan(void);
//void delay(int ms);
/*******************************************************************************
* 函 数 名         : main
* 函数功能		   : 主函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void main()
{

	LCD1602_Init();
	LCD1602_Display(0x80,"Uart R/S System ",0,16);	 
	LCD1602_Display(0xc0,"  S4->R  S5->S  ",0,16);
	UsartConfiguration();
	states=0;
	dula=0;
	wela=0;
	while(1)
	{
	   if(Key_Receive==0&&Key_Send!=0)
	   {
	        Delayms(2);
			if(Key_Receive==0&&Key_Send!=0)
		  	{
	   			states=1;
		  	}
	   }
	   else if (Key_Receive!=0&&Key_Send==0)
	   {
		    Delayms(2);
			if (Key_Receive!=0&&Key_Send==0)
		  	{
	   			states=2;
		  	}		  
	   }
	   else
	   {
		 Delayms(2);
			if (Key_Receive==0&&Key_Send==0)
		  	{
	   			states=0;
		  	}
	   }

	  switch(states)
	  {
	    case 1:
			LCD1602_Display(0x80,"R char[ ] '!'end",0,16);
			LCD1602_Display(0xc0,"                ",0,16);	
			LCD1602_Display(0xc0,RevBuf,0,temp_length);
			Delayms(100);
       	break;
		
		case 2:
			 LCD1602_Display(0x80,"S char[2]'0->99'",0,16);
			 LCD1602_Display(0xc0,"                ",0,16);
			 SendData(); 
			 states=0;
			 Delayms(10);
		break;
		default:
			LCD1602_Display(0x80,"Uart R/S System ",0,16);	 
			LCD1602_Display(0xc0,"  S4->R  S5->S  ",0,16);
		break;
	  }

	



	}
}
/*******************************************************************************
* 函 数 名         :UsartConfiguration()
* 函数功能		   :设置串口
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

void UsartConfiguration()
{
	TMOD=0x20;
	TH1=0xFD;
	TL1=0xFD;
	TR1=1;
	REN=1;
	SM0=0;
	SM1=1;
	EA=1;
	ES=1;	
}
void UART_One_Interrupt_Receive(void) interrupt 4
{
 	unsigned char temp;
  	if(RI==1)
    {
        RI=0;
        temp=SBUF;
        if(temp!='!') //判断是否接收到结束符
        {
            RevBuf[data_count]=temp;// 否，就存到RevBuf【】数组中
            data_count++;
        }
        else
        {
            temp_length=data_count;//是，记录其数据长度
            data_count=0;
        }
        
    }
}

void  SendString(unsigned char *p)  
{  
    while(*p)  
    {  
        SendChar(*p);  
        p++;  
    }  
}  
void SendChar(unsigned char Char)  
{     
    SBUF=Char;  
    while(!TI);  
    TI=0;  
} 
void SendData()
{
    static unsigned char number=0;
    char str[2]="  ";
	while(1)
	{
	    Delayms(100);
		number++;
		if(number>=100)
		{
			number=0;
		}
		if(number>=0&&number<100)
		{
	             //键值入显示缓存

		if(number>=0&&number<=9)
		  {
			 str[0]=' ';
			 str[1]=number+'0';
		  }		  
		if(number>=10&&number<=99)
		   {
			 str[0]=number/10+'0';
			 str[1]=number%10+'0';
		   }
		
			SendString(str);
		    LCD1602_Display(0xc6,str,0,2);	
		}

  		if(Key_Receive==0&&Key_Send!=0)
	   {
	        Delayms(2);
			if(Key_Receive==0&&Key_Send!=0)
		  	{
	   			states=1;
				break;
		  	}

		if(Key_Receive==0&&Key_Send!=0)
	    {
	      	    Delayms(2);
			if(Key_Receive==0&&Key_Send==0)
		  	{
				states=0;
				break;
		  	}
		}
	   }
	}

}

//unsigned char key_scan(void) //键盘扫描
//{
//	num=16;//16表示没按键输入
//	P1=0xfe; //扫描第一行
//	temp1=P1;
//	temp1&=0xf0;
//	if(temp1!=0xf0)	//检测有按键按下
//	{
//		Delayms(5);
//		temp1=P1;
//		temp1&=0xf0;
//		if(temp1!=0xf0)
//		{
//			temp1=P1;
//			switch(temp1)	  //判断按下是哪一个按键
//			{
//				case 0xee:num=1;
//					break;
//				case 0xde:num=2;
//					break;
//				case 0xbe:num=3;
//					break;
//				case 0x7e:num=12;
//					break;
//			}
//			while(temp1!=0xf0)
//			{
//				temp1=P1;
//				temp1&=0xf0;	
//			}
//		}
//	}
//
//	P1=0xfd;	//扫描第二行
//	temp1=P1;
//	temp1&=0xf0;
//	if(temp1!=0xf0)
//	{
//		Delayms(5);
//		temp1=P1;
//		temp1&=0xf0;
//		if(temp1!=0xf0)
//		{
//			temp1=P1;
//			switch(temp1)
//			{
//				case 0xed:num=4;
//					break;
//				case 0xdd:num=5;
//					break;
//				case 0xbd:num=6;
//					break;
//				case 0x7d:num=13;
//					break;
//			}
//		 	while(temp1!=0xf0)
//			{
//				temp1=P1;
//				temp1&=0xf0;	
//			}	
//		}
//	}
//
//	P1=0xfb;         //扫描第三行
//	temp1=P1;
//	temp1&=0xf0;
//	if(temp1!=0xf0)
//	{
//		Delayms(5);
//		temp1=P1;
//		temp1&=0xf0;
//		if(temp1!=0xf0)
//		{
//			temp1=P1;
//			switch(temp1)
//			{
//				case 0xeb:num=7;
//					break;
//				case 0xdb:num=8;
//					break;
//				case 0xbb:num=9;
//					break;	 
//				case 0x7b:num=14;
//					break;
//			}
//		 	while(temp!=0xf0)
//			{
//				temp1=P1;
//				temp1&=0xf0;	
//			}	
//		}
//	}
//
//	P1=0xf7;	  //扫描第四行
//	temp1=P1;
//	temp1&=0xf0;
//	if(temp1!=0xf0)
//	{
//		Delayms(5);
//		temp1=P1;
//		temp1&=0xf0;
//		if(temp1!=0xf0)
//		{
//			temp1=P1;
//			switch(temp1)
//			{
//				case 0xe7:num=0;
//					break;
//				case 0xd7:num=10;
//					break;
//				case 0xb7:num=11;
//					break;
//				case 0x77:num=15;
//					break;
//			}
//		 	while(temp1!=0xf0)
//			{
//				temp1=P1;
//				temp1&=0xf0;	
//			}	
//		}
//	}
//	return num;	   //返回按键对应编号
//}
//void delay(int ms)
//{                    //延时子程序
//int i;
//while(ms--)
//{
//  for(i = 0; i< 250; i++)
//  {
//   _nop_();	  //空执行
//   _nop_();
//   _nop_();
//   _nop_();
//  }
//}
//}