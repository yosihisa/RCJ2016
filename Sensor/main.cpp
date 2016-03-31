#include "Header.h"

#define HMC5883L_WRITE 0x3C
#define HMC5883L_READ 0x3D

#define HMC5883L_CONFIG_A    0x00 
#define HMC5883L_CONFIG_B    0x01 
#define HMC5883L_MODE        0x02 
#define HMC5883L_GAIN    0b00000000 //レンジ
#define HMC5883L_M	4				//倍率

#define X_OFFSET -461	//方位　X補正
#define Y_OFFSET -643	//方位　Y補正

#define IR_CYCLE 255	//ボールセンサ　サンプリング回数

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

#define T_1 17 //超音波右


unsigned char compass,data[32],urm[4]={0,0,0,0};

//SPI割り込み
ISR(SPI_STC_vect){
	unsigned char num =SPDR;
	switch(num){
		case 0x00:
			SPDR=data[X_H];
			break;
		case 0x01:
			SPDR=data[X_L];
			break;
		case 0x02:
			SPDR=data[Y_H];
			break;
		case 0x03:
			SPDR=data[Y_L];
			break;
		case 0x04:
			SPDR=data[Z_H];
			break;
		case 0x05:
			SPDR=data[Z_L];
			break;
		case 0x06:
			SPDR=data[ARG];
			break;
			
		case IR0:
			SPDR=data[IR0];
			break;
		case IR1:
			SPDR=data[IR1];
			break;
		case IR2:
			SPDR=data[IR2];
			break;
		case IR3:
			SPDR=data[IR3];
			break;
		case IR4:
			SPDR=data[IR4];
			break;
		case IR5:
			SPDR=data[IR5];
			break;
			
		case URM_F:
			SPDR=data[URM_F];
			break;
		case URM_L:
			SPDR=data[URM_L];
			break;
		case URM_B:
			SPDR=data[URM_B];
			break;
		case URM_R:
			SPDR=data[URM_R];
			break;
		default:
			SPDR=0xFF;
			break;
	}
	
}

//超音波用タイマ
ISR(PCINT1_vect){
	char C = PINC;
	char c = PINC^0xFF;
	/*if((C & (0b00001110) ) && urm[1] ==0&& urm[2] ==0 && urm[3] ==0){
		TCNT0 =0;
	}*/

	//前
	if((C & (1<<PINC0) )&& urm[0] ==0 ){
		urm[0]=TCNT0;
	}
	if(c & (1<<PINC0) && urm[0] !=0 ){
		data[URM_F]=TCNT0-urm[0];
		urm[0]=0;
	}

	//右
	if((C & (1<<PINC1) )&& urm[1] ==0 ){
		urm[1]=TCNT0;
	}
	if(c & (1<<PINC1) && urm[1] !=0 ){
		data[URM_L]=TCNT0-urm[1];
		urm[1]=0;
	}

	//後
	if((C & (1<<PINC2) )&& urm[2] ==0 ){
		urm[2]=TCNT0;
	}
	if(c & (1<<PINC2) && urm[2] !=0 ){
		data[URM_B]=TCNT0-urm[2];
		urm[2]=0;
	}

	//左
	if((C & (1<<PINC3) )&& urm[3] ==0 ){
		urm[3]=TCNT0;
	}
	if(c & (1<<PINC3) && urm[3] !=0 ){
		data[URM_R]=TCNT0-urm[3];
		urm[3]=0;
	}
}


int main(void)
{
	
	//I2C設定
	i2c_init();
	_delay_ms(15);
	
	i2c_start(HMC5883L_WRITE);
	i2c_write(HMC5883L_CONFIG_A); 
	i2c_write(0x78); // 平均化8回 75Hz 通常測定
	i2c_stop();

	i2c_start(HMC5883L_WRITE);
	i2c_write(HMC5883L_CONFIG_B); 
	i2c_write(HMC5883L_GAIN);
	i2c_stop();

	i2c_start(HMC5883L_WRITE);
	i2c_write(HMC5883L_MODE); // set pointer to measurement mode
	i2c_write(0x00); // continous measurement
	i2c_stop();
	
	//入出力設定
	DDRB  =0b00010000; //SPI IR入力
	DDRD  =0b00001100; //トリガー　IR電源　IR入力
	PORTD =0b00001000; //IR電源OFF
	
	
	//超音波関連
	
	//割り込み許可
	PCICR  =0b00000010;
	PCMSK1 =0b00001111;
	
	//タイマー
	TCCR0A = 0b00000000;
	TCCR0B = 0b00000101;
	
	//トリガー用タイマー初期化
	TCCR1A = 0b00000000;
	TCCR1B = 0b00000100;
	
	
	
	//SPI設定
	SPCR=0b11000000;
	SPDR=0;
	sei();
	_delay_ms(15);
    while (1) {
		int x_raw,y_raw;
		unsigned long ir[6]={0,0,0,0,0,0};
		float arg;
		//方位データ読み出し
		PORTD = 0b00000000;//IR電源ON
		i2c_start(HMC5883L_WRITE);
		i2c_write(0x03); 
		i2c_stop();

		i2c_start(HMC5883L_READ);
		
		data[X_H]=i2c_read_ack();
		data[X_L]=i2c_read_ack();
		data[Y_H]=i2c_read_ack();
		data[Y_L]=i2c_read_ack();
		data[Z_H]=i2c_read_ack();
		data[Z_L]=i2c_read_nack();
		
		x_raw=(data[X_H]<<8)+data[X_L];
		y_raw=(data[Z_H]<<8)+data[Z_L];
		
		x_raw= x_raw+X_OFFSET;
		y_raw= y_raw+Y_OFFSET;
		
		x_raw*= HMC5883L_M;
		y_raw*= HMC5883L_M;
		
		arg = atan2((double)x_raw,(double)y_raw);
		arg = arg/M_PI;
		data[ARG]=(char)(arg*(128));
	
		i2c_stop();
		
		for(int i=0;i<IR_CYCLE;i++){
			char d = PIND^0xFF;
			char b = PINB^0xFF;
			if(d & (1<<PIND4))ir[0]++;
			if(d & (1<<PIND5))ir[1]++;
			if(d & (1<<PIND6))ir[2]++;
			if(d & (1<<PIND7))ir[3]++;
			if(b & (1<<PINB0))ir[4]++;
			if(b & (1<<PINB1))ir[5]++;
			_delay_us(15);
		}
		PORTD = 0b00001000;//IR電源OFF
		data[IR0]=ir[0];
		data[IR1]=ir[1];
		data[IR2]=ir[2];
		data[IR3]=ir[3];
		data[IR4]=ir[4];
		data[IR5]=ir[5];
		
		int t =TCNT1L;//←よくわからないが入れると動く
		if(TCNT1H > 10 ){ //測距
			PORTD =0b00000100;
			_delay_us(1000);
			PORTD =0b00000000;
			TCNT1H =0;
			TCNT1L =0;
			TCNT0 =0;
		}
		
		_delay_ms(15);
		
    }
}

