# ASSIGNMENT 1 - RT1

This project focuses on controlling two turtles within the Turtlesim environment using two primary ROS 2 nodes written in cpp: UI and Distance.
These two nodes continuously monitor the turtles' movement to ensure safety and control:
- the user can select which turtle to move and at what velocity;
- if a turtle gets too close to the other, it automatically reverses its movement direction to back away;
- if a turtle approaches the boundary of the environment, it automatically reverses its movement
<br>
<div align="center">
  <img src="/image_videoReadMe/visualization_game.png" alt="Visualization Game" width="500"/>
</div>

<br>**ARCHITECTURE AND NODES**
<br>The simulation is managed by a total of 4 nodes, launched simultaneously via a single launch file (`launch_nodes.py`) located in the `/launch` directory.

**1) turtlesim_node**: the main ROS node providing the graphical environment and the default turtle1.

**2) turtle_spawn.py**: a python script that spawns a second turtle, turtle2, into the Turtlesim environment.

**3) ui_node (InputController)**: it's the node that manages user input and is responsible for communicating motion commands. This node:
- allows the user to select which turtle to control(1 $\rightarrow$ turtle1; 2 $\rightarrow$ turtle2) and define the linear and angular velocity to apply;
- sends the velocity command for 1 second over an intermediate topic (not directly to `/cmd_vel`). The node then automatically stops the movement by publishing a zero velocity on the same `/intermediate_vel` topic;
- blocks the acquisition of new user input if it receives a waiting command from the distance_node on `/is_reversing` topic, indicating that the turtle is in a forced reversing state.

**4) distance_node (DistanceController)**: it's the node that implements the safety logic of the system:
- it calculates the distance between the two turtles and publishes it to the `/distance topic`, also printing it to the terminal as a form of "log file";
- it continuously checks the position of the turtles on `/turtlex_pose` topic, specifically monitoring if the moving turtle:
  - is too close to the environment boundaries (if x or y > 10.0 or if x or y < 1.0);
  - is too close to the other turtle (within a range of 1.0 unit).
- if a "danger" position is detected:
  - it communicates to the ui_node to stop requesting user input;
  - it reverses the turtle's direction of movement until it exits the forbidden area;
  - it stops the turtle;
  - it updates the `/is_reversing` topic, allowing the ui_node to resume requesting user input.
<br>
 <div align="center">
  <img src="/image_videoReadMe/TurtlesMove.gif" alt="Dimostrazione Simulazione ROS 2" width="500"/>
</div>


<br>**HOW TO RUN THE SIMULATION**
<br>To launch the entire simulation, including `turtlesim_node`, `turtle_spawn.py`, `ui_node`, and `distance_node`, just use the `launch_nodes.py` file. 
<br>First, to compile pck_assignment1 package within your ROS 2 workspace:
```bash
colcon build
```
and after
```bash
source install/setup.bash
```

<br>In a terminal launch the nodes using the command:
```bash
ros2 launch pck_assignment1 launch_nodes.py
```
<br>This command will open three separate windows using xterm.

Since the launch file utilizes the xterm -e prefix to open nodes in separate windows, you must have xterm installed on your system. 
<br>If the launch fails, execute the following commands
```bash 
sudo apt update
```
```bash
sudo apt install xterm
```

In case the launch file does not work as expected, you can manually launch the nodes in four separate terminal windows, in this order:
1) ```bash
   ros2 run turtlesim turtlesim_node
2) ```bash
   run pck_assignment1 turtle_spawn.py
3) ```bash
   ros2 run pck_assignment1 ui
4) ```bash
   ros2 run pck_assignment1 distance
 
