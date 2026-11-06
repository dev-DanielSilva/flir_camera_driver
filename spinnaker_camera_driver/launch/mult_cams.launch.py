from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():
    driver_launch_path = PathJoinSubstitution(
        [FindPackageShare('spinnaker_camera_driver'), 'launch', 'driver_node.launch.py']
    )

    # Câmera A(Serial: 23015084)
    camA = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(driver_launch_path),
        launch_arguments={
            'camera_name': 'flir_cam_A',
            'serial': "'23015084'",
        }.items()
    )

    # Câmera B (Serial: 24031513)
    camB = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(driver_launch_path),
        launch_arguments={
            'camera_name': 'flir_cam_B',
            'serial': "'24031513'",
        }.items()
    )

    # Câmera C (Serial: 24065588)
    camC = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(driver_launch_path),
        launch_arguments={
            'camera_name': 'flir_cam_C',
            'serial': "'24065588'",
        }.items()
    )
    

    return LaunchDescription([camA, camB, camC])