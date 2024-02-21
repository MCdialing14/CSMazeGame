# CS Maze Game
A maze game I made using an Arduino kit. I used the LCD screen, joystick, 7-seg display, and passive buzzer in my kit to make the functional game. The player is given about 10 seconds, or how long the 8-bit song plays on the buzzer, to complete the maze without bumping any walls or running out of time. The LCD screen displays the instructions, the game, and the multiple ending screens.

### Screen Layouts
<header> START SCREEN </header>
<img src="/Pictures/MazeStart.PNG?raw=true" alt="Start Menu" height="350">
<p>The player can click down on the joystick button to enter the game.</p>

<header> INSTRUCTIONS SCREENS </header>
<img src="/Pictures/MazeInstructions1.PNG?raw=true" alt="Instructions" height="350">
<img src="/Pictures/MazeInstructions2.PNG?raw=true" alt="Instructions" height="350">
<p>These two screens run on a timer of 5 seconds each to give players enough time to read and understand how the maze works.</p>

<header> IN GAME SCREEN </header>
<img src="/Pictures/MazeInGame.PNG?raw=true" alt="In-game" height="350">
<p>The player has 10 seconds as the 8-bit song plays to reach the goal without bumping into walls.</p>

<header> GAME OVER SCREENS </header>
<img src="/Pictures/MazeGameOverWall.PNG?raw=true" alt="Game over by wall" height="350">
<img src="/Pictures/MazeGameOverTime.PNG?raw=true" alt="Game over by time" height="350">
<p>Ending screens for when players meet either of the game over conditions, hitting a wall, or running out of time. These are on a timer for 5 seconds and then lead back to the start screen.</p>

<header> GAME WIN SCREEN </header>
<img src="/Pictures/MazeWin.PNG?raw=true" alt="In-game" height="350">
<p>Win screen for when players reach the goal, this is on a timer for 5 seconds and then leads back to the start screen.</p>
