/*
 * uart.c
 *
 * Created: 2021. 03. 10. 10:48:14
 *  Author: Jedocs
 */ 

#include <atmel_start.h>
#include <global.h> 


//	Descriptor
struct io_descriptor *io;
	
volatile bool RX_in_progress = false;	//	Serial Receiving flag
volatile bool TX_in_progress = false;	//	Serial Transmitting flag
volatile uint8_t UART0_WaitforReturn = 0;	//	Interp return flag
volatile uint8_t parameter = 0;	//	Bytes received counter
volatile uint8_t total_Bytes = 0;

//	Sent & To be sent cursor
volatile uint8_t stage_txbuffer = 0;		//tail of circular buffer
volatile uint8_t storage_txbuffer = 0;		//head of circular buffer
volatile uint8_t chksum = 0;

char * Buffer[25] = {0};	// Sending buffer (circular)
	
extern struct report event_report;

extern bool ACK_received;
extern bool NAK_received;
	
void serial_rx_callback(const struct usart_async_descriptor *const io_descr)
{
	uint8_t ch;

	//	Read a Character
	io_read(&USART_1.io,&ch,1);

	if (!RX_in_progress)
	{
		if (ch == ACK) {
			ACK_received = true;
			return;
		}
		else if (ch == NAK) {
			NAK_received = true;
			return;
		}
	}

	if (!event_report.report_valid)
	{
		// Check for first char of Command
		if (!RX_in_progress)
		{
			if (ch == REPORT_EVENT)
			{
				//	Set receiving flag
				RX_in_progress = true;

				//	Reset Byte counter
				parameter = 1;
				chksum=REPORT_EVENT;
			}
		}
		else
		{			
			switch (parameter) {
				case 1:
					event_report.object_id=ch;
					chksum^=ch;
					parameter++;
				break;
				case 2:
					event_report.object_index=ch;
					chksum^=ch;
					parameter++;
				break;
				case 3:
					event_report.value_msb=ch;
					chksum^=ch;
					parameter++;
				break;
				case 4:
					event_report.value_lsb=ch;
					chksum^=ch;
					parameter++;
				break;
				case 5:
					event_report.cksum=ch;
					chksum^=ch;
					if (chksum == 0) {
						event_report.report_valid=true;
					}
// 					else{     felesleges, az LCD ugysem csinál semmit, a doksi hazudik
// 						ch=NAK;
// 						io_write(&USART_1.io,&ch,1);
//					}
					parameter=0;
					RX_in_progress=false;
				break;
				default:
					return;
			}
		}
	}
// 	else			felesleges, az LCD ugysem csinál semmit, a doksi hazudik
// 	{
// 		ch=NAK;
//		io_write(&USART_1.io,&ch,1);
// 	}
}

void send_uart(char * message)
{
	//    Message different than null
	if (strlen(message) != 0)
	{
		//    Input message into FIFO buffer
		Buffer[storage_txbuffer]=message;
		
		//    Increase tail cursor of circular buffer for next message
		storage_txbuffer = (storage_txbuffer + 1) %25;
	}
	
	//    Uart ready to transmit & there is something to transmit (head and tail pointers =/=)
	if (!TX_in_progress && (storage_txbuffer != stage_txbuffer))
	{
		//    Get length of message
		int length = strlen(Buffer[stage_txbuffer]);
		
		//    Set transmitting flag
		TX_in_progress = true;
		
		//    Write message
		io_write(io,(uint8_t*)Buffer[stage_txbuffer],length);
		
		//    Increase head cursor of circular buffer
		stage_txbuffer = (stage_txbuffer + 1) %25;
	}
}

void serial_tx_callback(const struct usart_async_descriptor *const io_descr)
{
	TX_in_progress = false;
	send_uart("");
}

void uart_init()
{
	printf("register rx callback...\r");
	//	Setup RX callback
	usart_async_register_callback(&USART_1, USART_ASYNC_RXC_CB, serial_rx_callback);

	printf("register tx callback...\r");
	//	Setup TX callback
	usart_async_register_callback(&USART_1, USART_ASYNC_TXC_CB, serial_tx_callback);
	// 	usart_async_register_callback(&USART_1, USART_ASYNC_ERROR_CB, err_cb);

	//	Setup USART
	usart_async_get_io_descriptor(&USART_1, &io);
	printf("Enable uart1...\r");
	usart_async_enable(&USART_1);
}