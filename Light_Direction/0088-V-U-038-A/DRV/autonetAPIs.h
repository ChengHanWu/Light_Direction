/******************** (C) COPYRIGHT 2015 NXG Lab ******************************
* File Name          : autonetAPI.h
* Author             : AutoNet team
* Version            : V1.0.0.0
* Date               : 08/04/2015
* Description        : Header for autonetAPIs
*******************************************************************************/
/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __AUTONET_API_H
#define __AUTONET_API_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "us2400APIs.h"
#include "st.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Autonet_Initialization(uint16_t,uint16_t,uint16_t);
void Autonet_search_type(char *a);
void Mag_Error_Handle (short *pX,short *pY,short *pZ, short *max_x, short *min_x, short *max_y, short *min_y, short *max_z, short *min_z);
void RF_beacon(void);
int Mag_flatsurface(short *pX,short *pY);
int getcompasscourse(short *ax,short *ay,short *az,short *cx,short *cy,short *cz);
void data_fetch(uint8_t* data_out, uint8_t* data_in, uint8_t d_offset, uint8_t d_length);
	 
/* utility APIs */
void get_direction(int *heading_deg);
void get_gps(void);
void get_LOS_address(char *f_id, char *r_id);
void update_sensor_table(void);

extern void lighting(uint8_t State);
void PIN_ON(uint8_t n);
void PIN_OFF(uint8_t n);
#ifdef __cplusplus
}
#endif

#endif /* __AUTONET_API_H */

