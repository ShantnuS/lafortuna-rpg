# FortunaRPG
RPG project for a LaFortuna device.   
The objective is to walk around defeating evil NPCs, and then finally overthrow the evil lava king.  

# Gameplay Instructions
Press the central button to start the game when on the start screen.     
Use the arrow buttons to walk around. The central button opens the inventory.     
A battle with an evil NPC starts when you collide with them.   
To win a battle, use attacks until their health runs out.   
To win the game, kill the evil lava king.    

# Compilation Instructions
If you have DFU-Programmer installed, you can open the 'src' folder and automatically build and upload the game using "make upload".  
If you want to manually flash the HEX file, simply use "make" to complile, then open the 'build' folder and flash the "src.hex" file.  

# MapMaker
The MapMaker was created by Alex especially for this project. It is coded in C++.   
The MapMaker allowed us to create the world for the game in a GUI. This was much easier than manually entering HEX data.   

# External Libraries and Resources
Header files from previous tasks.     
Modified version of Klaus's Makefile.    
OpenGL (in MapMaker).  
STB Image (in MapMaker).  
GDI Plus (in MapMaker).  


