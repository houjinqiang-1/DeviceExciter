#ifndef ICD_H
#define ICD_H


#include <QObject>
#include <cstring>

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

// typedef struct
// {
//     uint16_t P1;     //左1油箱油量
//     uint16_t P2;     //左2油箱油量
//     uint16_t P3;     //右1油箱油量
//     uint16_t P4;     //右2油箱油量
//     uint16_t P5;     //左油箱油量
//     uint16_t P6;     //右邮箱油量
//     uint16_t P7;    //总油量
//     int8_t P8;    //燃油温度
//     uint8_t P9;     //燃油高温告警
//     uint16_t P10;     //左供油管压力
//     uint16_t P11;     //右供油管压力
//     uint8_t P12;    //燃油温度
//     uint8_t P13;     //燃油高温告警
//     faultcode1 P14;
//     faultcode2 P15;
//     uint16_t P16;     //左1油箱油量
//     uint16_t P17;     //左2油箱油量
//     uint16_t P18;     //右1油箱油量
//     uint16_t P19;     //右2油箱油量
//     uint8_t reserve1;
//     uint16_t P20;     //左油箱油量
//     uint16_t P21;     //右邮箱油量
//     uint16_t P22;    //总油量
//     uint16_t P23;     //左1油箱油量
//     uint16_t P24;     //左2油箱油量
//     uint16_t P25;     //右1油箱油量
//     uint16_t P26;     //右2油箱油量
//     uint16_t P27;     //左油箱油量
//     int16_t P28;     //右邮箱油量
//     int16_t P29;    //总油量
//     uint16_t P30;
//     uint8_t P31;
//     uint8_t P32;
//     uint8_t P33;
//     uint8_t reserve2;
//     uint8_t reserve3;
// }fuel_Struct;

// typedef struct
// {
//     uint8_t frameHead1;
//     uint8_t frameHead2;
//     uint8_t frameSize;
//     fuel_Struct dataBase;
//     uint8_t check;
// }MSG_Struct;

typedef struct
{
    uint16_t left1fuel;//左1油箱油量
    uint16_t left2fuel;//左2油箱油量
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
    faultcode1 fault1; //错误码1
    faultcode2 fault2;//错误码2
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

}sendParamters;


typedef struct
{
    uint16_t P1;     //左1油箱油量
    uint16_t P2;     //左2油箱油量
    uint16_t P3;     //右1油箱油量
    uint16_t P4;     //右2油箱油量
    uint16_t P5;     //左油箱油量
    uint16_t P6;     //右邮箱油量
    uint16_t P7;    //总油量
    int8_t P8;    //燃油温度
    uint8_t P9;     //燃油高温告警
    uint16_t P10;     //左供油管压力
    uint16_t P11;     //右供油管压力
    uint8_t P12;    //燃油温度
    uint8_t P13;     //燃油高温告警
    faultcode1 P14;
    faultcode2 P15;
    uint16_t P16;     //左1油箱油量
    uint16_t P17;     //左2油箱油量
    uint16_t P18;     //右1油箱油量
    uint16_t P19;     //右2油箱油量
    uint8_t reserve1;
    uint16_t P20;     //左油箱油量
    uint16_t P21;     //右邮箱油量
    uint16_t P22;    //总油量
    uint16_t P23;     //左1油箱油量
    uint16_t P24;     //左2油箱油量
    uint16_t P25;     //右1油箱油量
    uint16_t P26;     //右2油箱油量
    uint16_t P27;     //左油箱油量
    int16_t P28;     //右邮箱油量
    int16_t P29;    //总油量
    uint16_t P30;
    uint8_t P31;
    uint8_t P32;
    uint8_t P33;
    uint8_t reserve2;
    uint8_t reserve3;
    uint8_t check;

} MSG_Struct;

#pragma pack()


#include <QMetaType>
Q_DECLARE_METATYPE(sendParamters)

#endif // ICD_H
