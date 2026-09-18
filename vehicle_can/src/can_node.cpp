#include <iostream>
#include <string>
#include <bitset>
#include <iomanip>
#include <sstream>
#include <map>
#include <vector>
#include <memory>
#include <functional>
#include <thread>

#include "vehicle_can/decode_ABS_h_FrP00.h" 
#include "vehicle_can/decode_ABS_h_FrP02.h"
#include "vehicle_can/decode_SASM_h_FrP00.h"


#include "rclcpp/rclcpp.hpp"
#include "vehicle_can/msg/canmsg.hpp" 
extern "C" {
    #include "vehicle_can/i7565H1H2.h"
}
using namespace std::placeholders;

class CanNode : public rclcpp::Node
{
public: 
    CanNode() : Node("can_node")
    {   
        // Registro dos decodificadores
        decoders_["160"] = std::make_unique<ABS_h_FrP00>();
        decoders_["190"] = std::make_unique<ABS_h_FrP02>();
        decoders_["10"] = std::make_unique<SASM_h_FrP00>();

        int port = 0; // Define a porta USB (ex: ttyUSB0)

        // Configuração dos parâmetros do conversor
        _VCI_CAN_PARAM param;
        param.DevPort = port;
        param.DevType = 2;         // Indica o modelo H2 (duas portas CAN)
        param.CAN1_Baud = 500000;  // Velocidade da rede CAN 1 (500kbps)
        param.CAN2_Baud = 125000;  // Velocidade da rede CAN 2 (125kbps)

        // Liga o hardware. Se retornar algo diferente de 0, o aparelho não foi encontrado.
        if (VCI_OpenCAN(&param) != 0) {
            RCLCPP_ERROR(this->get_logger(), "Falha ao abrir I-7565-H2 no ttyUSB%d com saida %d", port, VCI_OpenCAN(&param));
            return;
        }

        this->declare_parameter("can_id", -1);
        can_id_ = this->get_parameter("can_id").as_int();

        RCLCPP_INFO(this->get_logger(), "CAN Node Parser %d inicializado!", can_id_);
        //Cria o tópico de publicamento para as multiplas cans
        can_pub_ = this->create_publisher<vehicle_can::msg::Canmsg>(
            "focus/parsed_can" + std::to_string(can_id_), 10);

        read_thread_ = std::thread(&CanNode::read_callback, this);
    }
    ~CanNode() {
        VCI_CloseCAN(0);
        if (read_thread_.joinable()) {
        read_thread_.join();// Fechar Thread
        }
    }

private:
    int can_id_;
    int port;
    std::thread read_thread_;
    rclcpp::Publisher<vehicle_can::msg::Canmsg>::SharedPtr can_pub_;
    std::map<std::string, std::unique_ptr<BaseDecoder>> decoders_;
    rclcpp::TimerBase::SharedPtr timer_;
    vehicle_can::msg::Canmsg can_msg_;

    void read_callback() {
	    
        while (rclcpp::ok()){
            _VCI_CAN_MSG can_msg;
            //DADOS DA _VCI_CAN_MSG
            // Mode: CAN message Mode (0: 11-bit; 1: 29-bit).
            // RTR: CAN message RTR (0: No RTR; 1: RTR).
            // DLC: CAN message Data Length (0~8).
            // ID: CAN message ID.
            // TimeL: CAN message Time-Stamp (Lo-DWORD).
            // TimeH: CAN message Time-Stamp (Hi-DWORD).
            // Data[8]: CAN message Data Array.
            // enquanto o VCI_RecvCANMsg retornar 0 (sucesso)
            if (VCI_RecvCANMsg(port, can_id_+1, &can_msg) == 0) {

                if(can_msg.DLC > 0 && can_msg.DLC <= 8){
                    std::vector<uint8_t> split_msg(16,0);
                    //Message Frame: [I2I3, 0I1, 00, CC, T0, T1, T2, T3, AA, BB, CC, DD, EE, FF, GG, HH]

                    // ID e Canal
                    split_msg[0]=((can_msg.ID & 0xFF)); // ID da mensagem I2I#
                    split_msg[1]=((can_msg.ID >> 8) & 0x0F); //ID da mensagem 0I1
                    split_msg[2]=(0x00); //Espaçamento de 1 byte
                    split_msg[3]=((static_cast<uint8_t>(can_id_))); //Canal Cc

                    //TimeL
                    split_msg[4]=((can_msg.TimeL >> 24) & 0xFF); // Pega o primeiro byte de timestamp
                    split_msg[5]=((can_msg.TimeL >> 16) & 0xFF); // Pega o segundo byte de timestamp
                    split_msg[6]=((can_msg.TimeL >> 8) & 0xFF); // Pega o terceiro byte de timestamp
                    split_msg[7]=(can_msg.TimeL & 0xFF); // Pega o quarto byte de timestamp

                    // Dados
                    std::copy(can_msg.Data, can_msg.Data + 8, split_msg.begin() + 8); // Dados sempre no 8

                    if (split_msg.size() != 16) {
                        RCLCPP_INFO(this->get_logger(), "Mensagem inválida.");
                        return; 
                    }
                    parser_callback(split_msg);
                }
            } else {
                std::this_thread::sleep_for(std::chrono::microseconds(100));
            }
        }RCLCPP_INFO(this->get_logger(), "Thread de leitura encerrada com sucesso.");
	}

    std::map<std::string, std::string> internal_parser(const std::vector<uint8_t>& data) {
        std::string hex_word = "";
        std::string bin_word = "";
        
        //Conversor dos bytes em strings
        for (unsigned char byte : data) {
            std::stringstream ss;
            //Montar uma stringstream em hexdecimal colocando de 2 em 2 elementos
            ss << std::hex /*Trabalhar com hexdecimal*/ << std::setw(2) /*Preencher com até 2 valores*/<< std::setfill('0') << (int)byte;
            hex_word += ss.str(); //Montar string normal
            bin_word += std::bitset<8>(byte).to_string(); //Montar de binário agrupando os bytes em bits
        }
        //Cria uma map para organização das informações da mensagem
        std::map<std::string, std::string> msg_parsed;
        //Guarda as informações da mensagem em diferentes conteiners para o publisher
        if (hex_word.length() >= 16) {
            std::string msg_id = "";
            if (hex_word[3] != '0') msg_id = hex_word[3]; //Pegar o terceiro elemento caso ele seja difernte de zero
            msg_id += hex_word.substr(0, 2); //Adicionar o 2 e 3  elementos

            msg_parsed["msg_id"] = msg_id;
            msg_parsed["channel"] = hex_word.substr(6, 2);
            msg_parsed["timestamp"] = hex_word.substr(8, 8);
            msg_parsed["data_hex"] = hex_word.substr(16);
            msg_parsed["data_bin"] = bin_word.substr(64, 64); // Pega exatamente os 8 bytes de dados
            
        }

        return msg_parsed;
    }

    void parser_callback(const std::vector<uint8_t>& msg) {
        //Chama o internal_parser para decodificar a mensagem
        auto parsed = internal_parser(msg);
        std::string id = parsed["msg_id"]; //Salva o id da mensagem

        if (decoders_.count(id))//Verifica o id registrado anteriormente
         {
            decoders_[id]->decode_and_fill(parsed["data_bin"], can_msg_, parsed["timestamp"]); //Chama as funções decodificadores de cada id entrando com o o data_bin
            //Publica a mensagem
            can_pub_->publish(can_msg_);
        }
    }
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<CanNode>());
  rclcpp::shutdown();
  return 0;
}
