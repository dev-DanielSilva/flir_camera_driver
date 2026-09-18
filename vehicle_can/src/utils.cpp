#include "vehicle_can/utils.h"

Variable variable; 

Variable::Variable(const std::string& data_bin, int length, int start_bit, 
                   std::function<std::string(std::string)> decode_fn, 
                   std::string var_name, std::string var_unit) 
{
    this->name = var_name;
    this->unit = var_unit;
    this->var_data_bin = get_var(data_bin, length, start_bit);
    this->decoded_value = decode_fn(this->var_data_bin);
}

std::string Variable::get_var(const std::string& data_bin, int length, int start_bit) {
    if (start_bit + length > (int)data_bin.length()) return "";
    return data_bin.substr(start_bit, length);
}

void Variable::print_var_data_bin() const {
    std::cout << name << ": " << var_data_bin << std::endl;
}

void Variable::print_decoded_value() const {
    std::cout << name << ": " << decoded_value << " " << unit << std::endl;
}

std::string Variable::get_decoded_value() const {
     return decoded_value; 
}
    
long Variable::to_int(std::string s) const {
    // stol converte de binário (base 2) para long
    try {
        return std::stol(s, nullptr, 2);
    } catch (...) {
        return 0;
    }
}

std::string Variable::status_switcher(std::string args) const {
    return (to_int(args) == 1) ? "1: 'Active'" : "0: 'Inactive'";
}