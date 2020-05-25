# NEON86 Documentation

A toy game engine reflecting on simplicity and 
retro approach to gamedev. It is currently work in progress.

## Project setup

player.exe is responsible for loading the game data, the folder you want to load
can be provided as a command-line argument to player.exe, it uses folder called `data` by default.

If you use the quickstart template from itch.io, you are already set up to work with the engine.
The game folder always contains a file called `init.lua`, which contains the game code.
This is the file that defines the gameplay logic or visuals, the assets the game uses.

## The game flow

The game engine provides 4 events that provide all the necessary facalities to develop the game.

### _init() 
Called once the game is started. This is when you can set up all your resources and gameplay logic.

### _destroy()
Called when the game is ready to be shut down. 

### _update(dt) 
Called once per engine update. It can be called multiple times per engine tick to process
accumulated time to catch up with the real time. This allows for deterministic physics or timing to be made use of. 
This method also provides the `dt` argument providing the delta time between updates.

### _render()
Called once per engine update. This is where you render your visuals.

## Lua API

### Base

