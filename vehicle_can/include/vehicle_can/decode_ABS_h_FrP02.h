#ifndef DECODE_ABS_H_FRP02_H
#define DECODE_ABS_H_FRP02_H

#include "vehicle_can/utils.h"
#include "vehicle_can/base_decoder.h"

class ABS_h_FrP02 : public BaseDecoder
{
public:
    void decode_and_fill(const std::string& data_bin, vehicle_can::msg::Canmsg& msg, const std::string& timestamp) override;

    std::string getWhlFl_W_Meas(std::string args);
    std::string getWhlFr_W_Meas(std::string args); 
    std::string getWhlRl_W_Meas(std::string args); 
    std::string getWhlRr_W_Meas(std::string args);

    std::vector<Variable> decode_ABS_h_FrP02(std::string data_bin);
};

#endif