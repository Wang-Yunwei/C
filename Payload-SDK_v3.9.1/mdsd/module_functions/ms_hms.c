/**
 * ********************************************************************
 * Created by WangYunwei [2024-10-15] HMS 功能
 * ********************************************************************
 */

/* Includes ------------------------------------------------------------------*/

#include "dji_logger.h"
#include "dji_typedef.h"
#include "dji_hms.h"
#include "dji_hms_info_table.h"

#include "ms_hms.h"

/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/

/**
 * 初始化 hms 模块
 *      注: 初始化 hms 模块, 用户应在使用 hms 功能之前调用此函数
 */
static void F_Init()
{
    T_DjiReturnCode returnCode = DjiHms_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("Hms 初始化错误, 错误码: 0x%08llX", returnCode);
    }
}

/**
 * 反初始化 hms 模块
 */
static void F_DeInit()
{
    T_DjiReturnCode returnCode = DjiHms_DeInit();
}

/**
 * 注册回调以获取 hms 信息
 *      注: 数据以 1Hz 的频率推送
 *
 * T_DjiHmsInfoTable
 *      T_DjiHmsInfo *hmsInfo;
 *          uint32_t errorCode;
 *          uint8_t componentIndex;
 *          uint8_t errorLevel;
 *      uint32_t hmsInfoNum;
 */
static T_DjiReturnCode F_HmsInfo_Callback(T_DjiHmsInfoTable hmsInfoTable)
{
}
static void F_RegHmsInfoCallback()
{
    T_DjiReturnCode returnCode = DjiHms_RegHmsInfoCallback(F_HmsInfo_Callback);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("注册回调函数到推送 HMS 信息失败, 错误码: 0x%08llX", returnCode);
    }
}

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode F_Hms()
{
}