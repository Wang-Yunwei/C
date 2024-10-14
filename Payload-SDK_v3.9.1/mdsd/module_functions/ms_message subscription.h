/**
 * ********************************************************************
 * Created by WangYunwei [2024-09-27] 数据订阅
 * ********************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MS_FC_SUBSCRIPTION_H
#define MS_FC_SUBSCRIPTION_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"
#include "dji_fc_subscription.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported types ------------------------------------------------------------*/
typedef struct
{

} T_MsDataSubscription;

/* Exported constants --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
T_DjiReturnCode F_Message_Subscription();

#ifdef __cplusplus
}
#endif

#endif
/* ************************ END OF FILE ************************ */