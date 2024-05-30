#ifndef CONF_CAN_H_INCLUDED
#define CONF_CAN_H_INCLUDED

/*
 * Below is the message RAM setting, it will be stored in the system RAM.
 * Please adjust the message size according to your application.
 */
#define CONF_CAN0_RX_BUFFER_NUM					32	/* Range: 1..64 */ 
#define CONF_CAN0_TX_BUFFER_NUM					1	/* Range: 1..16 */
#define CONF_CAN0_TX_FIFO_QUEUE_NUM				1	/* Range: 1..16 */
#define CONF_CAN0_TX_EVENT_FIFO					1	/* Range: 1..32 */
#define CONF_CAN0_RX_STANDARD_ID_FILTER_NUM     32	/* Range: 1..128 */

/* The value should be 8/12/16/20/24/32/48/64. */
#define CONF_CAN_ELEMENT_DATA_SIZE         64

// unused stuff
#define CONF_CAN0_RX_FIFO_0_NUM				1
#define CONF_CAN0_RX_FIFO_1_NUM				1
#define CONF_CAN0_RX_EXTENDED_ID_FILTER_NUM	1
#define CONF_CAN1_RX_FIFO_0_NUM				1
#define CONF_CAN1_RX_FIFO_1_NUM				1
#define CONF_CAN1_RX_BUFFER_NUM				1	/* Range: 1..64 */
#define CONF_CAN1_TX_BUFFER_NUM				1	/* Range: 1..16 */
#define CONF_CAN1_TX_FIFO_QUEUE_NUM			1	/* Range: 1..16 */
#define CONF_CAN1_TX_EVENT_FIFO				1	/* Range: 1..32 */
#define CONF_CAN1_RX_STANDARD_ID_FILTER_NUM	1	/* Range: 1..128 */
#define CONF_CAN1_RX_EXTENDED_ID_FILTER_NUM	1

/*
 * The setting of the nominal bit rate is based on the GCLK_CAN is 48M which you can
 * change in the conf_clock.h. Below is the default configuration. The
 * time quanta is 96MHz / (11+1) =  8MHz. And each bit is (1 + NTSEG1 + 1 + NTSEG2 + 1) = 16 time
 * quanta which means the bit rate is 8MHz/16=500KHz.
 */
// Tested with PCAN-USB FD at:
// 48MHz, 500k, P:5  SEG1:10 SEG2:3
// 80MHz, 500k, P:9  SEG1:10 SEG2:3
// 96MHz, 500k, P:11 SEG1:10 SEG2:3
// 80MHz, 1000k, P:3 SEG1:13 SEG2:4

/* Nominal bit Baud Rate Prescaler */
#define CONF_CAN_NBTP_NBRP_VALUE    3
/* Nominal bit (Re)Synchronization Jump Width */
#define CONF_CAN_NBTP_NSJW_VALUE    3
/* Nominal bit Time segment before sample point */
#define CONF_CAN_NBTP_NTSEG1_VALUE  13
/* Nominal bit Time segment after sample point */
#define CONF_CAN_NBTP_NTSEG2_VALUE  4

/*
 * The setting of the data bit rate is based on the GCLK_CAN is 48M which you can
 * change in the conf_clock.h. Below is the default configuration. The
 * time quanta is 96MHz / (0+1) =  96MHz. And each bit is (1 + DTSEG1 + 1 + DTSEG2 + 1) = 16 time
 * quanta which means the bit rate is 96MHz/16=10,000KHz.
 */
// Tested with PCAN-USB FD at:
// 48MHz, 1000k, P:2 SEG1:10 SEG2:3
// 80MHz, 1000k, P:4 SEG1:10 SEG2:3
// 96MHz, 1000k, P:5 SEG1:10 SEG2:3
// 80MHz, 4000k, P:1 SEG1:6  SEG2:1

/* Data bit Baud Rate Prescaler */
#define CONF_CAN_DBTP_DBRP_VALUE    1
/* Data bit (Re)Synchronization Jump Width */
#define CONF_CAN_DBTP_DSJW_VALUE    3
/* Data bit Time segment before sample point */
#define CONF_CAN_DBTP_DTSEG1_VALUE  6
/* Data bit Time segment after sample point */
#define CONF_CAN_DBTP_DTSEG2_VALUE  1

#endif
