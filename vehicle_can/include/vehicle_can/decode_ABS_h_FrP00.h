#ifndef DECODE_ABS_H_FRP00_H
#define DECODE_ABS_H_FRP00_H

#include "vehicle_can/utils.h"
#include "vehicle_can/base_decoder.h"


class ABS_h_FrP00 : public BaseDecoder
{
public:

    void decode_and_fill(const std::string& data_bin, vehicle_can::msg::Canmsg& msg, const std::string& timestamp) override;

    std::string getWhlFl_W_Meas(std::string args);
    
    std::string getVehLongOvrGnd_A_Est(std::string args);

    std::string getVehOverGnd_V_Est(std::string args); 

    std::string getTCMode(std::string args); 

    std::string getVehicleSpeedOverGndQF(std::string args);

    std::vector<Variable> decode_ABS_h_FrP00(std::string data_bin) ;

};

#endif 
