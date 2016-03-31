/*
 * motor.cpp
 *
 * Created: 2015/11/07 20:11:01
 *  Author: yosihisa
 */ 
#include <avr/io.h>
#define MOTOR_AS 0b00001000 //���[�^�[A PWM
#define MOTOR_AD 0b00000100 //���[�^�[A ����
#define MOTOR_BS 0b00100000 //���[�^�[B PWM
#define MOTOR_BD 0b00010000 //���[�^�[B ����
#define MOTOR_CS 0b01000000 //���[�^�[C PWM
#define MOTOR_CD 0b10000000 //���[�^�[C ����

//���[�^�[����
// ����(30�x���Ɓ@�x���@),����(0�`255),�p�x�␳(-127�`+127)
void motor(int angle,unsigned char power,signed  char cor){
	angle/=30;
	long p[12][3]={
		{866  ,-866 ,0    },
		{1000 ,-500 ,-500 },
		{866  ,0    ,-866 },
		{500  ,500  ,-1000},
		{0    ,866  ,-866 },
		{-500 ,1000 ,-500 },
		{-866 ,866  ,0    },
		{-1000,500  ,500  },
		{-866 ,0    ,866  },
		{-500 ,-500 ,1000 },
		{0    ,-866 ,866  },
		{500  ,-1000,500  }	};
	long a=power*p[angle][0];
	long b=power*p[angle][1];
	long c=power*p[angle][2];
	
	a/=1000;
	b/=1000;
	c/=1000;
	
	a=a+cor;
	b=b+cor;
	c=c+cor;
	if(a>255)a=255;
	if(b>255)b=255;
	if(c>255)c=255;
	//A
	if(a>=0){
		PORTD&= ~MOTOR_AD;
		OCR2B= a; 
	}else{
		PORTD|=  MOTOR_AD;
		OCR2B= -1*a; 
	}
	//B
	if(b>=0){
		PORTD&= ~MOTOR_BD;
		OCR0B= b;
	}else{
		PORTD|=  MOTOR_BD;
		OCR0B= -1*b;
	}
	//C
	if(c>=0){
		PORTD&= ~MOTOR_CD;
		OCR0A= c; 
	}else{
		PORTD|=  MOTOR_CD;
		OCR0A= -1*c; 
	}
	return;
		
}