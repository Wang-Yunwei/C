/**
 * ********************************************************************
 * Created by WangYunwei [2024-10-15] 时间同步
 * ********************************************************************
 */

/* Includes ------------------------------------------------------------------*/

#include "dji_logger.h"
#include "psdk_typedef.h"
#include "dji_time_sync.h"

#include "ms_time_sync.h"

/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/

/**
 * 初始化时间同步功能模块
 *      注: 在阻塞模式下初始化时间同步模块, 用户应在所有其他时间同步操作之前调用此函数, 就像传输时间一样
 *          - 该函数的最大执行时间略大于 2000ms
 *          - 该函数必须在用户任务中调用, 而不是 main() 函数, 并且在调度程序启动后调用
 */
static void F_Init()
{
    T_DjiReturnCode returnCode = DjiTimeSync_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("时间同步模块初始化错误!");
    }
}

/**
 * 获取最新的时间戳
 *      注: 注册回调函数, 用于在检测到 PPS 上升沿信号时获取本地时间系统中的最新时间戳
 */
T_PsdkReturnCode F_GetNewestPpsTriggerLocalTimeUs_Callback(uint64_t *localTimeUs)
{
}
static void F_RegGetNewestPpsTriggerTimeCallback()
{
    T_DjiReturnCode returnCode = DjiTimeSync_RegGetNewestPpsTriggerTimeCallback(F_GetNewestPpsTriggerLocalTimeUs_Callback);
}

/**
 * 时间转换
 *      注: 将负载设备的本地时间转换为无人机上的时间
 *          在使用本接口时, 请调用PsdkTimeSync_RegGetNewestPpsTriggerTimeCallback() 注册用于获取负载设备检测到PPS 信号上升沿时, 负载设备本地最新的时间戳的回调函数
 */
static void F_TransferToAircraftTime()
{
    uint64_t localTimeUs; // 负载设备中的本地时间, 单位: 微秒
    T_DjiTimeSyncAircraftTime aircraftTime; // 指向用于存储无人机系统时间的内存空间
    T_DjiReturnCode returnCode = DjiTimeSync_TransferToAircraftTime(localTimeUs, &aircraftTime);
}

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode F_Time_Sync()
{
}