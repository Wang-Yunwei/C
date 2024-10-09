/**
 * ********************************************************************
 * Created by WangYunwei [2024-10-09] 云台功能
 * ********************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MS_GIMBAL_FUNCTION_H
#define MS_GIMBAL_FUNCTION_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"
#include "dji_fc_subscription.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef struct
{

} T_FcSubscriptionData;

/* Exported functions --------------------------------------------------------*/
T_DjiReturnCode F_FcSubscription();

#ifdef __cplusplus
}
#endif

#endif
/* ************************ END OF FILE ************************ */