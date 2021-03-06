// USART.H
#ifndef __USART__H__
#define __USART__H__

/* sio設定 */
void sio_init(unsigned int baud,int bit)
{
	unsigned int ubrr = (((F_CPU>>4)+(baud>>1))/baud-1);
	// UBRRを設定するときに丸め処理をしておく。

	UBRR0H = (unsigned char)(ubrr>>8);    // ボーレート上位8bit
	UBRR0L = (unsigned char)ubrr;        // ボーレート下位8bit
	UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0) | (1<<TXCIE0);
	// 送受信許可,送受信割り込み許可
	switch(bit)
	{
		case 8:
		UCSR0C = (3<<UCSZ00) ;        // stopbit 1bit , 8bit送信
		break;
		case 5:
		UCSR0C = 0;                 // stopbit 1bit , 5bit送信
	}
}

// byteを定義しておく。
typedef unsigned char byte;

// フロー制御をしないので256 bytesの送受信bufferを自前で用意する
volatile char usart_recvData[256];    // USARTで受信したデータ。ring buffer
volatile byte usart_recv_write;        // 現在のwrite位置(usart_recvDataのindex)
byte usart_recv_read;         // 現在のread位置(usart_recvDataのindex)
volatile char usart_sendData[256];    // USARTで送信するデータ。ring buffer
byte usart_send_write;        // 現在のwrite位置(usart_sendDataのindex)
volatile byte usart_send_read;         // 現在のread位置(usart_sendDataのindex)

char usart_RD[256],usart_string[256];
char usart_R = 0;

// データを受信しているかのチェック。受信しているなら非0。
int is_received(){
	return  (usart_recv_write !=  usart_recv_read) ? 1 : 0;
	// read位置とwrite位置が異なるならば受信データがあるはず
}

// データを受信するまで待機する
void wait_for_receiving(){
	while(!is_received())
	;
}

// 受信したデータを返す。受信したデータがない場合は受信するまで待機。
int getReceivedData(){
	wait_for_receiving();
	return usart_recvData[usart_recv_read++];
}

// 割り込みによる受信
ISR(USART_RX_vect){
	//usart_recvData[usart_recv_write++] = UDR0;    // 受信データを受信バッファに格納
	if(UDR0>33 && UDR0 <127){
		usart_RD[usart_R] =UDR0;
		usart_R++;
	}
	if(UDR0 == 13 || UDR0 ==10){//改行の場合
		sprintf(usart_string,usart_RD);
		usart_R=0;
		for (int i=0;i<256;i++){
			usart_RD[i]=0;
		}
	}
}

// 送信バッファにデータがあれば、そこから1バイト送信するルーチン。
// 内部的に使用しているだけなのでユーザーは呼び出さないで。
void private_send_char(){
	if (usart_send_write != usart_send_read)
	UDR0 = usart_sendData[usart_send_read++];// 送信バッファのデータを送信
}

// 割り込みによる送信
ISR(USART_TX_vect){
	private_send_char();
}

// 1バイト送信
void sendChar(int c){
	// 送信バッファがいっぱいなら待つ
	while(((usart_send_write + 1) & 0xff) == usart_send_read)
	;

	// 何はともあれ送信バッファにデータを積む。
	usart_sendData[usart_send_write++] = c;

	// 送信レジスタがセットされている == 送信できる状態　ならば、
	// 一度だけ送信しておく。
	if (UCSR0A & (1<<UDRE0))
	private_send_char();

	// 例えば次のように送信バッファにデータを積まずにUDR0に直接アクセスするコードは
	// よくない。
	// if (UCSR0A & (1<<UDRE0))
	//    UDR0 = c;
	// else
	//    usart_sendData[usart_send_write++] = c;
	// これは、else句が実行される瞬間にUSART_TX_vectによる割り込みがかかり、
	// usart_send_write == usart_send_readであった場合、次にsendCharが呼び出されて
	// その送信が完了するまでここで積んだデータが送信されないからである。
}

// 文字列の送信
void sendString(char *p){
	while(*p)
	sendChar(*p++);
}

//数値→文字列変換
void IntToString(char *str, int number)
{
	sprintf(str, "%d", number);
}

#endif