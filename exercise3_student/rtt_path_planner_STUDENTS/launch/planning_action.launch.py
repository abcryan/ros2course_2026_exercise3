import launch
import launch_ros.actions
from launch_ros.parameter_descriptions import ParameterFile
from ament_index_python.packages import get_package_share_directory
import os

def generate_launch_description():
    # Get the path to params.yaml
    param_file = os.path.join(get_package_share_directory('rrt_path_planner'), 'config', 'params.yaml')

    return launch.LaunchDescription([
        # TASK 1 TODO: Load parameters somewhere below

        # Action Server
        launch_ros.actions.Node(
            package="rrt_path_planner",
            executable="planning_action_server",
            name="rrt_path_planner_server",
            output="screen",
        ),

        # RViz
        launch_ros.actions.Node(
            package="rviz2",
            executable="rviz2",
            name="rviz",
            output="screen",
            arguments=["-d", os.path.join(get_package_share_directory("rrt_path_planner"), "rviz", "planning.rviz")]
        ),

        # Action Client
        launch_ros.actions.Node(
            package="rrt_path_planner",
            executable="planning_action_client",
            name="rrt_path_planner_client",
            output="screen"
        ),

        # Visualizer
        launch_ros.actions.Node(
            package="rrt_path_planner",
            executable="planning_action_visualizer",
            name="rrt_path_planner_visualizer",
            output="screen"
        ),
    ])
