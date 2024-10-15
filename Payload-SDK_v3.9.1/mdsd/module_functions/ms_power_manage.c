/**
 * ********************************************************************
 * Created by WangYunwei [2024-10-15] 电源管理
 * ********************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "ms_power_manage.h"

/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/
/**
 * 初始化电源管理模块
 *      注: 初始化电源管理模块，用户在使用电源管理功能前应调用该函数
 */
static T_DjiReturnCode F_Init()
{
    T_DjiReturnCode returnCode = DjiPowerManagement_Init();
}

/**
 * 在阻塞模式下从飞机施加高功率
 *      注: 在应用之前, 用户应该使用 DjiPowerManagement_RegWriteHighPowerApplyPinCallback() 函数注册回调函数, 用于设置高功率应用引脚的电平状态
 *          施加大功率后, DJI 适配器的电源引脚将根据预定规格输出大功率, 该函数的最大执行时间略大于 600ms
 */
static void F_ApplyHighPowerSync()
{
    T_DjiReturnCode returnCode = DjiPowerManagement_ApplyHighPowerSync();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("应用高功率错误");
    }
}

/**
 * 用于设置高功率应用引脚的电平状态的注册回调函数
 *      注: 注册回调函数, 用于设置高功率应用引脚的电平状态, 必须在应用 F_ApplyHighPowerSync() 高功率之前调用
 *
 * E_DjiPowerManagementPinState
 *      DJI_POWER_MANAGEMENT_PIN_STATE_RESET = 0,   // 引脚为低电压
 *      DJI_POWER_MANAGEMENT_PIN_STATE_SET = 1,     // 引脚为高电压
 */
static T_DjiReturnCode F_WriteHighPowerApplyPin_Callback(E_DjiPowerManagementPinState pinState)
{
}
static void F_RegWriteHighPowerApplyPinCallback()
{
    T_DjiReturnCode returnCode = DjiPowerManagement_RegWriteHighPowerApplyPinCallback(F_WriteHighPowerApplyPin_Callback);
}

/**
 * 用于通知payload飞机即将关机并获取payload是否准备好关机状态的注册回调函数
 *      注: 注册完成后会在飞行器关机时以固定频率调用回调函数, 用户填写关机准备标志一旦payload准备关机, 回调函数将不会被调用
 *          在指定时间后如果负载未准备好关机, 飞行器将立即关机指定时间为 10s
 *
 * 无人机下发关机通知的流程如下所示:
 *      1、当无人机获取到用户发送的关机通知时, 无人机将向使用 PSDK 开发的负载设备发送关机通知;
 *      2、当负载设备接收到无人机发送的关机通知后, 负载设备将完成关机前需执行的操作, 并更改关机准备状态;
 *      3、当无人机获取到所有负载设备关机准备的状态后, 将会立即关机;
 */
static T_DjiReturnCode F_PowerOffNotification_Callback(bool *powerOffPreparationFlag)
{
    // 为防止该函数阻塞 PSDK 的主线程, 导致出现程序响应缓慢、相机类负载设备断连及死循环等问题, 请勿以阻塞的方式在回调函数中执行该函数
    USER_LOG_INFO("飞行器即将断电!");
    *powerOffPreparationFlag = true;
    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}
static void F_RegPowerOffNotificationCallback()
{
    T_DjiReturnCode returnCode = DjiPowerManagement_RegPowerOffNotificationCallback(F_PowerOffNotification_Callback);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("注册掉电通知回调功能错误!");
    }
}

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode F_Power_Manage()
{
    T_DjiReturnCode returnCode = F_Init();
    return returnCode;
}