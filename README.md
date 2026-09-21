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
# Package vehicle_camera
##  **[MonoCameras]** Point Grey Flea3 FL3-U3-20E4C-C
### ROS Install steps:

1. Download and compile the ROS Package `flir_camera_driver`. For installation tutorials, please check [https://github.com/ros-drivers/flir_camera_driver/blob/humble-devel/spinnaker_camera_driver/doc/index.rst]
