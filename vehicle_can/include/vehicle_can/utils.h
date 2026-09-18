#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <memory>
#include <vector>
#include <cmath>
#include <iostream>
#include <functional>
#include <stdexcept>

class Variable {
private:
    std::string name;
    std::string var_data_bin;
    std::string decoded_value;
    std::string unit;

    std::string get_var(const std::string& data_bin, int length, int start_bit);

public:
    Variable() = default; 
    
    Variable(const std::string& data_bin, int length, int start_bit, 
             std::function<std::string(std::string)> decode_fn, 
             std::string var_name, std::string var_unit = "");

    void print_var_data_bin() const;
    void print_decoded_value() const;
    std::string get_decoded_value() const;
    
    long to_int(std::string s) const;
    std::string status_switcher(std::string args) const;
};

extern Variable variable;

#endif