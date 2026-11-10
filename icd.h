#ifndef ICD_H
#define ICD_H


#include <QObject>
#include <cstring>
#include <QHash>
#include <QMetaType>


#pragma pack(1)
typedef struct
{
    uint8_t reserve:3;
    uint8_t bit4:1;
    uint8_t bit5:1;
    uint8_t bit6:1;
    uint8_t reserve2:1;

} faultcode1;

typedef struct
{
    uint8_t reserve:3;
    uint8_t bit4:1;
    uint8_t bit5:1;
    uint8_t bit6:1;
    uint8_t bit7:1;
    uint8_t bit8:1;
} faultcode2;

typedef struct
{
    uint16_t left1fuel;   //左1油箱油量
    uint16_t left2fuel;   //左2油箱油量
    uint16_t right1fuel;     //右1油箱油量
    uint16_t right2fuel;     //右2油箱油量
    uint16_t leftfuel;     //左油箱油量
    uint16_t rightfuel;     //右邮箱油量
    uint16_t allfuel;    //总油量
    int8_t fuelheat;    //燃油温度
    uint8_t fuelheatcas;     //燃油高温告警
    uint16_t leftsendpress;     //左供油管压力
    uint16_t rigthsendpress;     //右供油管压力
    uint8_t leftsendlowpress;    //左供油管路低压告警
    uint8_t rightsendlowpress;     //右供油管路低压告警
    faultcode1 fault1;    //错误码1
    faultcode2 fault2;    //错误码2
    uint16_t left1fuelsensorsample;     //左1油箱油量传感器采样值
    uint16_t left2fuelsensorsample;     //左2油箱油量传感器采样值
    uint16_t right1fuelsensorsample;     //右1油箱油量传感器采样值
    uint16_t right2fuelsensorsample;     //右2油箱油量传感器采样值
    uint8_t reserve1; //预留
    uint16_t left1fuelsensorzero;     //左1油箱油量传感器零值
    uint16_t left1fuelsensorfull;     //左1油箱油量传感器满值
    uint16_t left2fuelsensorzero;    //左2油箱油量传感器零值
    uint16_t left2fuelsensorfull;     //左2油箱油量传感器满值
    uint16_t right1fuelsensorzero;     //右1油箱油量传感器零值
    uint16_t right1fuelsensorfull;     //右1油箱油量传感器满值
    uint16_t right2fuelsensorzero;     //右2油箱油量传感器零值
    uint16_t right2fuelsensorfull;     //右2油箱油量传感器满值
    int16_t fuelfuyang;     //油面俯仰角
    int16_t fuelhenggun;    //油面横滚角
    uint16_t temperature; //温度传感器采样值
    uint8_t softversion; //软件版本信息
    uint8_t allcheckfeedback;  //全机校准信息反馈
    uint8_t temperaturefeedback; //温度校准信息反馈
    uint8_t reserve2; //预留
    uint8_t reserve3; //预留
}fuelStruct;


// 接收报文数据区（20 字节）
typedef struct
{
    uint8_t  tempCalibration;      // 温度校准：0xAA 校准 / 0x00 不校准
    uint8_t  tempSensorSelect;     // 温度校准传感器选择：0x01 左油箱温度传感器
    uint8_t  tempGearSelect;       // 温度校准档位选择：0x01~0x06
    int16_t  pitchAngle;           // 飞机俯仰角，0.1°/LSB，-90~90
    int16_t  rollAngle;            // 飞机滚转角，0.1°/LSB，-180~180
    int8_t   axialLoad;            // 轴向过载，0.1g，-10~10
    int8_t   lateralLoad;          // 侧向过载，0.1g
    int8_t   normalLoad;           // 法向过载，0.1g
    uint8_t  wheelLoadSignal;      // 轮载信号：0x00 地面 / 0x01 空中
    int16_t  zeroCalibrationCmd;   // 零校准指令：0x100F
    int16_t  fullCalibrationCmd;   // 满校准指令：0x100F
    uint8_t  fuelType;             // 油品信息：0x01~0x06
    uint8_t  reserve1;             // 预留
    uint8_t  reserve2;             // 预留
    uint8_t  reserve3;             // 预留
    uint8_t  reserve4;             // 预留
} receiveParamters;

static_assert(sizeof(receiveParamters) == 20,
              "receiveParamters 必须为 20 字节，与接收报文数据区一致");
#pragma pack()


struct PacketField
{
    const char *name;
    std::size_t memberOffset;
    int byteCount;
    bool isSigned = false;
    double resolution = 1.0;
    double engineeringOffset = 0.0;
};

//offsetof用于获取某个成员距离结构体起始位置有多少字节
constexpr PacketField kFuelFields[]={
    {"左1油箱油量",offsetof(fuelStruct,left1fuel), 2},
    {"左2油箱油量",offsetof(fuelStruct, left2fuel), 2},
    {"右1油箱油量",offsetof(fuelStruct, right1fuel),2},
    {"右2油箱油量",offsetof(fuelStruct, right2fuel),2},
    {"左油箱油量",offsetof(fuelStruct, leftfuel),2},
    {"右邮箱油量",offsetof(fuelStruct, rightfuel),2},
    {"总油量",offsetof(fuelStruct, allfuel),2},
    {"燃油温度",offsetof(fuelStruct, fuelheat),1,true},
    {"燃油高温告警",offsetof(fuelStruct, fuelheatcas),1},
    {"左供油管压力",offsetof(fuelStruct, leftsendpress),2},
    {"右供油管压力",offsetof(fuelStruct, rigthsendpress),2},
    {"左供油管路低压告警",offsetof(fuelStruct, leftsendlowpress),1},
    {"右供油管路低压告警",offsetof(fuelStruct, rightsendlowpress),1},
    {"错误码1",offsetof(fuelStruct, fault1),1},
    {"错误码2",offsetof(fuelStruct, fault2),1},
    {"左1油箱油量传感器采样值",offsetof(fuelStruct, left1fuelsensorsample),2},
    {"左2油箱油量传感器采样值",offsetof(fuelStruct, left2fuelsensorsample),2},
    {"右1油箱油量传感器采样值",offsetof(fuelStruct, right1fuelsensorsample),2},
    {"右2油箱油量传感器采样值",offsetof(fuelStruct, right2fuelsensorsample),2},
    {"左1油箱油量传感器零值",offsetof(fuelStruct, left1fuelsensorzero),2},
    {"左1油箱油量传感器满值",offsetof(fuelStruct, left1fuelsensorfull),2},
    {"左2油箱油量传感器零值",offsetof(fuelStruct, left2fuelsensorzero),2},
    {"左2油箱油量传感器满值",offsetof(fuelStruct, left2fuelsensorfull),2},
    {"右1油箱油量传感器零值",offsetof(fuelStruct, right1fuelsensorzero),2},
    {"右1油箱油量传感器满值",offsetof(fuelStruct, right1fuelsensorfull),2},
    {"右2油箱油量传感器零值",offsetof(fuelStruct, right2fuelsensorzero),2},
    {"右2油箱油量传感器满值",offsetof(fuelStruct, right2fuelsensorfull),2},
    {"油面俯仰角",offsetof(fuelStruct, fuelfuyang),2,true,0.1},
    {"油面横滚角",offsetof(fuelStruct, fuelhenggun),2,true,0.1},
    {"温度传感器采样值",offsetof(fuelStruct, temperature),2},
    {"软件版本信息",offsetof(fuelStruct, softversion),1},
    {"全机校准信息反馈",offsetof(fuelStruct, allcheckfeedback),1},
    {"温度校准信息反馈",offsetof(fuelStruct, temperaturefeedback),1},
    };


// Q_DECLARE_METATYPE(sendParamters)

#endif // ICD_H
