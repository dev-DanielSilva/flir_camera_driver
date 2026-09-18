#include "vehicle_can/decode_ABS_h_FrP02.h"
 
using namespace std;
using namespace placeholders;

void ABS_h_FrP02::decode_and_fill(const std::string& data_bin, vehicle_can::msg::Canmsg& msg, const std::string& timestamp){
    auto vars = decode_ABS_h_FrP02(data_bin);

    try{
        float whlfl = stof(vars[0].get_decoded_value());
        float whlfr = stof(vars[1].get_decoded_value());
        float whlrl = stof(vars[2].get_decoded_value());
        float whlrr = stof(vars[3].get_decoded_value());
        
        msg.whlfl_w_meas = whlfl;
        msg.whlfr_w_meas = whlfr;
        msg.whlrl_w_meas = whlrl;
        msg.whlrr_w_meas = whlrr;

        msg.timestamp_abs_h_frp00 = stof(timestamp);
    } 
    catch (const std::exception& e) {
        //std::cerr << "Dados invalidos recebidos. Mantendo valores anteriores. Erro: " << e.what() << std::endl;
    }   
}

string ABS_h_FrP02::getWhlFl_W_Meas(string args){
    if (args == "111111111111110") return "'Unknown'";
    if (args == "111111111111111") return "'Fault'";

    return to_string(variable.to_int(args) * 0.01);
}

string ABS_h_FrP02::getWhlFr_W_Meas(string args){
    if (args == "111111111111110") return "'Unknown'";
    if (args == "111111111111111") return "'Fault'";

    return to_string(variable.to_int(args) * 0.01);
}

string ABS_h_FrP02::getWhlRl_W_Meas(string args){
    if (args == "111111111111110") return "'Unknown'";
    if (args == "111111111111111") return "'Fault'";

    return to_string(variable.to_int(args) * 0.01);
}

string ABS_h_FrP02::getWhlRr_W_Meas(string args){
    if (args == "111111111111110") return "'Unknown'";
    if (args == "111111111111111") return "'Fault'";

    return to_string(variable.to_int(args) * 0.01);
}

vector<Variable> ABS_h_FrP02::decode_ABS_h_FrP02(string data_bin) {
        vector<Variable> vars;
        vars.reserve(4);

        //vars.emplace_back(data_bin, 1, 0, [this](string s){ return to_string(variable.to_int(s)); }, "getWhlFl_W_Meas_UB");
        vars.emplace_back(data_bin, 15, 1, bind(&ABS_h_FrP02::getWhlFl_W_Meas, this, _1), "getWhlFl_W_Meas", "rad/s");
        //vars.emplace_back(data_bin, 1, 16, [this](string s){ return to_string(variable.to_int(s)); }, "getWhlFr_W_Meas_UB");
        vars.emplace_back(data_bin, 15, 17, bind(&ABS_h_FrP02::getWhlFr_W_Meas, this, _1), "getWhlFr_W_Meas", "rad/s");
        //vars.emplace_back(data_bin, 1, 32, [this](string s){ return to_string(variable.to_int(s)); }, "getWhlRl_W_Meas_UB");
        vars.emplace_back(data_bin, 15, 33, bind(&ABS_h_FrP02::getWhlRl_W_Meas, this, _1), "getWhlRl_W_Meas", "rad/s");
        //vars.emplace_back(data_bin, 1, 48, [this](string s){ return to_string(variable.to_int(s)); }, "getWhlRr_W_Meas_UB"); 
        vars.emplace_back(data_bin, 15, 49, bind(&ABS_h_FrP02::getWhlRr_W_Meas, this, _1), "getWhlRr_W_Meas", "rad/s");

        //for (const auto& v : vars) v.print_decoded_value();

        return vars;
    }
