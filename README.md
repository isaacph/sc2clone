# Multiplayer OpenGL RTS Game
This project is a 3D multiplayer RTS game inspired primarily by StarCraft II.
# Game Features:
![Demo Image](https://github.com/isaacph/sc2clone/blob/images/demo.png)
- Simple placeholder graphics
- Log into a remote server and select a team color using a command line interface
- Three types of units: workers, soldiers, and barracks that are all different-sized cubes
- Workers can transform to barracks, soldiers can attack, and barracks can produce workers and soldiers.
- Soldiers can A-click to attack all enemies in their path
# Overall Features:
- Font rendering with Freetype and OpenGL
- 3D diffuse lighting using shaders
- 3D model OBJ file reading (currently only reading a cube model with normals)
- Face picking with both 3D lines and a frustum, implemented with algebra, used to select objects in 3D space with mouse clicks or mouse drags
- Multithreading in both the client and server application using posix threads
- Reuseable packet receiving and (async) packet sending thread code (used in both client and server application)
- UDP socket handling done using winsock2
- Simple unique identification of users using randomly generated 32-bit integers
- World logic class adapted to both client and server
