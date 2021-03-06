# Copernicus Space Simulator
This repository contains one of my projects involving 3D space simulation by using OpenGL and C. It has been built in in 2009-2010 given my passion for astronomy. Since I was in highschool I loved simulating space. I started in Pascal with my own 3D simulation procedures then moved on to C# with DirectX and finally C with OpenGL. The simulator still has some bugs and TODOs but otherwise it is functional allowing you to move around the solar system and nearby stars. It has lens flares from the Sun, collision detection, billboards for deep sky objects, and clouds over Earth. It displays a lot of asteroids and moons, many as 3D objects taken from public repositories. Given the long time I cannot remember all sources for data and models. It should run in both Windows and Linux.

Back when I was starting to develop this, software like Celestia or OpenUniverse was being developed.

If you are interesting in seeing how a simple simulator can be built (no shaders and shadows) you are invited to have a look.

The code I have here has been unchanged since 2010.

## Windows binary

The repository contains a precompiled binary part of the *copernicus-windows-binary.zip* file that should run on Windows 10.

## Linux

The executable for Linux is the file *copernicus.elf*. Unzip the images.zip archive in an images folder before running the application.

You can compile the entire codebase by running the *makefile*.

## Commands

You can use the mouse to look around and keys to perform various actions. You can find the commands in *copernicus.cpp*.
