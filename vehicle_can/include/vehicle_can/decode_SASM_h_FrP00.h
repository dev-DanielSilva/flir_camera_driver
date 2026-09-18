#ifndef DECODE_SASM_H_FRP00_H
#define DECODE_SASM_H_FRP00_H

#include "vehicle_can/utils.h"
#include "vehicle_can/base_decoder.h"

class SASM_h_FrP00 : public BaseDecoder
{

public:
    void decode_and_fill(const std::string &data_bin, vehicle_can::msg::Canmsg &msg, const std::string& timestamp) override;

    std::string getSteeringAngleStatus(std::string args);

    std::string getSteeringAngleCRSign(std::string args);

    std::string getSteeringAngleSign(std::string args);

    std::vector<Variable> decode_SASM_h_FrP00(std::string data_bin);
};

#endif