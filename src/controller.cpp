#include <cstring>
#include <psp2kern/bt.h>

#include "controller.h"
#include "mempool.h"
#include "controllers/dualshock3_controller.h"
#include "controllers/dualshock4_controller.h"
#include "controllers/raiju_2_ultimate_controller.h"
#include "controllers/dualsense_controller.h"
#include "controllers/xbox_one_controller.h"
#include "controllers/xbox_one_controller_2016.h"
#include "controllers/switch_pro_controller.h"
#include "logPlus.h"

inline void *operator new(std::size_t, void *__p) throw() { return __p; }

#define DECL_CONTROLLER(vid, pid, name) \
    case ((vid) << 16) | (pid):         \
        return new (Mempool::alloc(sizeof(name))) name(mac0, mac1, port)

Controller *Controller::makeController(uint32_t mac0, uint32_t mac1, int port)
{
    // Get the VID and PID of the device with the given MAC address
    uint16_t id[2];
    ksceBtGetVidPid(mac0, mac1, id);
    char deviceName[0x80] = {0};
    ksceBtGetDeviceName(mac0, mac1, deviceName);

    // LOG("deviceName: %s\n", deviceName);
    // LOG("vid: %x\n", id[0]);
    // LOG("pid: %x\n", id[1]);

    if (strcmp(deviceName, "Razer Raiju Ultimate") == 0 && id[0] == 0 && id[1] == 0)
    {
        // LOG("VIDPID set\n");
        id[0] = 0x1532;
        id[1] = 0x1009;
    }

    // Match the VID and PID to a controller type, and create one if it exists
    switch ((id[0] << 16) | id[1])
    {
        DECL_CONTROLLER(0x054C, 0x0268, DualShock3Controller);
        DECL_CONTROLLER(0x054C, 0x05C4, DualShock4Controller);
        DECL_CONTROLLER(0x054C, 0x09CC, DualShock4Controller);
        DECL_CONTROLLER(0x1532, 0x1009, Raiju2UltimateController); // Gaming Controller [Raiju 2 Ultimate Edition (BT)]
        DECL_CONTROLLER(0x054C, 0x0CE6, DualSenseController);
        DECL_CONTROLLER(0x054C, 0x0DF2, DualSenseController);
        DECL_CONTROLLER(0x045E, 0x02E0, XboxOneController2016);
        DECL_CONTROLLER(0x045E, 0x02FD, XboxOneController);
        DECL_CONTROLLER(0x045E, 0x0B00, XboxOneController);
        DECL_CONTROLLER(0x045E, 0x0B05, XboxOneController);
        DECL_CONTROLLER(0x045E, 0x0B0A, XboxOneController);
        DECL_CONTROLLER(0x057E, 0x2009, SwitchProController);
    }

    return nullptr;
}

void Controller::requestReport(uint8_t type, uint8_t *buffer, size_t length)
{
    static SceBtHidRequest request;
    memset(&request, 0, sizeof(SceBtHidRequest));

    // Clear the buffer for read requests
    if (type == HID_REQUEST_READ)
        memset(buffer, 0, length);

    // Build a report request
    request.type = type;
    request.buffer = buffer;
    request.length = length;
    request.next = &request;

    // Send the request to the controller
    ksceBtHidTransfer(mac0, mac1, &request);
}

uint32_t Controller::calculateCrc(uint8_t *buffer, size_t length)
{
    // Calculate the CRC of the given data (used in requests for some controllers)
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < length; i++)
    {
        uint8_t ch = buffer[i];
        for (size_t j = 0; j < 8; j++)
        {
            uint32_t b = (ch ^ crc) & 1;
            crc >>= 1;
            if (b)
                crc = crc ^ 0xEDB88320;
            ch >>= 1;
        }
    }
    return ~crc;
}
