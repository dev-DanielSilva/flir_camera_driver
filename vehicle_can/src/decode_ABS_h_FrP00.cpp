#include "vehicle_can/decode_ABS_h_FrP00.h"

using namespace std;
using namespace placeholders;

    void ABS_h_FrP00::decode_and_fill(const std::string& data_bin, vehicle_can::msg::Canmsg& msg, const std::string& timestamp){
        auto vars = decode_ABS_h_FrP00(data_bin);
    
        try {
        // Se qualquer um falhar, o fluxo pula para o catch ANTES de atualizar msg.
        float temp_a_est = std::stof(vars[0].get_decoded_value());
        float temp_v_est = std::stof(vars[1].get_decoded_value());
        float temp_a_act = std::stof(vars[2].get_decoded_value());

        //Enviar os dados vállidos
        msg.veh_long_ovr_gnd_a_est = temp_a_est;
        msg.veh_over_gnd_v_est     = temp_v_est;
        msg.veh_long_comp_a_actl   = temp_a_act;

        msg.timestamp_abs_h_frp00 = stof(timestamp);
        } 
        catch (const std::exception& e) {
            //std::cerr << "Dados invalidos recebidos. Mantendo valores anteriores. Erro: " << e.what() << std::endl;
        }   
    }
    string ABS_h_FrP00::getVehLongOvrGnd_A_Est(string args) {
        if (args == "1111111110") return "'Unknown'";
        if (args == "1111111111") return "'Fault'";
        return to_string(variable.to_int(args) * 0.035 - 17.9);
    }

    string ABS_h_FrP00::getVehOverGnd_V_Est(string args) {
        if (args == "1111111111111110") return "'Unknown'";
        if (args == "1111111111111111") return "'Fault'";
        return to_string(variable.to_int(args) * 0.01);
    }

    string ABS_h_FrP00::getTCMode(string args) {
        int val = variable.to_int(args);
        if (val == 0) return "0: 'No regulation (ABS/SC/TC/MSR/AYC/RSC/EBA)'";
        if (val == 1) return "1: 'Regulation (ABS/SC/TC/MSR/AYC/RSC/EBA)'";
        return "Invalid Status!";
    }

    string ABS_h_FrP00::getVehicleSpeedOverGndQF(string args) {
        switch (variable.to_int(args)) {
            case 0: return "0: 'The accuracy of the data is undefinable.'";
            case 1: return "1: 'The data is temp. undefined, evaluation in progress.'";
            case 2: return "2: 'The accuracy of the data is not within specification.'";
            case 3: return "3: 'The data is calculated with the specified accuracy.'";
            default: return "Invalid Status!";
        }
    }

    vector<Variable> ABS_h_FrP00::decode_ABS_h_FrP00(string data_bin) {
        vector<Variable> vars;
        vars.reserve(3);
    
        //vars.emplace_back(data_bin, 1, 1, [this](string s){ return to_string(variable.to_int(s)); }, "PrplDrgCtlActv_B_Actl_UB");
        //vars.emplace_back(data_bin, 1, 2, bind(&ABS_h_FrP00::variable.status_switcher, this, _1), "PrplDrgCtlActv_B_Actl");
        //vars.emplace_back(data_bin, 1, 3, [this](string s){ return to_string(variable.to_int(s)); }, "StabCtlBrkActv_B_Actl_UB");
        //vars.emplace_back(data_bin, 1, 4, bind(&ABS_h_FrP00::variable.status_switcher, this, _1), "StabCtlBrkActv_B_Actl");
        //vars.emplace_back(data_bin, 1, 5, [this](string s){ return to_string(variable.to_int(s)); }, "TracCtlPtActv_B_Actl_UB");
        vars.emplace_back(data_bin, 10, 6, bind(&ABS_h_FrP00::getVehLongOvrGnd_A_Est, this, _1), "VehLongOvrGnd_A_Est", "m/s^2");
        vars.emplace_back(data_bin, 16, 16, bind(&ABS_h_FrP00::getVehOverGnd_V_Est, this, _1), "VehOverGnd_V_Est", "km/h");
        //vars.emplace_back(data_bin, 1, 32, bind(&ABS_h_FrP00::variable.status_switcher, this, _1), "TracCtlPtActv_B_Actl");
        //vars.emplace_back(data_bin, 1, 33, [this](string s){ return to_string(variable.to_int(s)); }, "TCMode_UB");
        //vars.emplace_back(data_bin, 1, 34, bind(&ABS_h_FrP00::getTCMode, this, _1), "TCMode");
        //vars.emplace_back(data_bin, 1, 35, [this](string s){ return to_string(variable.to_int(s)); }, "AbsActv_B_Actl_UB");
        //vars.emplace_back(data_bin, 1, 36, bind(&ABS_h_FrP00::variable.status_switcher, this, _1), "AbsActv_B_Actl");
        //vars.emplace_back(data_bin, 1, 37, [this](string s){ return to_string(variable.to_int(s)); }, "VehLongOvrGnd_A_Est_UB");
        //vars.emplace_back(data_bin, 2, 38, bind(&ABS_h_FrP00::getVehicleSpeedOverGndQF, this, _1), "VehicleSpeedOverGndQF");
        //vars.emplace_back(data_bin, 8, 40, [this](string s){ return to_string(variable.to_int(s)); }, "VehicleSpeedOverGndCS");
        //vars.emplace_back(data_bin, 1, 48, [this](string s){ return to_string(variable.to_int(s)); }, "VehOverGnd_V_Est_UB");
        //vars.emplace_back(data_bin, 4, 49, [this](string s){ return to_string(variable.to_int(s)); }, "VehOverGndVEst_No_Cnt");
        //vars.emplace_back(data_bin, 1, 53, [this](string s){ return to_string(variable.to_int(s)); }, "VehLongComp_A_Actl_UB");
        vars.emplace_back(data_bin, 10, 54, bind(&ABS_h_FrP00::getVehLongOvrGnd_A_Est, this, _1), "VehLongComp_A_Actl", "m/s^2"); 

        //for (const auto& v : vars) v.print_decoded_value();

        return vars;
    }