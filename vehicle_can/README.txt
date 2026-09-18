#------------- CAN DRIVERS ---------------------

    cd ~/workspace_folder
    source install/setup.bash
    ros2 launch can_drivers can_parser_launch.py can_id:=0

# Para vizualiar de forma gráfica (Instale o plotjuggler) rode o comando abaixo e coloque os topicos publicados:

    cd ~/workspace_folder
    source install/setup.bash
    ros2 run plotjuggler plotjuggler
