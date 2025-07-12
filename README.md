<h1>Hook The Fish</h1>
<h2>Authors</h2>
Shadman Zaman Sajid<br>
Md. Abdullah Bin Sorwar Chowdhury<br>
Tahjir Tansim<br>
Arnob Saha
<h2>Institution</h2>
University of Dhaka
<h2>Purpose</h2>
University Project
<h2>Language</h2>
C++
<h2>Third-Party Libraries</h2>
SDL2 (Simple Directmedia Layer 2)
<h2>Game Structure</h2>
<h3>Menus</h3>
<ol>
  <li>New Game<ul>
    <li>Easy<ul>
      <li>Loading Screen</li>
      <li>Interface</li>
      <li>Pause Menu</li>
    </ul></li>
    <li>Medium<ul>
      <li>Loading Screen</li>
      <li>Interface</li>
      <li>Pause Menu</li>
    </ul></li>
    <li>Hard<ul>
      <li>Loading Screen</li>
      <li>Interface</li>
      <li>Pause Menu</li>
    </ul></li>
  </ul></li>
  <li>High Scores<ul>
    <li>Easy</li>
    <li>Medium</li>
    <li>Hard</li>
  </ul></li>
  <li>Settings<ul>
    <li>Controls</li>
    <li>Sounds</li>
    <li>How To Play</li>
    <li>Weather</li>
  </ul></li>
  <li>About</li>
  <li>Exit</li>
</ol>
<h3>Functionalities</h3>
<ul>
  <li>Flashing Effects while Button Interaction</li>
  <li>Main Menu Fish reaction while Mouse Interaction</li>
  <li>Auto Sound/Weather Update Save</li>
  <li>Dynamic High Score Update</li>
  <li>Two Different Playable Weather: Sunny and Rainy</li>
  <li>Playable Modes: Easy, Medium, Hard; each having different play conditions</li>
  <li>In-Game Pause Menu to naturally Quit Game, Sound On/Off or going through Game Rules</li>
  <li>In-Game Randomized Objective Fishes</li>
</ul>
<h2>Compile Code</h2>
g++ main.cpp Common.cpp Exit.cpp Settings.cpp HighScores.cpp GameRules.cpp Controls.cpp Weather.cpp NewGame.cpp HardInterface.cpp MediumInterface.cpp EasyInterface.cpp Loading.cpp Pause.cpp Rain.cpp GameOver.cpp About.cpp -o Game -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lSDL2_gfx -ldl
<h2>Dependencies</h2>
<ul>
  <li>SDL2</li>
  <li>SDL2_ttf</li>
  <li>SDL2_mixer</li>
  <li>SDL2_image</li>
  <li>SDL2_gfx</li>
</ul>
