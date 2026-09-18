#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <cstring>
#include <csignal> // Necessário para tratar o sinal SIGPIPE (evita crash de rede)

// Sockets POSIX (Linux)
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

// ROS 2 C++
#include "rclcpp/rclcpp.hpp"
#include "mavros_msgs/msg/rtcm.hpp"
#include "nmea_msgs/msg/sentence.hpp"

class NtripBridge : public rclcpp::Node {
public:
  NtripBridge() : Node("ntrip_bridge"), running_(true), has_nmea_(false) {
    // 1. Publicador de correções RTCM para a Xsens
    rtcm_pub_ = this->create_publisher<mavros_msgs::msg::RTCM>("/rtcm", 10);

    // 2. Inscrito no tópico /nmea publicado pela Xsens
    nmea_sub_ = this->create_subscription<nmea_msgs::msg::Sentence>(
      "/nmea", rclcpp::SensorDataQoS(),
      std::bind(&NtripBridge::nmeaCallback, this, std::placeholders::_1)
    );

    RCLCPP_INFO(this->get_logger(), "Iniciando Ponte NTRIP C++ (Aguardando NMEA real da Xsens)...");

    // 3. Thread separada para o gerenciamento do Socket TCP
    worker_thread_ = std::thread(&NtripBridge::ntripLoop, this);
  }

  ~NtripBridge() {
    running_ = false;
    if (worker_thread_.joinable()) {
      worker_thread_.join();
    }
  }

private:
  // Callback executado sempre que a Xsens publica uma nova sentença NMEA
  void nmeaCallback(const nmea_msgs::msg::Sentence::SharedPtr msg) {
    // Filtra apenas sentenças GGA (que contêm as coordenadas reais do carro)
    if (msg->sentence.find("GGA") != std::string::npos) {
      std::lock_guard<std::mutex> lock(nmea_mutex_);
      latest_nmea_ = msg->sentence;

      // Remove caracteres de quebra de linha (\r ou \n) do final da string
      while (!latest_nmea_.empty() && (latest_nmea_.back() == '\r' || latest_nmea_.back() == '\n')) {
        latest_nmea_.pop_back();
      }
      has_nmea_ = true;
    }
  }

  // Função auxiliar para codificar o usuário:senha em Base64
  std::string base64Encode(const std::string &in) {
    static const std::string chars = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    int val = 0, valb = -6;
    for (unsigned char c : in) {
      val = (val << 8) + c;
      valb += 8;
      while (valb >= 0) {
        out.push_back(chars[(val >> valb) & 0x3F]);
        valb -= 6;
      }
    }
    if (valb > -6) out.push_back(chars[((val << 8) >> (valb + 8)) & 0x3F]);
    while (out.size() % 4) out.push_back('=');
    return out;
  }

  void ntripLoop() {
    std::string host = "170.84.40.52";
    int port = 2101;
    std::string mountpoint = "EESC0";
    std::string user = "lasiveic";
    std::string password = "1cL&468*";

    std::string auth_b64 = base64Encode(user + ":" + password);

    while (running_ && rclcpp::ok()) {
      // ETAPA A: Aguarda a Xsens publicar a primeira mensagem NMEA válida no tópico /nmea
      {
        std::unique_lock<std::mutex> lock(nmea_mutex_);
        if (!has_nmea_) {
          RCLCPP_INFO_THROTTLE(
            this->get_logger(), *this->get_clock(), 3000,
            "Aguardando primeira coordenada NMEA (GGA) real da Xsens no topico /nmea..."
          );
          lock.unlock();
          std::this_thread::sleep_for(std::chrono::milliseconds(500));
          continue;
        }
      }

      // ETAPA B: Captura a coordenada NMEA mais recente
      std::string current_nmea;
      {
        std::lock_guard<std::mutex> lock(nmea_mutex_);
        current_nmea = latest_nmea_;
      }

      RCLCPP_INFO(this->get_logger(), "Conectando ao Caster %s:%d/%s com NMEA real...", host.c_str(), port, mountpoint.c_str());

      // ETAPA C: Cria o Socket TCP
      int sock = socket(AF_INET, SOCK_STREAM, 0);
      if (sock < 0) {
        RCLCPP_ERROR(this->get_logger(), "Erro ao criar Socket TCP.");
        std::this_thread::sleep_for(std::chrono::seconds(5));
        continue;
      }

      // Timeouts de Leitura e Escrita (5 segundos)
      struct timeval tv;
      tv.tv_sec = 5;
      tv.tv_usec = 0;

      setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
      setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv));

      // Endereçamento do Caster
      struct sockaddr_in server_addr{};
      server_addr.sin_family = AF_INET;
      server_addr.sin_port = htons(port);
      inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr);

      // Conecta fisicamente via TCP
      if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        RCLCPP_WARN(this->get_logger(), "Falha na conexão TCP. Reconectando em 5s...");
        close(sock);
        std::this_thread::sleep_for(std::chrono::seconds(5));
        continue;
      }

      // ETAPA D: Monta o cabeçalho HTTP usando a NMEA real obtida do tópico
      std::string headers = 
        "GET /" + mountpoint + " HTTP/1.1\r\n" +
        "Host: " + host + ":" + std::to_string(port) + "\r\n" +
        "User-Agent: NTRIP Client\r\n" +
        "Authorization: Basic " + auth_b64 + "\r\n" +
        "Nmea: " + current_nmea + "\r\n" +
        "Connection: close\r\n\r\n";

      if (send(sock, headers.c_str(), headers.length(), MSG_NOSIGNAL) < 0) {
        RCLCPP_WARN(this->get_logger(), "Falha ao enviar cabecalho HTTP. Reconectando em 5s...");
        close(sock);
        std::this_thread::sleep_for(std::chrono::seconds(5));
        continue;
      }

      char buffer[1024];
      ssize_t bytes_read = recv(sock, buffer, sizeof(buffer) - 1, 0);

      if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        std::string response(buffer);

        if (response.find("200") != std::string::npos || response.find("ICY") != std::string::npos) {
          RCLCPP_INFO(this->get_logger(), "CONECTADO COM SUCESSO! Transmitindo RTCM e atualizando NMEA...");

          auto last_nmea_send = std::chrono::steady_clock::now();
          int consecutive_timeouts = 0;

          while (running_ && rclcpp::ok()) {
            // 1. Envia a posição NMEA real atualizada ao Caster a cada 10 segundos
            auto now_time = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now_time - last_nmea_send).count() >= 10) {
              std::string nmea_to_send;
              {
                std::lock_guard<std::mutex> lock(nmea_mutex_);
                nmea_to_send = latest_nmea_;
              }
              std::string nmea_cmd = nmea_to_send + "\r\n";
              
              if (send(sock, nmea_cmd.c_str(), nmea_cmd.length(), MSG_NOSIGNAL) < 0) {
                RCLCPP_WARN(this->get_logger(), "Falha ao enviar NMEA (Rede offline). Fechando socket...");
                break;
              }
              last_nmea_send = now_time;
            }

            // 2. Recebe dados RTCM do Caster
            ssize_t n = recv(sock, buffer, sizeof(buffer), 0);
            
            if (n > 0) {
              consecutive_timeouts = 0; // Zera contador pois os dados estão chegando
              
              auto msg = mavros_msgs::msg::RTCM();
              msg.header.stamp = this->now();
              msg.header.frame_id = "llh";
              msg.data.assign(buffer, buffer + n);

              rtcm_pub_->publish(msg);
            } 
            else if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
              consecutive_timeouts++;
              RCLCPP_WARN(this->get_logger(), "Sem dados do Caster (Timeout %d/2)...", consecutive_timeouts);
              
              if (consecutive_timeouts >= 2) {
                RCLCPP_ERROR(this->get_logger(), "Conexão morta detectada! Forçando reconexão...");
                break; 
              }
            } 
            else {
              RCLCPP_WARN(this->get_logger(), "Conexão encerrada pelo servidor. Reconectando...");
              break; 
            }
          }
        } else {
          RCLCPP_ERROR(this->get_logger(), "Resposta de autorização inválida do Caster: %s", response.c_str());
        }
      }

      close(sock);
      std::this_thread::sleep_for(std::chrono::seconds(5));
    }
  }

  // Membros privados da classe
  rclcpp::Publisher<mavros_msgs::msg::RTCM>::SharedPtr rtcm_pub_;
  rclcpp::Subscription<nmea_msgs::msg::Sentence>::SharedPtr nmea_sub_;
  
  std::thread worker_thread_;
  std::mutex nmea_mutex_;
  std::string latest_nmea_;
  bool has_nmea_;
  bool running_;
};

int main(int argc, char** argv) {
  // IGNORA O SINAL DE BROKEN PIPE DO LINUX:
  std::signal(SIGPIPE, SIG_IGN);

  rclcpp::init(argc, argv);
  auto node = std::make_shared<NtripBridge>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
