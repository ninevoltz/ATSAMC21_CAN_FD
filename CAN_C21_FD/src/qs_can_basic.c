#include <asf.h>
#include <string.h>
#include <conf_can.h>

static struct usart_module cdc_instance;
static struct can_module can_instance;

static uint8_t tx_message[CONF_CAN_ELEMENT_DATA_SIZE];

static volatile uint32_t standard_receive_index = 0;
static struct can_rx_element_buffer rx_element_buffer;

static void configure_usart_cdc(void) {

	struct usart_config config_cdc;
	usart_get_config_defaults(&config_cdc);
	config_cdc.baudrate	 = 115200;
	config_cdc.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
	config_cdc.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
	config_cdc.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
	config_cdc.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
	config_cdc.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;
	stdio_serial_init(&cdc_instance, EDBG_CDC_MODULE, &config_cdc);
	usart_enable(&cdc_instance);
}

static void configure_can(void) {
	
	uint32_t i;
	/* Initialize the memory. */
	for (i = 0; i < CONF_CAN_ELEMENT_DATA_SIZE; i++) {
		tx_message[i] = i;
	}

	/* Set up the CAN TX/RX pins */
	struct system_pinmux_config pin_config;
	system_pinmux_get_config_defaults(&pin_config);
	pin_config.mux_position = CAN_TX_MUX_SETTING;
	system_pinmux_pin_set_config(CAN_TX_PIN, &pin_config);
	pin_config.mux_position = CAN_RX_MUX_SETTING;
	system_pinmux_pin_set_config(CAN_RX_PIN, &pin_config);

	/* Initialize the module. */
	struct can_config config_can;
	can_get_config_defaults(&config_can);
	config_can.transmit_pause = false;
	config_can.edge_filtering = false;
	config_can.automatic_retransmission = true;
	can_init(&can_instance, CAN_MODULE, &config_can);
	can_enable_fd_mode(&can_instance);
	can_start(&can_instance);

	/* Enable interrupts for this CAN module */
	system_interrupt_enable(SYSTEM_INTERRUPT_MODULE_CAN0);
	can_enable_interrupt(&can_instance, CAN_PROTOCOL_ERROR_ARBITRATION | CAN_PROTOCOL_ERROR_DATA);
}

static void can_set_standard_filters(void) {
	
	struct can_standard_message_filter_element sd_filter;

	can_get_standard_message_filter_element_default(&sd_filter);
	sd_filter.S0.bit.SFID2 = 1; // buffer index
	sd_filter.S0.bit.SFID1 = 0x123; // filter ID
	sd_filter.S0.bit.SFEC = CAN_STANDARD_MESSAGE_FILTER_ELEMENT_S0_SFEC_STRXBUF_Val;

	can_set_rx_standard_filter(&can_instance, &sd_filter, 1); // filter index
	can_enable_interrupt(&can_instance, CAN_RX_BUFFER_NEW_MESSAGE);
}

static void can_send_standard_message(uint32_t id_value, uint8_t *data, uint32_t data_length) {
	
	uint32_t i;
	struct can_tx_element tx_element;

	can_get_tx_buffer_element_defaults(&tx_element);
	tx_element.T0.reg |= CAN_TX_ELEMENT_T0_STANDARD_ID(id_value);
	tx_element.T1.reg |= (CAN_TX_ELEMENT_T1_DLC(CAN_TX_ELEMENT_T1_DLC_DATA64_Val) | CAN_TX_ELEMENT_T1_FDF | CAN_TX_ELEMENT_T1_BRS);

	for (i = 0; i < data_length; i++) {
		tx_element.data[i] = *data;
		data++;
	}

	can_set_tx_buffer_element(&can_instance, &tx_element, 0);
	can_tx_transfer_request(&can_instance, 1 << 0);
}

uint8_t decode_can_message_dlc(uint8_t r1_dlc) {
	
	switch (r1_dlc) {
		case CAN_TX_ELEMENT_T1_DLC_DATA64_Val:
			return 64;
		break;
		case CAN_TX_ELEMENT_T1_DLC_DATA48_Val:
			return 48;
		break;
		case CAN_TX_ELEMENT_T1_DLC_DATA32_Val:
			return 32;
		break;
		case CAN_TX_ELEMENT_T1_DLC_DATA24_Val:
			return 24;
		break;
		case CAN_TX_ELEMENT_T1_DLC_DATA20_Val:
			return 20;
		break;
		case CAN_TX_ELEMENT_T1_DLC_DATA16_Val:
			return 16;
		break;
		case CAN_TX_ELEMENT_T1_DLC_DATA12_Val:
			return 12;
		break;
		case CAN_TX_ELEMENT_T1_DLC_DATA8_Val:
			return 8;
		break;
		default:
			return 0;
		break;	
	}
}

void CAN0_Handler(void) {
	
	volatile uint32_t status, i, rx_buffer_index;
	status = can_read_interrupt_status(&can_instance);
	uint8_t dlc = 0;
	
	if (status & CAN_RX_BUFFER_NEW_MESSAGE) {
		can_clear_interrupt_status(&can_instance, CAN_RX_BUFFER_NEW_MESSAGE);
		for (i = 0; i < CONF_CAN0_RX_BUFFER_NUM; i++) {
			if (can_rx_get_buffer_status(&can_instance, i)) {
				rx_buffer_index = i;
				can_rx_clear_buffer_status(&can_instance, i);
				can_get_rx_buffer_element(&can_instance, &rx_element_buffer, rx_buffer_index);
				dlc = decode_can_message_dlc(rx_element_buffer.R1.bit.DLC);
				printf("\n\r Message received in Rx buffer %d. Received %d bytes: \r\n", rx_buffer_index, dlc);
				for (i = 0; i < dlc; i++) {
					printf("  %d", rx_element_buffer.data[i]);
				}
				printf("\r\n\r\n");
			}
		}
	}

	if ((status & CAN_PROTOCOL_ERROR_ARBITRATION) || (status & CAN_PROTOCOL_ERROR_DATA)) {
		can_clear_interrupt_status(&can_instance, CAN_PROTOCOL_ERROR_ARBITRATION | CAN_PROTOCOL_ERROR_DATA);
		printf("Protocol error, please double check the clock in two boards. \r\n\r\n");
	}
}

#ifdef USE_USART_CDC
static void display_menu(void)
{
	printf("Menu :\r\n"
			"  -- Select the action:\r\n"
			"  1: Send standard message with ID: 0x45A \r\n"
			"  h: Display menu \r\n\r\n");
}
#endif

int main(void) {
	
	system_init();
	
#ifdef USE_USART_CDC
	uint8_t key;
	configure_usart_cdc();
	display_menu();
#endif

	configure_can();
	can_set_standard_filters();

	while(1) {
		
#ifdef USE_USART_CDC
		scanf("%c", (char *)&key);

		switch (key) {
		case 'h':
			display_menu();
			break;

		case '1':
			printf("  1: Send 64 byte message with ID: 0x45A \r\n");
			can_send_standard_message(0x45A, tx_message, CONF_CAN_ELEMENT_DATA_SIZE);
			break;

		default:
			break;
		}
#else
		can_send_standard_message(0x45A, tx_message, CONF_CAN_ELEMENT_DATA_SIZE);
#endif
	
	}

}
