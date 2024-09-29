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

/**
 * 回调函数 - 四元数
 */
static T_DjiReturnCode F_Quaternion_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    // q0: w, q1: x, q2: y, q3: z
    T_DjiFcSubscriptionQuaternion *quaternion = (T_DjiFcSubscriptionQuaternion *)data;

    USER_UTIL_UNUSED(dataSize);

    // 俯仰: 横向轴X轴旋转
    dji_f64_t pitch = (dji_f64_t)asinf(-2 * quaternion->q1 * quaternion->q3 + 2 * quaternion->q0 * quaternion->q2) * 57.3;
    // 航向: 竖直轴Z轴旋转
    dji_f64_t roll = (dji_f64_t)atan2f(2 * quaternion->q2 * quaternion->q3 + 2 * quaternion->q0 * quaternion->q1, -2 * quaternion->q1 * quaternion->q1 - 2 * quaternion->q2 * quaternion->q2 + 1) * 57.3;
    // 翻滚: 纵向轴Y轴旋转
    dji_f64_t yaw = (dji_f64_t)atan2f(2 * quaternion->q1 * quaternion->q2 + 2 * quaternion->q0 * quaternion->q3, -2 * quaternion->q2 * quaternion->q2 - 2 * quaternion->q3 * quaternion->q3 + 1) * 57.3;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 相对地面加速度
 */
static T_DjiReturnCode F_AccelrationGround_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 相对机体加速度
 */
static T_DjiReturnCode F_AccelerationBody_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 原始加速度
 */
static T_DjiReturnCode F_AccelerationRaw_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 速度
 */
static T_DjiReturnCode F_Velocity_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 融合角速度
 */
static T_DjiReturnCode F_AngularRateFusioned_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 原始角速度
 */
static T_DjiReturnCode F_AngularRateRaw_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 融合高度
 */
static T_DjiReturnCode F_AltitudeFused_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 气压计高度
 */
static T_DjiReturnCode F_AltitudeBarometer_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - Home 点高度
 */
static T_DjiReturnCode F_AltitudeOfHomepoint_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 融合相对地面高度
 */
static T_DjiReturnCode F_HeightFusion_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 相对地面高度
 */
static T_DjiReturnCode F_HeightRelative_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 融合位置坐标
 */
static T_DjiReturnCode F_PositionFused_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - GPS 日期 (年月日)
 */
static T_DjiReturnCode F_GpsDate_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - GPS 时间 (时分秒)
 */
static T_DjiReturnCode F_GpsTime_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - GPS 位置
 */
static T_DjiReturnCode F_GpsPosition_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - GPS 速度
 */
static T_DjiReturnCode F_GpsVelocity_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - GPS 信息
 */
static T_DjiReturnCode F_GpsDetails_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - GPS 信号强度
 */
static T_DjiReturnCode F_GpsSignalLevel_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - RTK 位置
 */
static T_DjiReturnCode F_RtkPosition_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - RTK 速度
 */
static T_DjiReturnCode F_RtkVelocity_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - RTK 航向角
 */
static T_DjiReturnCode F_RtkYaw_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - RTK 位置信息
 */
static T_DjiReturnCode F_RtkPositionInfo_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - RTK 航向信息
 */
static T_DjiReturnCode F_RtkYawInfo_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 指南针信息
 */
static T_DjiReturnCode F_Compass_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 遥控摇杆信息
 */
static T_DjiReturnCode F_Rc_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 云台角度
 */
static T_DjiReturnCode F_GimbalAngles_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 云台状态
 */
static T_DjiReturnCode F_GimbalStatus_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 飞行状态
 */
static T_DjiReturnCode F_StatusFlight_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 飞行模式状态
 */
static T_DjiReturnCode F_StatusDisplaymode_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 起落架状态
 */
static T_DjiReturnCode F_StatusLandinggear_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 电机启动错误码
 */
static T_DjiReturnCode F_StatusMotorStartError_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 电池信息
 */
static T_DjiReturnCode F_BatteryInfo_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 设备控制信息
 */
static T_DjiReturnCode F_ControlDevice_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 硬件时钟同步
 */
static T_DjiReturnCode F_HardSync_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - GPS 控制等级
 */
static T_DjiReturnCode F_GpsControlLevel_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 带标记遥控遥感信息
 */
static T_DjiReturnCode F_RcWithFlagData_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 电调数据
 */
static T_DjiReturnCode F_EscData_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - RTK 连接状态
 */
static T_DjiReturnCode F_RtkConnectStatus_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 云台控制模式
 */
static T_DjiReturnCode F_GimbalControlMode_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 飞行异常信息
 */
static T_DjiReturnCode F_FlightAnomaly_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 笛卡尔坐标位置
 */
static T_DjiReturnCode F_PositionVo_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 避障数据
 */
static T_DjiReturnCode F_AvoidData_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 返航点设置状态
 */
static T_DjiReturnCode F_HomePointSetStatus_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 返航点信息
 */
static T_DjiReturnCode F_HomePointInfo_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 三云台信息
 */
static T_DjiReturnCode F_ThreeGimbalData_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 1 号电池信息
 */
static T_DjiReturnCode F_BatterySingleInfoIndex1_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 2 号电池信息
 */
static T_DjiReturnCode F_BatterySingleInfoIndex2_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private variables ---------------------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode F_FcSubscription()
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
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_QUATERNION, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_Quaternion_Callback);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("订阅 - 四元数_失败!");
    }
    else
    {
        USER_LOG_DEBUG("订阅 - 四元数_成功!");
    }

    // 订阅 - 相对地面加速度
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_ACCELERATION_GROUND, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_AccelrationGround_Callback);

    // 订阅 - 相对机体加速度
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_ACCELERATION_BODY, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_AccelerationBody_Callback);

    // 订阅 - 原始加速度
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_ACCELERATION_RAW, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_AccelerationRaw_Callback);

    // 订阅 - 速度
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_VELOCITY, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_Velocity_Callback);

    // 订阅 - 融合角速度
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_ANGULAR_RATE_FUSIONED, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_AngularRateFusioned_Callback);

    // 订阅 - 原始角速度
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_ANGULAR_RATE_RAW, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_AngularRateRaw_Callback);

    // 订阅 - 融合高度
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_ALTITUDE_FUSED, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_AltitudeFused_Callback);

    // 订阅 - 气压计高度
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_ALTITUDE_BAROMETER, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_AltitudeBarometer_Callback);

    // 订阅 - Home点高度
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_ALTITUDE_OF_HOMEPOINT, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_AltitudeOfHomepoint_Callback);

    // 订阅 - 融合相对地面高度
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_HEIGHT_FUSION, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_HeightFusion_Callback);

    // 订阅 - 相对地面高度
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_HEIGHT_RELATIVE, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_HeightRelative_Callback);

    // 订阅 - 融合位置坐标
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_POSITION_FUSED, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_HeightFusion_Callback);

    // 订阅 - GPS 日期 (年月日)
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_GPS_DATE, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_GpsDate_Callback);

    // 订阅 - GPS 时间 (时分秒)
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_GPS_TIME, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_GpsTime_Callback);

    // 订阅 - GPS 位置
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_GPS_POSITION, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_GpsPosition_Callback);

    // 订阅 - GPS 速度
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_GPS_VELOCITY, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_GpsVelocity_Callback);

    // 订阅 - GPS 信息
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_GPS_DETAILS, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_GpsDetails_Callback);

    // 订阅 - GPS 信号强度
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_GPS_SIGNAL_LEVEL, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_GpsSignalLevel_Callback);

    // 订阅 - RTK 位置
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_RTK_POSITION, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_RtkPosition_Callback);

    // 订阅 - RTK 速度
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_RTK_VELOCITY, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_RtkVelocity_Callback);

    // 订阅 - RTK 航向角
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_RTK_YAW, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_RtkYaw_Callback);

    // 订阅 - RTK 位置信息
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_RTK_POSITION_INFO, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_RtkPositionInfo_Callback);

    // 订阅 - RTK 航向信息
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_RTK_YAW_INFO, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_RtkYawInfo_Callback);

    // 订阅 - 指南针信息
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_COMPASS, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_Compass_Callback);

    // 订阅 - 遥控摇杆信息
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_RC, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_Rc_Callback);

    // 订阅 - 云台角度
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_GIMBAL_ANGLES, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_GimbalAngles_Callback);

    // 订阅 - 云台状态
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_GIMBAL_STATUS, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_GimbalStatus_Callback);

    // 订阅 - 飞行状态
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_STATUS_FLIGHT, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_StatusFlight_Callback);

    // 订阅 - 飞行模式状态
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_STATUS_DISPLAYMODE, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_StatusDisplaymode_Callback);

    // 订阅 - 起落架状态
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_STATUS_LANDINGGEAR, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_StatusLandinggear_Callback);

    // 订阅 - 电机启动错误码
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_STATUS_MOTOR_START_ERROR, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_StatusMotorStartError_Callback);

    // 订阅 - 电池信息
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_BATTERY_INFO, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_BatteryInfo_Callback);

    // 订阅 - 设备控制信息
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_CONTROL_DEVICE, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_ControlDevice_Callback);

    // 订阅 - 硬件时钟同步
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_HARD_SYNC, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_HardSync_Callback);

    // 订阅 - GPS 控制等级
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_GPS_CONTROL_LEVEL, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_GpsControlLevel_Callback);

    // 订阅 - 带标记遥控遥感信息
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_RC_WITH_FLAG_DATA, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_RcWithFlagData_Callback);

    // 订阅 - 电调数据
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_ESC_DATA, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_EscData_Callback);

    // 订阅 - RTK 连接状态
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_RTK_CONNECT_STATUS, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_RtkConnectStatus_Callback);

    // 订阅 - 云台控制模式
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_GIMBAL_CONTROL_MODE, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_GimbalControlMode_Callback);

    // 订阅 - 飞行异常信息
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_FLIGHT_ANOMALY, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_FlightAnomaly_Callback);

    // 订阅 - 笛卡尔坐标位置
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_POSITION_VO, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_PositionVo_Callback);

    // 订阅 - 避障数据
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_AVOID_DATA, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_AvoidData_Callback);

    // 订阅 - 返航点设置状态
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_HOME_POINT_SET_STATUS, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_HomePointSetStatus_Callback);

    // 订阅 - 返航点信息
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_HOME_POINT_INFO, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_HomePointInfo_Callback);

    // 订阅 - 三云台信息 (适用 M300 RTK 与 M350 RTK, 上下三个云台的信息)
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_THREE_GIMBAL_DATA, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_ThreeGimbalData_Callback);

    // 订阅 - 1 号电池信息
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_BATTERY_SINGLE_INFO_INDEX1, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_BatterySingleInfoIndex1_Callback);

    // 订阅 - 2 号电池信息
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_BATTERY_SINGLE_INFO_INDEX2, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_BatterySingleInfoIndex2_Callback);
}

