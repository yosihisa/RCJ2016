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

#define X_H 0 //方位X上位バイト
#define X_L 1 //方位X下位バイト
#define Y_H 2 //方位Y上位バイト
#define Y_L 3 //方位Y下位バイト
#define Z_H 4 //方位Y上位バイト
#define Z_L 5 //方位Z下位バイト
#define ARG 6 //方位角

#define IR0 7  //ボールセンサ0
#define IR1 8  //ボールセンサ1
#define IR2 9  //ボールセンサ2
#define IR3 10 //ボールセンサ3
#define IR4 11 //ボールセンサ4
#define IR5 12 //ボールセンサ5

#define URM_F 13 //超音波前
#define URM_L 14 //超音波左
#define URM_B 15 //超音波後
#define URM_R 16 //超音波右

#define L_F 2 //ラインセンサ　前
#define L_R 0 //ラインセンサ　右
#define L_L 1 //ラインセンサ　左
//motor.cpp
void motor(int angle,unsigned char power,signed  char cor);
