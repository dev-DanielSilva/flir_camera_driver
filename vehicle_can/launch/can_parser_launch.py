import os
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration, TextSubstitution

def generate_launch_description():
    # 1. Declara o argumento can_id
    can_id_arg = DeclareLaunchArgument(
        'can_id',
        default_value='0',
        description='ID do canal CAN para o parser'
    )

    can_id_val = LaunchConfiguration('can_id')

    # 2. Nó do Parser (C++)
    can_parser_node = Node(
        package='can_drivers',
        executable='can_node_exec',
        name=[TextSubstitution(text='can_node_'), can_id_val],
        output='screen',
        parameters=[{
            'can_id': can_id_val
        }]
    )

    # 4. Retorna a descrição com todos os nós
    return LaunchDescription([
        can_id_arg,
        can_parser_node,
    ])
