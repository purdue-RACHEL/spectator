# spectator
## Code Structure
### GamePowerup
The main class. This class contains startup functions and starts the two main threads
### GameLoop
This will contain all game logic code, and call Projector and UartDecoder according to the game's rules.
### CameraLoop
This will contain all the camera ball tacking logic. Will call CameraInterface to get RGB data from camera.
### CameraInterface
Wrapper interface for OpenNI2 camera interaction.
### UartDecoder
Wrapper interface for interacting with FT232RL device.
### Projector
Wrapper interface for displaying information to a popup window. Uses OpenCV.

IN PROGRESS:

Update Score: Write rotated text to the red and blue side of the table