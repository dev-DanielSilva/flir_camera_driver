#ifndef BASE_DECODER_H
#define BASE_DECODER_H

#include <vector>
#include <string>
#include "vehicle_can/utils.h"
#include "vehicle_can/msg/canmsg.hpp"

class BaseDecoder {
public:
    virtual ~BaseDecoder() = default;
    // Cada sensor vai implementar sua própria lógica aqui dentro
    virtual void decode_and_fill(const std::string& data_bin, vehicle_can::msg::Canmsg& msg, const std::string& timestamp) = 0;
};

#endif