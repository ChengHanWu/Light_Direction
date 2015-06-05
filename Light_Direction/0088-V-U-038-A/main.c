//test

/**
  ******************************************************************************
  * @file    Project/STM32F0xx_StdPeriph_Templates/main.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "main.h"
#include "autonetAPIs.h"
#include "us2400APIs.h"
#include "TxRx.h" 
#include "st.h"
#include "stm32f0xx_it.h"
#include "mpu6050APIs.h"
#include "ak8975APIs.h"
#include "lea6sAPIs.h"
#include "tmp75APIs.h"
#include "mcp2120APIs.h"
#include "bh1750fviAPIs.h"
#include "mag3110APIs.h"


/** @addtogroup STM32F0xx_StdPeriph_Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define Direction 	0
#define Velocity 		1
#define LOS 				2
#define SRC_ADDR 		0x0001

#define UNIT_MS 0
#define UNIT_SEC 1
#define UNIT_MIN 2

#define _gateway_ID 0x0000
#define RX_OFFSET 12
#define MAC_HEADER_LENGTH 12
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t _radio_freq;
uint16_t _radio_panID;
uint16_t _src_ID;

extern uint8_t pTxData[];
extern uint8_t pRxData[];
extern int framelength;
extern TimObjTypeDef_s TimObj;
extern uint8_t Data[];
extern uint8_t DataLen;

/* readable sensors' data */
int Speed_m_s;
int heading_deg;   // 0~360 degrees (north: 0 degree)
char Front_ID = 0xFF;
char Rear_ID = 0xFF;

enum{
	Type_Controller,
	Type_Light,
};

enum{
	Message_Control,
	Message_Type,
};

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */

int main(void)
{
	uint8_t Device_Type;
	uint8_t i;
	uint8_t light_flag = 0;
	uint8_t TxData[256];  
	uint8_t RxData[256];
	uint8_t Rx_Payload[256];
	uint8_t Lighting_Flag=0;
	
	uint8_t Rx_DataLen;
	uint16_t ID_List[10];
	int heading;
	uint8_t heading_diff=20;
	uint8_t Match_Devices_Num;
	
	Device_Type = Type_Light;
	_src_ID = 0x0001;
	_radio_freq = 2475;
  	_radio_panID = 0x00AA;
	Autonet_Initialization(_radio_freq, _radio_panID, _src_ID);
	
  // setting the period of broadcasting information of AutoNet	
	Timer_Beacon(200);	
  // every 200ms to pull up the time flag
	setTimer(1, 500, UNIT_MS);

	while(1){
		RF_beacon();  // broadcast beacon information
		if(Device_Type == Type_Controller){
			get_direction(&heading);
			if(checkTimer(1)){
				Match_Devices_Num = Group_Diff(ID_List,Direction,heading,heading_diff);
				TxData[0] = Message_Control;
				TxData[1] = Match_Devices_Num;
				for( i=0 ; i<Match_Devices_Num ; i++ ){
					TxData[2*i+2] = ID_List[0];
					TxData[2*i+3] = ID_List[0] >> 8;
				}
				RF_Tx(0xFFFF,TxData,Match_Devices_Num*2+2);
			}
		}
		else if(Device_Type == Type_Light){
			if(RF_Rx(RxData,&Rx_DataLen)){
			  Lighting_Flag = 0;
				getPayload(Rx_Payload,RxData,Rx_DataLen);
				for( i=0 ; i<Rx_Payload[1]; i++ ){
					if((Rx_Payload[2*i+2] | Rx_Payload[2*i+3] << 8) == _src_ID){
						Lighting_Flag = 1;
						break;
					}
				}
				if(Lighting_Flag == 1)
					PIN_ON(1);
				else
					PIN_OFF(1);
			}
		}
	/*
		// The device has received sth.
		if(RF_Rx(RxData, &dataLen)==1){   
			// frame to check that the state of devices is a master or a slave 
			if(RxData[0+MAC_HEADER_LENGTH] == 0x01){    
				master_ADDR = RxData[1+MAC_HEADER_LENGTH] | RxData[2+MAC_HEADER_LENGTH] << 8;
				if(SRC_ADDR == master_ADDR)
					state = master;   // master
				else
					state = slave;    // slave 
			}
		
			// frame to check whether to light up LEDs or not
			if(RxData[0+MAC_HEADER_LENGTH] == 0x02){
				light_flag = 0;
				
				for(i=0; i<RxData[1+MAC_HEADER_LENGTH]; i++) {
					// the address of the device shows in the lighting frame
					tmp_ADDR = RxData[2*i+2+MAC_HEADER_LENGTH] | RxData[2*i+3+MAC_HEADER_LENGTH] << 8;
					if(SRC_ADDR == tmp_ADDR) {
						light_flag = 1;
						break;
					}
				}
				
				if(light_flag ==1)
					PIN_ON(1);    // turn on the PIN
				else
					PIN_OFF(1);
			}
		}

		if(checkTimer(1)){    // the time flag has been pulled up
			if(state == master){  // the device utiling grouping APIs
				get_direction(&heading);
				get_velocity(&speed);
				
				Num = Group_Diff(addr_array, attribute, center, difference);
				//Num1 = AutoNet_Group(ID_1, Direction, heading, 30); 
				//Num2 = AutoNet_Group(ID_2, Velocity, speed, 1);
				//Num3 = AutoNet_Group(ID_3, LOS, 0xFF, 0);
				// ID = intersection of ID_1, ID_2, ID_3
				
				// Constructure a lighting frame to broadcast the results
				TxData[0] = 0x02;    // lighting message type
				TxData[1] = Num;     // number of result IDs
				
				for(i = 0; i < addr_array[0];i++){
					TxData[2*i+2] = addr_array[i+1];
					TxData[2*i+3] = addr_array[i+1] >> 8;
				}
				txLength = Num*2 + 2;   // results plus type and number of the results
				RF_Tx(0xFFFF, TxData, txLength);
			}
		}
		*/
	}
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
