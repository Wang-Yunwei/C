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
    USER_UTIL_UNUSED(dataSize);

    /**
     * dji_f32_t q0;           w,使用弧度制 (转换为旋转矩阵或欧拉角)
     * dji_f32_t q1;           x,使用弧度制 (转换为旋转矩阵或欧拉角)
     * dji_f32_t q2;           y,使用弧度制 (转换为旋转矩阵或欧拉角)
     * dji_f32_t q3;           z,使用弧度制 (转换为旋转矩阵或欧拉角)
     */
    T_DjiFcSubscriptionQuaternion *quaternion = (T_DjiFcSubscriptionQuaternion *)data;
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
    USER_UTIL_UNUSED(dataSize);

    /**
     * dji_f32_t x; 为向量指定 x 的浮点值
     * dji_f32_t y; 为向量指定 y 的浮点值
     * dji_f32_t z; 为向量指定 z 的浮点值
     */
    T_DjiFcSubscriptionAccelerationGround *accelerationGround = (T_DjiFcSubscriptionAccelerationGround *)data;
    dji_f32_t x = accelerationGround->x;
    dji_f32_t y = accelerationGround->y;
    dji_f32_t z = accelerationGround->z;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 相对机体加速度
 */
static T_DjiReturnCode F_AccelerationBody_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    /**
     * dji_f32_t x; 为向量指定 x 的浮点值
     * dji_f32_t y; 为向量指定 y 的浮点值
     * dji_f32_t z; 为向量指定 z 的浮点值
     */
    T_DjiFcSubscriptionAccelerationBody *accelerationBody = (T_DjiFcSubscriptionAccelerationBody *)data;
    dji_f32_t x = accelerationBody->x;
    dji_f32_t y = accelerationBody->y;
    dji_f32_t z = accelerationBody->z;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 原始加速度
 */
static T_DjiReturnCode F_AccelerationRaw_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    /**
     * dji_f32_t x; 为向量指定 x 的浮点值
     * dji_f32_t y; 为向量指定 y 的浮点值
     * dji_f32_t z; 为向量指定 z 的浮点值
     */
    T_DjiFcSubscriptionAccelerationRaw *accelerationRaw = (T_DjiFcSubscriptionAccelerationRaw *)data;
    dji_f32_t x = accelerationRaw->x;
    dji_f32_t y = accelerationRaw->y;
    dji_f32_t z = accelerationRaw->z;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 速度
 */
static T_DjiReturnCode F_Velocity_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    /**
     * dji_f32_t x; 为向量指定 x 的浮点值
     * dji_f32_t y; 为向量指定 y 的浮点值
     * dji_f32_t z; 为向量指定 z 的浮点值
     */
    T_DjiFcSubscriptionVelocity *velocity = (T_DjiFcSubscriptionVelocity *)data;
    dji_f32_t x = velocity->data.x;
    dji_f32_t y = velocity->data.y;
    dji_f32_t z = velocity->data.z;
    uint8_t health = velocity->health; // 0: 订阅的数据是健康的, 可以使用; 1: 订阅的数据不健康, 建议不要使用

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 融合角速度
 */
static T_DjiReturnCode F_AngularRateFusioned_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    /**
     * dji_f32_t x; 为向量指定 x 的浮点值
     * dji_f32_t y; 为向量指定 y 的浮点值
     * dji_f32_t z; 为向量指定 z 的浮点值
     */
    T_DjiFcSubscriptionAngularRateFusioned *angularRateFusioned = (T_DjiFcSubscriptionAngularRateFusioned *)data;
    dji_f32_t x = angularRateFusioned->x;
    dji_f32_t y = angularRateFusioned->y;
    dji_f32_t z = angularRateFusioned->z;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 原始角速度
 */
static T_DjiReturnCode F_AngularRateRaw_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    /**
     * dji_f32_t x; 为向量指定 x 的浮点值
     * dji_f32_t y; 为向量指定 y 的浮点值
     * dji_f32_t z; 为向量指定 z 的浮点值
     */
    T_DjiFcSubscriptionAngularRateRaw *angularRateRaw = (T_DjiFcSubscriptionAngularRateRaw *)data;
    dji_f32_t x = angularRateRaw->x;
    dji_f32_t y = angularRateRaw->y;
    dji_f32_t z = angularRateRaw->z;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 融合高度
 */
static T_DjiReturnCode F_AltitudeFused_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionAltitudeFused *altitudeFused = (T_DjiFcSubscriptionAltitudeFused *)data; // dji_f32_t; 单精度浮点类型

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 气压计高度
 */
static T_DjiReturnCode F_AltitudeBarometer_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionAltitudeBarometer *altitudeBarometer = (T_DjiFcSubscriptionAltitudeBarometer *)data; // dji_f32_t; 单精度浮点类型

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - Home 点高度
 */
static T_DjiReturnCode F_AltitudeOfHomepoint_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionAltitudeOfHomePoint *altitudeOfHomePoint = (T_DjiFcSubscriptionAltitudeOfHomePoint *)data; // dji_f32_t; 单精度浮点类型

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 融合相对地面高度
 */
static T_DjiReturnCode F_HeightFusion_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionHeightFusion *heightFusion = (T_DjiFcSubscriptionHeightFusion *)data; // dji_f32_t; 单精度浮点类型

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 相对地面高度
 */
static T_DjiReturnCode F_HeightRelative_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionHeightRelative *heightRelative = (T_DjiFcSubscriptionHeightRelative *)data; // dji_f32_t; 单精度浮点类型, 单位: m

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 融合位置坐标
 */
static T_DjiReturnCode F_PositionFused_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionPositionFused *positionFused = (T_DjiFcSubscriptionPositionFused *)data;
    dji_f64_t longitude = positionFused->longitude;                          // 经度, 单位：度
    dji_f64_t latitude = positionFused->latitude;                            // 纬度, 单位：度
    dji_f32_t altitude = positionFused->altitude;                            // 高度, 单位：米
    uint16_t visibleSatelliteNumber = positionFused->visibleSatelliteNumber; // 可见卫星数量

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - GPS 日期 (年月日)
 */
static T_DjiReturnCode F_GpsDate_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionGpsDate *gpsDate = (T_DjiFcSubscriptionGpsDate *)data; // format: yyyymmdd

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - GPS 时间 (时分秒)
 */
static T_DjiReturnCode F_GpsTime_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionGpsTime *gpsTime = (T_DjiFcSubscriptionGpsTime *)data; // format: hhmmss

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - GPS 位置
 */
static T_DjiReturnCode F_GpsPosition_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    /**
     * int32_t x; 为向量指定 x 的 int32 值
     * int32_t y; 为向量指定 y 的 int32 值
     * int32_t z; 为向量指定 z 的 int32 值
     */
    T_DjiFcSubscriptionGpsPosition *gpsPosition = (T_DjiFcSubscriptionGpsPosition *)data;
    int32_t x = gpsPosition->x;
    int32_t y = gpsPosition->y;
    int32_t z = gpsPosition->z;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - GPS 速度
 */
static T_DjiReturnCode F_GpsVelocity_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    /**
     * dji_f32_t x; 为向量指定 x 的浮点值
     * dji_f32_t y; 为向量指定 y 的浮点值
     * dji_f32_t z; 为向量指定 z 的浮点值
     */
    T_DjiFcSubscriptionGpsVelocity *gpsVelocity = (T_DjiFcSubscriptionGpsVelocity *)data;
    dji_f32_t x = gpsVelocity->x;
    dji_f32_t y = gpsVelocity->y;
    dji_f32_t z = gpsVelocity->z;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - GPS 信息
 */
static T_DjiReturnCode F_GpsDetails_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionGpsDetails *gpsDetails = (T_DjiFcSubscriptionGpsDetails *)data;
    // 水平稀释精密度, 单位:0.01, 例如: 100 = 1.00, <1:理想, 1-2:优秀, 2-5:良好, 5-10:一般, 10-20:一般, >20:差
    dji_f32_t hdop = gpsDetails->hdop;
    // 定位稀释精度, 单位:0.01, 例如: 100 = 1.00, <1:理想, 1-2:优秀, 2-5:良好, 5-10:中等, 10-20:一般, >20:差
    dji_f32_t pdop = gpsDetails->pdop;
    // GPS定位状态 (0-GPS 未获取到定位信息, 1-GPS推算得到 GPS 位置, 2-包含纬度/经度（或X / Y）的水平位置已定位, 3-包含纬度/经度/海拔（或X / Y / Z）的水平和垂直位置已定位, 4-GPS推测计算位置, 5-仅时间信息有效)
    dji_f32_t fixState = gpsDetails->fixState;
    // 垂直定位精度(mm), 越小越好
    dji_f32_t vacc = gpsDetails->vacc;
    // 水平定位精度(mm), 越小越好
    dji_f32_t hacc = gpsDetails->hacc;
    // 速度精度(cm/s), 越小越好
    dji_f32_t sacc = gpsDetails->sacc;
    // 用于定位的 GPS 卫星数量
    uint32_t gpsSatelliteNumberUsed = gpsDetails->gpsSatelliteNumberUsed;
    // 用于定位的 GLONASS(全球导航卫星系统) 卫星数目
    uint32_t glonassSatelliteNumberUsed = gpsDetails->glonassSatelliteNumberUsed;
    // 用于定位的卫星总数
    uint16_t totalSatelliteNumberUsed = gpsDetails->totalSatelliteNumberUsed;

    uint16_t gpsCounter = gpsDetails->gpsCounter;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - GPS 信号强度
 */
static T_DjiReturnCode F_GpsSignalLevel_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    // 等级从0到5, 其中 0 是最差的 GPS 信号, 5 是最好的 GPS 信号
    T_DjiFcSubscriptionGpsSignalLevel *gpsSignalLevel = (T_DjiFcSubscriptionGpsSignalLevel *)data;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - RTK 位置
 */
static T_DjiReturnCode F_RtkPosition_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionRtkPosition *rtkPosition = (T_DjiFcSubscriptionRtkPosition *)data;
    dji_f64_t longitude = rtkPosition->longitude; // 经度, 单位: 度
    dji_f64_t latitude = rtkPosition->latitude;   // 纬度, 单位: 度
    dji_f32_t hfsl = rtkPosition->hfsl;           // 平均海平面以上高度, 单位: m

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - RTK 速度
 */
static T_DjiReturnCode F_RtkVelocity_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    /**
     * dji_f32_t x; 为向量指定 x 的浮点值
     * dji_f32_t y; 为向量指定 y 的浮点值
     * dji_f32_t z; 为向量指定 z 的浮点值
     */
    T_DjiFcSubscriptionRtkVelocity *rtkVelocity = (T_DjiFcSubscriptionRtkVelocity *)data;
    dji_f32_t x = rtkVelocity->x;
    dji_f32_t y = rtkVelocity->y;
    dji_f32_t z = rtkVelocity->z;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - RTK 航向角
 */
static T_DjiReturnCode F_RtkYaw_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionRtkYaw *rtkYaw = (T_DjiFcSubscriptionRtkYaw *)data; // int16_t; 单位: 度

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - RTK 位置信息
 */
static T_DjiReturnCode F_RtkPositionInfo_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    /**
     * 0-位置解数据不可用
     * 1-位置由FIX POSITION命令指定
     * 3-位置由FIX HEIGHT / AUTO命令指定
     * 8-速度由即时多普勒信息导出
     * 16-单点位置解
     * 17-伪距差分解
     * 18-SBAS定位
     * 19-由一个没有新观测值的卡尔曼滤波器输出
     * 20-OmniSTAR VBS 位置 (L1 sub-metre)
     * 32-L1浮点解
     * 33-消电离层浮点解
     * 34-窄巷浮点解
     * 48-L1固定解
     * 49-宽巷固定解
     * 50-窄巷固定解
     */
    T_DjiFcSubscriptionRtkPositionInfo *rtkPositionInfo = (T_DjiFcSubscriptionRtkPositionInfo *)data;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - RTK 航向信息
 */
static T_DjiReturnCode F_RtkYawInfo_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    /**
     * 0-位置解数据不可用
     * 1-位置由FIX POSITION命令指定
     * 3-位置由FIX HEIGHT / AUTO命令指定
     * 8-速度由即时多普勒信息导出
     * 16-单点位置解
     * 17-伪距差分解
     * 18-SBAS定位
     * 19-由一个没有新观测值的卡尔曼滤波器输出
     * 20-OmniSTAR VBS 位置 (L1 sub-metre)
     * 32-L1浮点解
     * 33-消电离层浮点解
     * 34-窄巷浮点解
     * 48-L1固定解
     * 49-宽巷固定解
     * 50-窄巷固定解
     */
    T_DjiFcSubscriptionRtkYawInfo *rtkYawInfo = (T_DjiFcSubscriptionRtkYawInfo *)data;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 指南针信息
 */
static T_DjiReturnCode F_Compass_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionCompass *compass = (T_DjiFcSubscriptionCompass *)data;
    int16_t x = compass->x;
    int16_t y = compass->y;
    int16_t z = compass->z;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 遥控摇杆信息
 */
static T_DjiReturnCode F_Rc_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionRC *rc = (T_DjiFcSubscriptionRC *)data;
    int16_t roll = rc->roll;         // 翻滚角: -10000~10000
    int16_t pitch = rc->pitch;       // 俯仰角 -10000~10000
    int16_t yaw = rc->yaw;           // 偏航角: -10000~10000
    int16_t throttle = rc->throttle; // 功率: -10000~10000
    int16_t mode = rc->mode;         // 模式开关(P/A/F): -10000~10000, M100 [P: -8000, A: 0, F: 8000]
    int16_t gear = rc->gear;         // 起落架开关(上/下): -10000~10000, M100 [Up: -10000, Down: -4545]

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 云台角度
 */
static T_DjiReturnCode F_GimbalAngles_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    /**
     * dji_f32_t x; 为向量指定 x 的浮点值
     * dji_f32_t y; 为向量指定 y 的浮点值
     * dji_f32_t z; 为向量指定 z 的浮点值
     */
    T_DjiFcSubscriptionGimbalAngles *gimbalAngles = (T_DjiFcSubscriptionGimbalAngles *)data;
    dji_f32_t x = gimbalAngles->x;
    dji_f32_t y = gimbalAngles->y;
    dji_f32_t z = gimbalAngles->z;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 云台状态
 */
static T_DjiReturnCode F_GimbalStatus_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionGimbalStatus *gimbalStatus = (T_DjiFcSubscriptionGimbalStatus *)data;

    uint32_t mountStatus = gimbalStatus->mountStatus; // 1 - 云台安装, 0 - 云台未安装
    uint32_t isBusy = gimbalStatus->isBusy;
    uint32_t pitchLimited = gimbalStatus->pitchLimited;                     // 1 - X轴达极限, 0 - 未达
    uint32_t rollLimited = gimbalStatus->rollLimited;                       // 1 - Y轴达极限, 0 - 未达
    uint32_t yawLimited = gimbalStatus->yawLimited;                         // 1 - Z轴达极限, 0 - 未达
    uint32_t calibrating = gimbalStatus->calibrating;                       // 1 - 校准中, 0 - 未校准
    uint32_t prevCalibrationgResult = gimbalStatus->prevCalibrationgResult; // 1 - 成功, 0 - 失败
    uint32_t installedDirection = gimbalStatus->installedDirection;         // 1 - 为OSMO反转, 0 - 普通
    uint32_t disabled_mvo = gimbalStatus->disabled_mvo;
    uint32_t gear_show_unable = gimbalStatus->gear_show_unable;
    uint32_t gyroFalut = gimbalStatus->gyroFalut;           // 1 - 错误, 0 - 正常
    uint32_t escPitchStatus = gimbalStatus->escPitchStatus; // 1 - Pitch数据是正常的, 0 - 错误
    uint32_t escRollStatus = gimbalStatus->escRollStatus;   // 1 - Roll数据是正常的, 0 - 错误
    uint32_t escYawStatus = gimbalStatus->escYawStatus;     // 1 - Yaw数据是正常的, 0 - 错误
    uint32_t droneDataRecv = gimbalStatus->droneDataRecv;   // 1 - 正常 , 0 - 错误
    uint32_t initUnfinished = gimbalStatus->initUnfinished; // 1 - 初始化完成, 0 - 未完成
    uint32_t FWUpdating = gimbalStatus->FWUpdating;         // 1 - 更新中, 0 - 未更新
    uint32_t reserved2 = gimbalStatus->reserved2;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 飞行状态
 */
static T_DjiReturnCode F_StatusFlight_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    /**
     * 0-无人机在地面上且电机静止
     * 1-无人机在地面上, 但电机仍在转动
     * 2-无人机在空中
     */
    T_DjiFcSubscriptionFlightStatus *statusFlight = (T_DjiFcSubscriptionFlightStatus *)data;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 飞行模式状态
 */
static T_DjiReturnCode F_StatusDisplaymode_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    /**
     * 此模式需要用户手动控制飞行器在空中保持稳定
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_MANUAL_CTRL = 0
     *
     * 此模式, 飞行器可以保持姿态稳定并仅使用气压计定位来控制姿态, 飞行器不能自动定位和稳定悬停
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_ATTITUDE = 1
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_2 = 2
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_3 = 3
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_4 = 4
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_5 = 5
     *
     * 飞行器在普通 GPS 模式, 在此模式, 飞行器可以自动定位并稳定悬停, 飞行器对于命令的反应速度中等
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_P_GPS = 6
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_7 = 7
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_8 = 8
     *
     * 在热点模式
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_HOTPOINT_MODE = 9
     *
     * 在此模式, 用户可以推动油门杆以完成稳定降落
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_ASSISTED_TAKEOFF = 10
     *
     * 在此模式, 飞行器可以自动启动电机, 爬升并最终悬停
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_AUTO_TAKEOFF = 11
     *
     * 在此模式, 飞行器可以自动降落
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_AUTO_LANDING = 12
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_13 = 13
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_14 = 14
     *
     * 在此模式, 飞行器可以自动返回前一个返航点, 有三种模式: 智能返航、低电量返航、故障返航
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_NAVI_GO_HOME = 15
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_16 = 16
     *
     * 在此模式, 飞行器由SDK API控制, 用户可以直接定义水平方向和垂直方向的的控制模式并向飞行器发送数据
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_NAVI_SDK_CTRL = 17
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_18 = 18
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_19 = 19
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_20 = 20
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_21 = 21
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_22 = 22
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_23 = 23
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_24 = 24
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_25 = 25
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_26 = 26
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_27 = 27
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_28 = 28
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_29 = 29
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_30 = 30
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_31 = 31
     *      DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_32 = 32
     *
     * 飞行器迫降, 可能是由于低电量
     *       DJI_FC_SUBSCRIPTION_DISPLAY_MODE_FORCE_AUTO_LANDING = 33
     *       DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_34 = 34
     *       DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_35 = 35
     *       DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_36 = 36
     *       DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_37 = 37
     *       DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_38 = 38
     *       DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_39 = 39
     *
     * 飞行器会搜寻当遥控器没有断连的前一个位置
     *       DJI_FC_SUBSCRIPTION_DISPLAY_MODE_SEARCH_MODE = 40
     *
     * 点击启动的模式, 每当用户解锁电机, 这将为第一个模式
     *       DJI_FC_SUBSCRIPTION_DISPLAY_MODE_ENGINE_START = 41
     *       DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_42 = 42
     *       DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_43 = 42
     */
    T_DjiFcSubscriptionDisplaymode *statusDisplaymode = (T_DjiFcSubscriptionDisplaymode *)data;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 起落架状态
 */
static T_DjiReturnCode F_StatusLandinggear_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionLandinggear *statusLandinggear = (T_DjiFcSubscriptionLandinggear *)data;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 电机启动错误码
 */
static T_DjiReturnCode F_StatusMotorStartError_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionMotorStartError statusMotorStartError = (T_DjiFcSubscriptionMotorStartError *)data;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 电池信息
 */
static T_DjiReturnCode F_BatteryInfo_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionWholeBatteryInfo *batteryInfo = (T_DjiFcSubscriptionWholeBatteryInfo *)data;
    uint32_t capacity = batteryInfo->capacity;    // 电池容积, 单位: mAh
    uint32_t voltage = batteryInfo->voltage;      // 电池电压, 单位: mV
    uint32_t current = batteryInfo->current;      // 电池电流, 单位: mA
    uint8_t percentage = batteryInfo->percentage; // 电池容量百分比, 单位: 1%

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 设备控制信息
 */
static T_DjiReturnCode F_ControlDevice_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionControlDevice *controlDevice = (T_DjiFcSubscriptionControlDevice *)data;
    uint8_t controlMode = controlDevice->controlMode;   //
    uint8_t deviceStatus = controlDevice->deviceStatus; // 对于 M300 和 M210V2(固件版本V01.00.0690及以后): 0-rc 1-app 4-serial, 其他: 0-rc 1-app 2-serial
    uint8_t flightStatus = controlDevice->flightStatus; // 1-打开 0-关闭
    uint8_t vrcStatus = controlDevice->vrcStatus;
    uint8_t reserved = controlDevice->reserved;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 硬件时钟同步
 */
static T_DjiReturnCode F_HardSync_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionHardSync *hardSync = (T_DjiFcSubscriptionHardSync *)data;
    SyncTimestamp ts = hardSync->ts;   // 传入数据的时间戳
    struct Quaternion q = hardSync->q; // 四元数
    T_DjiVector3f a = hardSync->a;     // 加速度计读数单元: g
    T_DjiVector3f w = hardSync->w;     // 陀螺仪读数单元: rad/sec

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - GPS 控制等级
 */
static T_DjiReturnCode F_GpsControlLevel_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    // 级别从0~5, 0-是最差的, 5-是最好的GPS信号
    T_DjiFcSubscriptionGpsControlLevel *gpsControlLevel = (T_DjiFcSubscriptionGpsControlLevel *)data;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 带标记遥控遥感信息
 */
static T_DjiReturnCode F_RcWithFlagData_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionRCWithFlagData *rcWithFlagData = (T_DjiFcSubscriptionRCWithFlagData *)data;
    dji_f32_t pitch = rcWithFlagData->pitch;                        // 上 = -0.999, 中 = 0.000, 上   =0.999
    dji_f32_t roll = rcWithFlagData->roll;                          // 左 = -0.999, 中 = 0.000, 右   =0.999
    dji_f32_t yaw = rcWithFlagData->yaw;                            // 左 = -0.999, 中 = 0.000, 右   =0.999
    dji_f32_t throttle = rcWithFlagData->throttle;                  // 下 = -0.999, 中 = 0.000, 上   =0.999
    uint8_t logicConnected = rcWithFlagData->flag.logicConnected;   // 0 如果天空或地面侧断开 3 秒
    uint8_t skyConnected = rcWithFlagData->flag.skyConnected;       // 天空端连接，接收器与FC相连
    uint8_t groundConnected = rcWithFlagData->flag.groundConnected; // 地面段连接，RC开启并与FC相连
    uint8_t appConnected = rcWithFlagData->flag.appConnected;       // 移动设备App与RC相连

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 电调数据
 */
static T_DjiReturnCode F_EscData_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionEscData *escData = (T_DjiFcSubscriptionEscData *)data;
    int16_t current = escData->esc->current;                  // ESC 电流，单位： mA
    int16_t speed = escData->esc->speed;                      // ESC 速度，单位： rpm
    uint16_t voltage = escData->esc->voltage;                 // 从电池到ESC的输入电压，单位： mV
    int16_t temperature = escData->esc->temperature;          // ESC 温度，单位： degree C
    uint16_t stall = escData->esc->stall;                     // 电机堵转
    uint16_t empty = escData->esc->empty;                     // 电机无负载
    uint16_t unbalanced = escData->esc->unbalanced;           // 电机速度不平衡
    uint16_t escDisconnected = escData->esc->escDisconnected; // ESC 断连
    uint16_t temperatureHigh = escData->esc->temperatureHigh; // 温度较高

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - RTK 连接状态
 */
static T_DjiReturnCode F_RtkConnectStatus_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionRTKConnectStatus *rtkConnectStatus = (T_DjiFcSubscriptionRTKConnectStatus *)data;
    uint16_t rtkConnected = rtkConnectStatus->rtkConnected;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 云台控制模式
 */
static T_DjiReturnCode F_GimbalControlMode_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionGimbalControlMode *gimbalControlMode = (T_DjiFcSubscriptionGimbalControlMode *)data;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 飞行异常信息
 */
static T_DjiReturnCode F_FlightAnomaly_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionFlightAnomaly *flightAnomaly = (T_DjiFcSubscriptionFlightAnomaly *)data;

    uint32_t impactInAir = flightAnomaly->impactInAir;                             // 0-表示无碰撞, 1-表示在空气中发生碰撞
    uint32_t randomFly = flightAnomaly->randomFly;                                 // 0-正常, 1-在GPS模式下随机飞行,无需摇杆输入
    uint32_t heightCtrlFail = flightAnomaly->heightCtrlFail;                       // 0-高度控制正常, 1-高度控制失败
    uint32_t rollPitchCtrlFail = flightAnomaly->rollPitchCtrlFail;                 // 0-表示倾斜控制正常, 1-表示倾斜控制失败
    uint32_t yawCtrlFail = flightAnomaly->yawCtrlFail;                             // 0-偏航控制正常, 1-偏航控制失败
    uint32_t aircraftIsFalling = flightAnomaly->aircraftIsFalling;                 // 0-飞机未坠落, 1-飞机正在坠落
    uint32_t strongWindLevel1 = flightAnomaly->strongWindLevel1;                   // 0-风力低于1级强风, 1-风力大于1级强风
    uint32_t strongWindLevel2 = flightAnomaly->strongWindLevel2;                   // 0-风力低于2级, 1-风力大于2级
    uint32_t compassInstallationError = flightAnomaly->compassInstallationError;   // 0-指南针安装正确, 1-指南针安装错误
    uint32_t imuInstallationError = flightAnomaly->imuInstallationError;           // 0-IMU安装正确, 1-IMU安装错误
    uint32_t escTemperatureHigh = flightAnomaly->escTemperatureHigh;               // 0-ESC温度正常, 1-ESC温度高
    uint32_t atLeastOneEscDisconnected = flightAnomaly->atLeastOneEscDisconnected; // 0-ESC没有断开, 1-至少有一个ESC断开
    uint32_t gpsYawError = flightAnomaly->gpsYawError;                             // 0:无偏航误差, 1-偏航误差

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 笛卡尔坐标位置
 */
static T_DjiReturnCode F_PositionVo_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionPositionVO *positionVo = (T_DjiFcSubscriptionPositionVO *)data;
    dji_f32_t x = positionVo->x; // 北 (best effort)，单位： m
    dji_f32_t y = positionVo->y; // 东 (best effort)，单位： m
    dji_f32_t z = positionVo->z; // 下， 单位： m
    uint8_t xHealth = positionVo->xHealth;
    uint8_t yHealth = positionVo->yHealth;
    uint8_t zHealth = positionVo->zHealth;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 避障数据
 */
static T_DjiReturnCode F_AvoidData_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionAvoidData *avoidData = (T_DjiFcSubscriptionAvoidData *)data;
    dji_f32_t down = avoidData->down;             // 下-距障碍物的距离 (m)
    dji_f32_t front = avoidData->front;           // 前-距障碍物的距离 (m)
    dji_f32_t right = avoidData->right;           // 右-距障碍物的距离 (m)
    dji_f32_t back = avoidData->back;             // 后-距障碍物的距离 (m)
    dji_f32_t left = avoidData->left;             // 左-距障碍物的距离 (m)
    dji_f32_t up = avoidData->up;                 // 上-距障碍物的距离 (m)
    uint8_t downHealth = avoidData->downHealth;   // 下传感器标志:0 -不工作，1 -工作
    uint8_t frontHealth = avoidData->frontHealth; // 前传感器标志:0 -不工作，1 -工作
    uint8_t rightHealth = avoidData->rightHealth; // 右传感器标志:0 -不工作，1 -工作
    uint8_t backHealth = avoidData->backHealth;   // 后传感器标志:0 -不工作，1 -工作
    uint8_t leftHealth = avoidData->leftHealth;   // 左传感器标志:0 -不工作，1 -工作
    uint8_t upHealth = avoidData->upHealth;       // 上传感器健康标志:0 -不工作，1 -工作

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 返航点设置状态
 */
static T_DjiReturnCode F_HomePointSetStatus_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    // 0-未设置, 1-成功设置
    T_DjiFcSubscriptionHomePointSetStatus *homePointSetStatus = (T_DjiFcSubscriptionHomePointSetStatus *)data;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 返航点信息
 */
static T_DjiReturnCode F_HomePointInfo_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionHomePointInfo *homePointInfo = (T_DjiFcSubscriptionHomePointInfo *)data;
    dji_f64_t longitude = homePointInfo->longitude; // 经度
    dji_f64_t latitude = homePointInfo->latitude;   // 纬度

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 三云台信息
 */
static T_DjiReturnCode F_ThreeGimbalData_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionThreeGimbalData *threeGimbalData = (T_DjiFcSubscriptionThreeGimbalData *)data;
    dji_f32_t pitch = threeGimbalData->anglesData->pitch;
    dji_f32_t roll = threeGimbalData->anglesData->roll;
    dji_f32_t yaw = threeGimbalData->anglesData->yaw;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 1 号电池信息
 */
static T_DjiReturnCode F_BatterySingleInfoIndex1_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionSingleBatteryInfo *batterySingleInfoIndex1 = (T_DjiFcSubscriptionSingleBatteryInfo *)data;
    uint8_t batteryIndex = batterySingleInfoIndex1->batteryIndex;
    int32_t currentVoltage = batterySingleInfoIndex1->currentVoltage;         // 单位: mV
    int32_t currentElectric = batterySingleInfoIndex1->currentElectric;       // 单位: mA
    uint32_t fullCapacity = batterySingleInfoIndex1->fullCapacity;            // 单位: mAh
    uint32_t remainedCapacity = batterySingleInfoIndex1->remainedCapacity;    // 单位: mAh
    int16_t batteryTemperature = batterySingleInfoIndex1->batteryTemperature; // 单位: ℃
    uint8_t cellCount = batterySingleInfoIndex1->cellCount;
    uint8_t batteryCapacityPercent = batterySingleInfoIndex1->batteryCapacityPercent; // 单位: %
    T_DjiFcSubscriptionSingleBatteryState batteryState = batterySingleInfoIndex1->batteryState;
    uint8_t SOP = batterySingleInfoIndex1->SOP; // 相对功率百分比

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * 回调函数 - 2 号电池信息
 */
static T_DjiReturnCode F_BatterySingleInfoIndex2_Callback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{
    USER_UTIL_UNUSED(dataSize);

    T_DjiFcSubscriptionSingleBatteryInfo *batterySingleInfoIndex2 = (T_DjiFcSubscriptionSingleBatteryInfo *)data;
    uint8_t batteryIndex = batterySingleInfoIndex2->batteryIndex;
    int32_t currentVoltage = batterySingleInfoIndex2->currentVoltage;         // 单位: mV
    int32_t currentElectric = batterySingleInfoIndex2->currentElectric;       // 单位: mA
    uint32_t fullCapacity = batterySingleInfoIndex2->fullCapacity;            // 单位: mAh
    uint32_t remainedCapacity = batterySingleInfoIndex2->remainedCapacity;    // 单位: mAh
    int16_t batteryTemperature = batterySingleInfoIndex2->batteryTemperature; // 单位: ℃
    uint8_t cellCount = batterySingleInfoIndex2->cellCount;
    uint8_t batteryCapacityPercent = batterySingleInfoIndex2->batteryCapacityPercent; // 单位: %
    T_DjiFcSubscriptionSingleBatteryState batteryState = batterySingleInfoIndex2->batteryState;
    uint8_t SOP = batterySingleInfoIndex2->SOP; // 相对功率百分比

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
    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_POSITION_FUSED, DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, F_PositionFused_Callback);

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