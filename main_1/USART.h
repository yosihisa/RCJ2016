// USART.H
#ifndef __USART__H__
#define __USART__H__

/* sio�ݒ� */
void sio_init(unsigned int baud,int bit)
{
	unsigned int ubrr = (((F_CPU>>4)+(baud>>1))/baud-1);
	// UBRR��ݒ肷��Ƃ��Ɋۂߏ��������Ă����B

	UBRR0H = (unsigned char)(ubrr>>8);    // �{�[���[�g���8bit
	UBRR0L = (unsigned char)ubrr;        // �{�[���[�g����8bit
	UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0) | (1<<TXCIE0);
	// ����M����,����M���荞�݋���
	switch(bit)
	{
		case 8:
		UCSR0C = (3<<UCSZ00) ;        // stopbit 1bit , 8bit���M
		break;
		case 5:
		UCSR0C = 0;                 // stopbit 1bit , 5bit���M
	}
}

// byte���`���Ă����B
typedef unsigned char byte;

// �t���[��������Ȃ��̂�256 bytes�̑���Mbuffer�����O�ŗp�ӂ���
volatile char usart_recvData[256];    // USART�Ŏ�M�����f�[�^�Bring buffer
volatile byte usart_recv_write;        // ���݂�write�ʒu(usart_recvData��index)
byte usart_recv_read;         // ���݂�read�ʒu(usart_recvData��index)
volatile char usart_sendData[256];    // USART�ő��M����f�[�^�Bring buffer
byte usart_send_write;        // ���݂�write�ʒu(usart_sendData��index)
volatile byte usart_send_read;         // ���݂�read�ʒu(usart_sendData��index)

char usart_RD[256],usart_string[256];
char usart_R = 0;

// �f�[�^����M���Ă��邩�̃`�F�b�N�B��M���Ă���Ȃ��0�B
int is_received(){
	return  (usart_recv_write !=  usart_recv_read) ? 1 : 0;
	// read�ʒu��write�ʒu���قȂ�Ȃ�Ύ�M�f�[�^������͂�
}

// �f�[�^����M����܂őҋ@����
void wait_for_receiving(){
	while(!is_received())
	;
}

// ��M�����f�[�^��Ԃ��B��M�����f�[�^���Ȃ��ꍇ�͎�M����܂őҋ@�B
int getReceivedData(){
	wait_for_receiving();
	return usart_recvData[usart_recv_read++];
}

// ���荞�݂ɂ���M
ISR(USART_RX_vect){
	//usart_recvData[usart_recv_write++] = UDR0;    // ��M�f�[�^����M�o�b�t�@�Ɋi�[
	if(UDR0>33 && UDR0 <127){
		usart_RD[usart_R] =UDR0;
		usart_R++;
	}
	if(UDR0 == 13 || UDR0 ==10){//���s�̏ꍇ
		sprintf(usart_string,usart_RD);
		usart_R=0;
		for (int i=0;i<256;i++){
			usart_RD[i]=0;
		}
	}
}

// ���M�o�b�t�@�Ƀf�[�^������΁A��������1�o�C�g���M���郋�[�`���B
// �����I�Ɏg�p���Ă��邾���Ȃ̂Ń��[�U�[�͌Ăяo���Ȃ��ŁB
void private_send_char(){
	if (usart_send_write != usart_send_read)
	UDR0 = usart_sendData[usart_send_read++];// ���M�o�b�t�@�̃f�[�^�𑗐M
}

// ���荞�݂ɂ�鑗�M
ISR(USART_TX_vect){
	private_send_char();
}

// 1�o�C�g���M
void sendChar(int c){
	// ���M�o�b�t�@�������ς��Ȃ�҂�
	while(((usart_send_write + 1) & 0xff) == usart_send_read)
	;

	// ���͂Ƃ����ꑗ�M�o�b�t�@�Ƀf�[�^��ςށB
	usart_sendData[usart_send_write++] = c;

	// ���M���W�X�^���Z�b�g����Ă��� == ���M�ł����ԁ@�Ȃ�΁A
	// ��x�������M���Ă����B
	if (UCSR0A & (1<<UDRE0))
	private_send_char();

	// �Ⴆ�Ύ��̂悤�ɑ��M�o�b�t�@�Ƀf�[�^��ς܂���UDR0�ɒ��ڃA�N�Z�X����R�[�h��
	// �悭�Ȃ��B
	// if (UCSR0A & (1<<UDRE0))
	//    UDR0 = c;
	// else
	//    usart_sendData[usart_send_write++] = c;
	// ����́Aelse�傪���s�����u�Ԃ�USART_TX_vect�ɂ�銄�荞�݂�������A
	// usart_send_write == usart_send_read�ł������ꍇ�A����sendChar���Ăяo�����
	// ���̑��M����������܂ł����Őς񂾃f�[�^�����M����Ȃ�����ł���B
}

// ������̑��M
void sendString(char *p){
	while(*p)
	sendChar(*p++);
}

//���l��������ϊ�
void IntToString(char *str, int number)
{
	sprintf(str, "%d", number);
}

#endif