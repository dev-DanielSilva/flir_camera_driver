# Vehicle Sensors

Este repositório contém os pacotes ROS 2 responsáveis pela integração e pelo gerenciamento dos sensores utilizados no veículo.

Cada sensor é implementado como um **pacote ROS 2 independente**, mas todos permanecem dentro de um único repositório Git.

Essa abordagem facilita a manutenção do sistema, o compartilhamento de configurações e a integração entre sensores, sem perder a modularidade oferecida pelos pacotes ROS 2.

---

## Estrutura do repositório

A estrutura geral é organizada da seguinte forma:

```text
vehicle_sensors/
├── vehicle_camera/
├── vehicle_lidar/
├── vehicle_radar/
├── vehicle_gnss/
├── vehicle_imu/
├── vehicle_sensors_bringup/
├── README.md
└── .gitignore
```

Cada diretório `vehicle_*` representa um **pacote ROS 2 independente**.

---

### `vehicle_sensors_bringup`

O pacote `vehicle_sensors_bringup` é responsável pela **inicialização conjunta dos sensores do veículo**.

---

# Instalação

## 1. Criar o workspace

Caso ainda não exista um workspace ROS 2:

```bash
mkdir -p ~/vehicle_ws/src
```

Entre na pasta `src`:

```bash
cd ~/vehicle_ws/src
```

---

## 2. Clonar o repositório

Clone este repositório dentro da pasta `src`:

```bash
git clone git@github.com:CRob-Focus/vehicle_sensing.git
```

A estrutura deverá ficar semelhante a:

```text
~/vehicle_ws/src/vehicle_sensing/
```

---

## 3. Instalar dependências

Entre na raiz do workspace:

```bash
cd ~/vehicle_ws
```

Carregue a instalação do ROS 2 Humble:

```bash
source /opt/ros/humble/setup.bash
rosdep install --from-paths src --ignore-src -r -y
```

# Compilação

A compilação deve ser realizada **a partir da raiz do workspace**, e não dentro do repositório.

```bash
cd ~/vehicle_ws
colcon build --symlink-install
```

Após a compilação:

```bash
source install/setup.bash
```

---

## Verificando os pacotes

Para verificar se os pacotes foram reconhecidos pelo ROS 2:

```bash
ros2 pkg list | grep vehicle
```

Os pacotes disponíveis deverão aparecer na saída, por exemplo:

```text
vehicle_camera
vehicle_gnss
vehicle_imu
vehicle_lidar
vehicle_radar
vehicle_sensors_bringup
```
