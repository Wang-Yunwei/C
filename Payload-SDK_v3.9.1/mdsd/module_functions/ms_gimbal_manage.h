/**
 * ********************************************************************
 * Created by WangYunwei [2024-10-14] 云台管理
 * ********************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MS_GIMBAL_MANAGE_H
#define MS_GIMBAL_MANAGE_H

/* Includes ------------------------------------------------------------------*/
#include "dji_logger.h"
#include "dji_typedef.h"
#include "dji_gimbal_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported types ------------------------------------------------------------*/
typedef struct
{

} T_MsCameraBasicManage;

/* Exported constants --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
T_DjiReturnCode F_Gimbal_Manage();

#ifdef __cplusplus
}
#endif

#endif
/* ************************ END OF FILE ************************ */