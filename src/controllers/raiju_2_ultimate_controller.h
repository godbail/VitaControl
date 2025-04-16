#ifndef RAIJU_2_ULTIMATE_CONTROLLER_H
#define RAIJU_2_ULTIMATE_CONTROLLER_H

#include "../controller.h"

struct Raiju2UltimateReport0x01
{
    // uint8_t 无符号8位整数 [0-255]
    uint8_t reportId;

    uint8_t leftX;
    uint8_t leftY;
    uint8_t rightX;
    uint8_t rightY;

    uint8_t dpad : 4;     // 十字方向键 1000:none 0111:left&up 0110:left 0101:down&left 0100:down 0011:right+down 0010:right 0001:up&right 0000:up
    uint8_t square : 1;   // □ 00011000 0x18
    uint8_t cross : 1;    // ×
    uint8_t circle : 1;   // ○
    uint8_t triangle : 1; // △ 10001000 0x88

    uint8_t l1 : 1;      // 肩键左1
    uint8_t r1 : 1;      // 肩键右1
    uint8_t l2 : 1;      // 肩键左2
    uint8_t r2 : 1;      // 肩键右2
    uint8_t share : 1;   // 分享键
    uint8_t options : 1; // 选项键
    uint8_t l3 : 1;      // 左类比摇杆按下
    uint8_t r3 : 1;      // 右类比摇杆按下

    uint8_t ps : 1;   // home键
    uint8_t tpad : 1; // touchpad
    uint8_t : 0;      // 填充

    uint8_t triggerL;
    uint8_t triggerR;

    uint16_t timestamp;
    uint8_t battery;

    int16_t velocityX;
    int16_t velocityY;
    int16_t velocityZ;

    int16_t accelerX;
    int16_t accelerY;
    int16_t accelerZ;

    uint8_t unk0[5];

    uint8_t batteryLevel : 4; // 电量等级
    uint8_t usbPlugged : 1;   // 是否连接USB?
    uint8_t headphones : 1;   //
    uint8_t microphone : 1;
    uint8_t : 0;

    uint8_t unk1[4];

    unsigned int touch1Id : 7;
    unsigned int touch1ActiveNeg : 1;
    unsigned int touch1X : 12;
    unsigned int touch1Y : 12;

    unsigned int touch2Id : 7;
    unsigned int touch2ActiveNeg : 1;
    unsigned int touch2X : 12;
    unsigned int touch2Y : 12;

    unsigned int touch3Id : 8;
    unsigned int touch4Id : 8;
}
__attribute__((packed));

class Raiju2UltimateController : public Controller
{
public:
    Raiju2UltimateController(uint32_t mac0, uint32_t mac1, int port);

    void processReport(uint8_t *buffer, size_t length);
};

#endif // RAIJU_2_ULTIMATE_CONTROLLER_H

//[2:2] 左摇杆X
//[4:2] 右摇杆X

//[6:1] 00001000 08 none

//[6:1] 00011000 18 square
//[6:1] 00101000 28 cross
//[6:1] 01001000 48 circle
//[6:1] 10001000 88 triangle

//[6:1] 00000000 00 up
//[6:1] 00000001 01 right|up
//[6:1] 00000010 02 right
//[6:1] 00000011 03 right|down
//[6:1] 00000100 04 down
//[6:1] 00000101 05 left|down
//[6:1] 00000110 06 left
//[6:1] 00000111 07 left|up

//[7:1] 00010000 10 share
//[7:1] 00100010 20 option
//[7:1] 01000000 40 L3
//[7:1] 10000000 80 R3
//[7:1] 00000001 01 L1
//[7:1] 00000010 02 R1
//[7:1] 00000100 04 L2
//[7:1] 00001000 08 R2

//[8:1] 00000001 01 PS
//[8:1] 00000010 02 TOUCH

//[9:1] 00000000 00-FF L3力反馈
//[10:1] 00000000 00-FF R3力反馈

//[36:4] Touch1
//[40:4] Touch2