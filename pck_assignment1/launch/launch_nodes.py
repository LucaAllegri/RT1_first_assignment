# launch_turtles.py
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    
    #TURTLESIM NODE
    turtlesim_node = Node(
        package='turtlesim',
        executable='turtlesim_node',
        name='turtlesim_gui',
        output='screen'
    )

    #TURTLE SPAWN NODE
    turtle_spawn_node = Node(
        package='pck_assignment1',
        executable='turtle_spawn.py', 
        name='turtle_spawn',
        output='screen',
    )

    #TURTLE 3 SPAWN NODE
    turtle3_spawn_node = Node(
        package='pck_assignment1',
        executable='turtle3_spawn.py', 
        name='turtle3_spawn',
        output='screen',
    )

    #UI NODE
    input_controller_node = Node(
        package='pck_assignment1',
        executable='ui', 
        name='ui_node',
        output='screen',
        prefix='xterm -e'
    )
    
    #DISTANCE NODE
    distance_controller_node = Node(
        package='pck_assignment1', 
        executable='distance',
        name='distance_node',
        output='screen', 
        prefix='xterm -e'
    )

    return LaunchDescription([
        turtlesim_node,
        turtle_spawn_node,
        turtle3_spawn_node,
        input_controller_node,
        distance_controller_node
    ])