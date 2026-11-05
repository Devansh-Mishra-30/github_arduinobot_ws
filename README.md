# ROS 2 ArduinoBot Manipulator

A ROS 2 workspace for a 3-DoF articulated arm with a servo gripper. The same
URDF/Xacro model and trajectory-control interfaces connect a Gazebo simulation
to an Arduino-driven physical prototype through MoveIt 2, `ros2_control`, and a
custom serial hardware interface.

> **Project status:** portfolio prototype. The repository contains simulation
> and hardware-integration code, but this documentation sprint did not rerun a
> clean simulation or physical-hardware acceptance test. No quantitative
> accuracy, repeatability, or production-readiness claim is made.

## What this repository demonstrates

- A mesh-based robot description assembled with URDF and Xacro.
- Gazebo Sim integration through `ros_gz_sim`, `ros_gz_bridge`, and
  `gz_ros2_control`/`ign_ros2_control`.
- Position trajectory controllers for three arm joints and a gripper.
- MoveIt 2 planning groups, KDL position-only inverse kinematics, and OMPL.
- A custom `hardware_interface::SystemInterface` that converts radians to
  servo-angle serial commands.
- Arduino firmware for four servos at 115200 baud.
- A custom ROS 2 action used by C++/Python task servers and an experimental
  voice-assistant adapter.

## Architecture

```text
Task client / experimental voice adapter
                  |
                  |  /task_server (ArduinobotTask action)
                  v
        C++ or Python task server
                  |
                  |  MoveGroupInterface: arm + gripper
                  v
         MoveIt 2 move_group / OMPL
                  |
                  |  FollowJointTrajectory actions
          +-------+----------------+
          |                        |
          v                        v
 arm_controller              gripper_controller
 (joint_1..joint_3)          (joint_4; joint_5 mimics)
          +-----------+------------+
                      |
              controller_manager
                      |
           +----------+-----------+
           |                      |
           v                      v
  gz_ros2_control          ArduinobotInterface
  Gazebo Sim               /dev/ttyACM0 @ 115200
           |                      |
           v                      v
  simulated joints         robot_control.ino
                                  |
                     servo signal pins 8..11

URDF/Xacro -> robot_state_publisher -> TF and robot model
URDF/SRDF  -> MoveIt 2             -> planning model and groups
```

The simulation and hardware paths share joint names and controller-facing
trajectory interfaces. They differ only below `ros2_control`: Gazebo supplies
the simulated system, while `ArduinobotInterface` serializes hardware commands
for the Arduino firmware.

## Package map

| Package | Responsibility |
| --- | --- |
| `arduinobot_bringup` | Includes Gazebo, controllers, MoveIt 2, and the remote interface for the intended all-in-one simulation launch. |
| `arduinobot_description` | URDF/Xacro, STL meshes, RViz configuration, Gazebo plugins, and robot spawning. |
| `arduinobot_controller` | Controller configuration and the custom serial `ros2_control` hardware plugin. |
| `arduinobot_moveit` | SRDF, planning groups, KDL kinematics, joint limits, OMPL, and trajectory-controller mapping. |
| `arduinobot_firmware` | Arduino sketches plus small ROS 2 serial examples. |
| `arduinobot_msgs` | Custom action and service definitions. |
| `arduinobot_remote` | C++/Python task servers and an experimental Alexa/Flask adapter. |
| `arduinobot_cpp_examples` | C++ ROS 2 communication, lifecycle, action, and MoveIt examples. |
| `arduinobot_py_examples` | Python equivalents of the ROS 2 examples. |
| `arduinobot_utils` | Euler/quaternion conversion services. |

## Prerequisites

The repository does not pin a single ROS 2 distribution or provide a lockfile.
Use a ROS 2 installation with matching MoveIt 2, Gazebo Sim, `ros2_control`, and
`ros2_controllers` packages. The launch files select the older Ignition plugin
name for Humble and the Gazebo plugin name for later distributions.

Additional host requirements include:

- `colcon` and `rosdep`;
- a C++17-capable compiler and CMake;
- `libserial-dev` for the physical hardware interface;
- an Arduino-compatible board and the Arduino Servo library for hardware use;
- a working graphics/display environment for Gazebo and RViz.

## Build

From the workspace root:

```bash
source /opt/ros/$ROS_DISTRO/setup.bash

rosdep install --from-paths src --ignore-src --rosdistro "$ROS_DISTRO" -r -y
colcon build --symlink-install
source install/setup.bash
```

Re-source `install/setup.bash` in every new terminal.

## Launch

### Inspect the model in RViz

```bash
ros2 launch arduinobot_description display.launch.py
```

This starts `joint_state_publisher_gui`, `robot_state_publisher`, and RViz
without commanding Gazebo or physical hardware.

### Simulation

The intended aggregate entry point is:

```bash
ros2 launch arduinobot_bringup simulated_robot.launch.py
```

It includes the Gazebo world and robot spawn, controller spawners, MoveIt 2,
RViz, and the remote task interface. For component-level diagnosis, launch the
same pieces separately:

```bash
# Terminal 1
ros2 launch arduinobot_description gazebo.launch.py

# Terminal 2
ros2 launch arduinobot_controller controller.launch.py is_sim:=true

# Terminal 3
ros2 launch arduinobot_moveit moveit.launch.py is_sim:=true

# Terminal 4 (optional task interface)
ros2 launch arduinobot_remote remote_interface.launch.py is_sim:=true
```

The aggregate path is present in source but should be treated as needing a
fresh integration pass; see [Known limitations](#known-limitations).

### Physical hardware

1. Review the wiring and power checklist below.
2. Flash
   `src/arduinobot_firmware/firmware/robot_control/robot_control.ino`.
3. Confirm the serial device and update the Xacro `port` parameter if it is not
   `/dev/ttyACM0`.
4. Start the hardware controller, then MoveIt 2, then the optional task server:

```bash
# Terminal 1
ros2 launch arduinobot_controller controller.launch.py is_sim:=false

# Terminal 2
ros2 launch arduinobot_moveit moveit.launch.py is_sim:=false

# Terminal 3 (optional)
ros2 launch arduinobot_remote remote_interface.launch.py is_sim:=false
```

Do not run the hardware path until the current plugin metadata and controller
mapping issues listed below have been resolved and tested on a de-energized or
mechanically restrained prototype.

## Hardware and firmware interface

The checked-in firmware uses this signal assignment:

| Function | Arduino signal pin | Startup command |
| --- | ---: | ---: |
| Base servo | 8 | 90 degrees |
| Shoulder servo | 9 | 90 degrees |
| Elbow servo | 10 | 90 degrees |
| Gripper servo | 11 | 0 degrees |

The host sends comma-terminated ASCII fields at 115200 baud:

```text
bDDD,sDDD,eDDD,gDDD,
```

`b`, `s`, `e`, and `g` select the base, shoulder, elbow, and gripper; `DDD` is a
zero-padded servo angle. The C++ hardware interface performs the joint-radian to
servo-degree conversion before writing the message.

### Electrical and motion safety checklist

- Power the servo rail from a supply sized for the actual servos; do not assume
  the board USB rail can supply stall current.
- Connect the servo supply ground and Arduino ground together before applying
  command signals.
- Verify the board, supply voltage, polarity, pinout, and current limit against
  the component datasheets.
- Test one disconnected or mechanically unloaded joint at a time.
- Establish physical travel limits and a reachable power disconnect before
  enabling coordinated motion.
- Keep people and fragile objects outside the manipulator workspace.

This repository does not implement a safety-rated E-stop, guarding, torque
limiting, or independent joint-limit enforcement.

## Demonstration and media

No local PNG, JPEG, GIF, or video demonstration assets were present during the
documentation audit.

- **Media placeholder - simulation:** add an RViz/Gazebo image showing the
  planned and executed manipulator pose.
- **Media placeholder - hardware:** add a clear prototype photo and a short
  simulation-to-hardware task video.
- **Media placeholder - wiring:** add a reviewed power and signal diagram with
  board and servo part numbers.

Do not add performance or accuracy captions until the corresponding logs and
test conditions are checked into the repository.

## Validation checklist

After resolving the known integration issues, record the ROS distribution,
dependency versions, board, servos, supply, and commit SHA, then run:

```bash
colcon test --event-handlers console_direct+
colcon test-result --verbose

ros2 control list_controllers
ros2 topic echo /joint_states --once
ros2 action list
```

For the task server, goals `0`, `1`, and `2` map to the three joint-space task
definitions currently implemented in `task_server.cpp`:

```bash
ros2 action send_goal /task_server \
  arduinobot_msgs/action/ArduinobotTask \
  "{task_number: 0}"
```

Capture at minimum:

- controller activation and action-server availability;
- planned versus commanded joint positions;
- physical joint observations from an independent measurement source;
- repeated-cycle success/failure counts;
- serial disconnect, invalid goal, cancellation, and power-cycle behavior.

No clean-build, launch, planning, or hardware result is claimed by this README
until that evidence is recorded.

## Known limitations

- The hardware `read()` path mirrors commanded positions; there is no encoder
  or servo feedback, so reported joint state is not measured joint position.
- The serial protocol has no acknowledgement, checksum, sequence number,
  timeout, or firmware-to-host fault report.
- The hardware port is hard-coded to `/dev/ttyACM0` in Xacro.
- The plugin XML type name does not currently match the capitalization of the
  exported C++ class and requires correction before hardware loading is trusted.
- The top-level Xacro consumes `is_sim` in included files without declaring it
  alongside `is_ignition`; launch-time Xacro expansion needs verification.
- MoveIt lists `joint_4` and mimic `joint_5` for the gripper controller, while
  the `ros2_control` controller configuration commands only `joint_4`. This
  mapping needs an integration test and may explain prior MoveIt errors.
- The task server implements goals `0`-`2`; the experimental voice adapter also
  sends goal `3`, which the C++ server rejects.
- Package manifests still contain placeholder descriptions/licenses, and no CI
  workflow is present.
- No quantitative tracking, repeatability, payload, or durability study is
  checked in.

## Future improvements

1. Resolve and test Xacro, plugin metadata, and gripper-controller mappings.
2. Add deterministic CI for build, lint, Xacro expansion, and launch smoke tests.
3. Add measured joint feedback or clearly separate commanded and observed state.
4. Harden the serial protocol with acknowledgements, framing, timeouts, and
   fault propagation.
5. Parameterize the device path and document udev rules.
6. Add calibrated joint limits, homing, cancellation, and safe-stop behavior.
7. Publish reproducible simulation and hardware test evidence with reviewed
   media.

## Scope

This is an engineering-learning and portfolio workspace, not a production or
safety-certified manipulator controller.
