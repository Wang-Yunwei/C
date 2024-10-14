/**
 * ********************************************************************
 * Created by WangYunwei [2024-10-09] 基础相机管理
 * ********************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MS_CAMERA_MANAGE_H
#define MS_CAMERA_MANAGE_H

/* Includes ------------------------------------------------------------------*/
#include "dji_logger.h"
#include "dji_typedef.h"
#include "dji_camera_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported types ------------------------------------------------------------*/
typedef struct
{

} T_MsCameraBasicManage;

/* Exported constants --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
T_DjiReturnCode F_Camera_Manage();

#ifdef __cplusplus
}
#endif

#endif
/* ************************ END OF FILE ************************ */