/*
 * header.h
 *
 * Created: 2015/11/07 20:11:26
 *  Author: yosihisa
 */ 

/*
#ifndef HEADER_H_
#define HEADER_H_





#endif  HEADER_H_ */

#define F_CPU 20000000UL  // 20 MHz
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h> 
#include <string.h>
#include <stdlib.h>

#include "USART.h"

#define PI M_PI

#define X_H 0 //����X��ʃo�C�g
#define X_L 1 //����X���ʃo�C�g
#define Y_H 2 //����Y��ʃo�C�g
#define Y_L 3 //����Y���ʃo�C�g
#define Z_H 4 //����Y��ʃo�C�g
#define Z_L 5 //����Z���ʃo�C�g
#define ARG 6 //���ʊp

#define IR0 7  //�{�[���Z���T0
#define IR1 8  //�{�[���Z���T1
#define IR2 9  //�{�[���Z���T2
#define IR3 10 //�{�[���Z���T3
#define IR4 11 //�{�[���Z���T4
#define IR5 12 //�{�[���Z���T5

#define URM_F 13 //�����g�O
#define URM_L 14 //�����g��
#define URM_B 15 //�����g��
#define URM_R 16 //�����g�E

#define L_F 2 //���C���Z���T�@�O
#define L_R 0 //���C���Z���T�@�E
#define L_L 1 //���C���Z���T�@��
//motor.cpp
void motor(int angle,unsigned char power,signed  char cor);
