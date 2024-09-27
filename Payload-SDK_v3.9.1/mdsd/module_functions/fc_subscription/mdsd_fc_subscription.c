/**
 * ********************************************************************
 * Created by WangYunwei [2024-09-27]
 * ********************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "dji_logger.h"

#include "mdsd_fc_subscription.h"

/* Private constants ---------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/
static bool s_userFcSubscriptionDataShow = false;
static uint32_t s_userFcSubscriptionDataCnt = 0;

/* Private functions declaration ---------------------------------------------*/
static T_DjiReturnCode QuaternionCallback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp);

/* Private variables ---------------------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
void MDSD_FcSubscription()
{

    T_DjiReturnCode djiStat;

    // 消息订阅功能模块初始化
    djiStat = DjiFcSubscription_Init();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("初始化数据订阅模块错误!");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    // 订阅 - 四元数
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_QUATERNION, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ,
                                               QuaternionCallback);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("订阅 - 四元数_失败!");
    }
    else
    {
        USER_LOG_DEBUG("订阅 - 四元数_成功!");
    }

}

/**
 * @brief 四元数 - 回调函数
 * @param data: 数据指针
 * @param dataSize: 数据大小
 * @param timestamp: 时间戳
 * @return T_DjiReturnCode
 */
static T_DjiReturnCode QuaternionCallback(const uint8_t *data, uint16_t dataSize,
                                                                       const T_DjiDataTimestamp *timestamp)
{
    // q0: w, q1: x, q2: y, q3: z
    T_DjiFcSubscriptionQuaternion *quaternion = (T_DjiFcSubscriptionQuaternion *)data;
    dji_f64_t pitch, yaw, roll;// 俯仰: 横向轴X轴旋转, 航向: 竖直轴Z轴旋转, 翻滚: 纵向轴Y轴旋转

    USER_UTIL_UNUSED(dataSize);

    pitch = (dji_f64_t)asinf(-2 * quaternion->q1 * quaternion->q3 + 2 * quaternion->q0 * quaternion->q2) * 57.3;
    roll = (dji_f64_t)atan2f(2 * quaternion->q2 * quaternion->q3 + 2 * quaternion->q0 * quaternion->q1,
                             -2 * quaternion->q1 * quaternion->q1 - 2 * quaternion->q2 * quaternion->q2 + 1) *
           57.3;
    yaw = (dji_f64_t)atan2f(2 * quaternion->q1 * quaternion->q2 + 2 * quaternion->q0 * quaternion->q3,
                            -2 * quaternion->q2 * quaternion->q2 - 2 * quaternion->q3 * quaternion->q3 + 1) *
          57.3;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}