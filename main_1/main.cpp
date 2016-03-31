#include "header.h"
#include <math.h>

#define GOAL 0 //攻めるGOALの方向
#define LINE 20 //ラインセンサー　閾値

char line[3];

//SPI通信
 unsigned char spi( unsigned char data){
	 SPDR = data;
	 while(!(SPSR & (1<<SPIF))); //送受信待機
	 _delay_us(10);
	 SPDR = data;
	 while(!(SPSR & (1<<SPIF))); //送受信待機
	 return SPDR;
 }
 
 //アナログ
 char line_ad(){
	 ADMUX  = 0b00100000;
	 ADCSRA = 0b11010110;
	 while(ADCSRA & 0x40);
	 line[0]=ADCH;
	 ADMUX  = 0b00100001;
	 ADCSRA = 0b11010110;
	 while(ADCSRA & 0x40);
	 line[1]=ADCH;
	 ADMUX  = 0b00100010;
	 ADCSRA = 0b11010110;
	 while(ADCSRA & 0x40);
	 line[2]=ADCH;
	 
	 if(line[L_F] < LINE)return -1;
	 if(line[L_L] < LINE)return -1;
	 if(line[L_R] < LINE)return -1;
	 return 0;
 }
 char line_check(void);
 void debug(void);
 void loop(void);
  
 int main(void){
	
	//入出力設定
	DDRD =0b11111100; //モーター
	DDRB =0b00101100; //SPI・切替SW
	PORTB=0b00000001; //切替SWプルアップ

	
	//SPI設定
	SPCR =0b01010001;
	SPSR|=0b00000000;
	
	//PWM設定
	TCCR0A=0b10100001;
	TCCR0B=0b00000100;
	TCCR2A=0b00100001;
	TCCR2B=0b00000101;
	
	//USART
	UBRR0 = 129;			//ボーレート9600 @20MHz
	UCSR0A = 0b00000000;	//受信すると10000000になる 送信有効になると00100000になる
	UCSR0B = 0b00011000;	//送受信有効
	UCSR0C = 0b00000110;	//データ8bit 非同期,パリティなし Stop 1bit
	
	sio_init(9600,8);    // SIO設定
	sei(); // 全割り込み許可

	//char str[32];
	//sendString("Hello! UART world\n");
	
	OCR0B=0;
	OCR2B=0;
	OCR0A=0;
	
	_delay_ms(1000);
	
	while (1) {
		if(PINB & 0b00000001 )loop();
		else debug();
	}
}

 void debug(){
	char str[32];
	unsigned int raw_x = 0,raw_y = 0,raw_z = 0;
	unsigned char ir[6],urm[4];
	signed  char arg =0;
	motor(0,0,0);
	
	raw_x = spi(X_H)<<8;
	raw_x |= spi(X_L);
	 
	raw_y = spi(Y_H)<<8;
	raw_y |= spi(Y_L);
	 
	raw_z = spi(Z_H)<<8;
	raw_z |= spi(Z_L);
	
	arg = spi(ARG);
	
	ir[0] = spi(IR0);
	ir[1] = spi(IR1);
	ir[2] = spi(IR2);
	ir[3] = spi(IR3);
	ir[4] = spi(IR4);
	ir[5] = spi(IR5);
	line_ad();
	
	urm[0] = spi(URM_F);
	urm[1] = spi(URM_L);
	urm[2] = spi(URM_B);
	urm[3] = spi(URM_R);
	
	char uart[256];
	int ary[6]={0};
	
	sprintf(uart,usart_string);
	ary[0] = atoi(strtok(uart,","));
	ary[1] = atoi(strtok(NULL,","));
	ary[2] = atoi(strtok(NULL,","));
	ary[3] = atoi(strtok(NULL,","));
	
	//遠隔制御
	if(ary[0]==10)motor(ary[1],ary[2],ary[3]);
	
	sprintf(str, "%d,%d,%d,,%d,", raw_x,raw_y,raw_z,arg);//方位
	sendString(str);
	sprintf(str, ",%d,%d,%d,%d,%d,%d,",ir[0],ir[1],ir[2],ir[3],ir[4],ir[5]);//ボール
	//sprintf(str, ",%d,%d,%d,%d,%d,%d,",ary[0],ary[1],ary[2],ary[3],ir[4],ir[5]);//ボール
	
	sendString(str);
	sprintf(str, ",%d,%d,%d,",line[0],line[1],line[2]);//ライン
	sendString(str);
	sprintf(str, ",%d,%d,%d,%d,",urm[0],urm[1],urm[2],urm[3]);//超音波
	sendString(str);
	sendString(usart_string);
	sendString("\n");
	_delay_ms(100);
	
 }
 
 void loop(){
	unsigned char ir[7],ir_max=0,ir_max2 =0,urm[4];
	unsigned int raw_x = 0,raw_z = 0;
	signed  char arg =0,goal=0;
	
	unsigned int cor,wait = 0;
	unsigned char speed;
	
	raw_x = spi(X_H)<<8;
	raw_x |= spi(X_L);
	raw_z = spi(Z_H)<<8;
	raw_z |= spi(Z_L);
	arg = spi(ARG);
	ir[6] = spi(IR0);
	ir[0] = ir[6]*1.05;
	ir[1] = spi(IR1);
	ir[2] = spi(IR2);
	ir[3] = spi(IR3)*0.8;
	ir[4] = spi(IR4);
	ir[5] = spi(IR5);
	line_ad();
	
	urm[0] = spi(URM_F);
	urm[1] = spi(URM_L);
	urm[2] = spi(URM_B);
	urm[3] = spi(URM_R);
	
	//IRの最大値を探索
	for(int i=0;i<6;i++){
		if(ir[i]>ir[ir_max])ir_max=i;
	}
	//2番目を探索
	for(int i=0;i<6;i++){
		if(i!=ir_max && ir[i]>ir[ir_max2])ir_max2=i;
	}
	
	if(ir[ir_max]>5){
		switch(ir_max){
			case 0:
			//	speed=210;
			//	cor = 180;
				if(urm[0]<15 && ir[6]>113){
					speed=210;
					cor = 180;
					wait = 200;
				}else{
					speed=225;
					cor = 180;
				}
				int x_all,x_lp;//左右の位置　左右の和,和に対する左の割合
				x_all = urm[1]+urm[3];
				if(x_all>120){
					x_lp = (urm[1]*100)/x_all;
				
					if(x_lp<39)goal=-20;
					if(x_lp>58)goal=20;
				}
				break;
			case 1:
				if(ir_max2 ==0 )cor = 270;
				if(ir_max2 ==2 )cor = 300;
				speed=200;
				break;
			case 2:
				speed=230;
				if(ir[2]>120)cor = 30;
				else         cor = 0;
				break;
			case 3://真後ろ
				if(ir_max2 ==2)cor = 330;
				if(ir_max2 ==4)cor = 30;
				speed=200;
				break;
			case 4:
				speed=230;
				if(ir[4]>120)cor = 330;
				else         cor = 0;
				break;
			case 5:
				if(ir_max2 ==0 )cor = 90;
				if(ir_max2 ==4 )cor = 60;
				speed=210;
				break;
		}
	}else{
		cor =0;
		speed =0;
	}
	
	do{
		arg = arg-goal;
		if(arg<5 && arg>-5)arg=0;
		else {
			unsigned char s =0;
			s=255-speed;
			if(s>126)s=126;
			if(speed==0)s=40;
			
			if(arg>0) arg=s;
			else arg=s*(-1.0);
		}
		if(line_ad() == 0)motor(cor,speed,arg);
		else{
			line_check();
		}
		if(wait!=0){
			wait--;
			arg = spi(ARG);
			_delay_ms(1);
		}
	}while(wait!=0);
	 
 }
 
 
 //ラインを出た場合
 char line_check(){
	//line_ad();
	if(line[L_F] < LINE){
		motor(0,255,0);
		_delay_ms(200);
		return -1;
	}
	if(line[L_L] < LINE){
		motor(90,255,0);
		_delay_ms(200);
		return -1;
	}
	if(line[L_R] < LINE){
		motor(270,255,0);
		_delay_ms(200);
		return -1;
	}
	
	return 0;
}
