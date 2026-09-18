#include "vehicle_can/decode_SASM_h_FrP00.h"

using namespace std;
using namespace placeholders;

void SASM_h_FrP00::decode_and_fill(const std::string& data_bin, vehicle_can::msg::Canmsg& msg, const std::string& timestamp){
        auto vars = decode_SASM_h_FrP00(data_bin);

        try {
            float temp_ang_cr_short = stof(vars[0].get_decoded_value());
            float temp_ang_cr = stof(vars[1].get_decoded_value());
            float tem_ang = stof(vars[2].get_decoded_value());

            msg.steer_angle_cr_short_filt = temp_ang_cr_short;
            msg.steering_angle_cr = temp_ang_cr;
            msg.steering_angle = tem_ang;

            msg.timestamp_abs_h_frp00 = stof(timestamp);
        }
        catch (const std::exception& e) {
            //std::cerr << "Dados invalidos recebidos. Mantendo valores anteriores. Erro: " << e.what() << std::endl;
        } 
    }

string SASM_h_FrP00::getSteeringAngleStatus(string args){
    switch (variable.to_int(args)) {
        case 0: return "0: 'Correct calculated SteeringAngle och SteeringAngleCR signals'";
        case 1: return "1: 'SAS ECU are not initialized and no faults are detected'";
        case 2: return "2: 'Fault detected, SAS nonactive until next driving cycle'";
        case 3: return "3: 'Not defined'";
        default: return "Invalid Argument!";
    }
}

string SASM_h_FrP00::getSteeringAngleCRSign(string args){
    switch (variable.to_int(args)) {
        case 0: return "0: 'Steering wheel velocity left (Counterclockwise)'";
        case 1: return "1: 'Steering wheel velocity right (Clockwise)'";
        default: return "Invalid Argument!";
    }
}

string SASM_h_FrP00::getSteeringAngleSign(string args){
    /*The signal inform about actual steering angle direction, from the centre position of the steering wheel. Used in
    the AYC-function calculations. The absolute steering wheel angle is supplied in the signal SteeringAngle.*/
    switch (variable.to_int(args)) {
        case 0: return "0: 'Left turn (Counterclockwise)'";
        case 1: return "1: 'Right turn (Clockwise)'";
        default: return "Invalid Argument!";
    }
}

std::vector<Variable> SASM_h_FrP00::decode_SASM_h_FrP00(std::string data_bin){
        vector<Variable> vars;
        vars.reserve(3);
        //vars.emplace_back(data_bin, 2, 2, bind(&SASM_h_FrP00::getSteeringAngleStatus, this, _1), "SteeringAngleStatus");
        //vars.emplace_back(data_bin, 4, 4, [this](string s){return to_string(variable.to_int(s));}, "SteeringAngleCounter");
        //vars.emplace_back(data_bin, 8, 8, [this](string s){return to_string(variable.to_int(s));},"SteeringAngleCS");
        //vars.emplace_back(data_bin, 1, 16, bind(&SASM_h_FrP00::getSteeringAngleCRSign), "SteeringAngleCRSign");

        /*To inform power steering about actual short filtered steering angle change rate of the steering wheel. The value
        is an absolute value.*/
        vars.emplace_back(data_bin, 15, 17, [this](string s){return to_string(variable.to_int(s)*0.06104);},"SteerAngleCRShortFilt","Degr/s");
        
        //vars.emplace_back(data_bin, 1, 32, bind(&SASM_h_FrP00::getSteeringAngleSign), "SteeringAngleSign");
        vars.emplace_back(data_bin, 15, 33, [this](string s){return to_string(variable.to_int(s)*0.06104);},"SteeringAngleCR", "Degr/s");
        //vars.emplace_back(data_bin, 1, 48, [this](string s){return to_string(variable.to_int(s));},"SteeringAngle_UB");
        vars.emplace_back(data_bin, 15, 49, [this](string s){return to_string(variable.to_int(s)*0.04395);},"SteeringAngle", "Degr"); // Posição do Volante

        //for (const auto& v : vars) v.print_decoded_value();

        return vars;
}
