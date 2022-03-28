<div align="center">
    <a href="https://github.com/zaklaus/neon86"><img src="https://user-images.githubusercontent.com/9026786/82740331-1db79c80-9d48-11ea-85b4-615d204f030a.png" alt="neon86" /></a>
</div>

<div align="center">
    <a href="https://discord.gg/eBQ4QHX"><img src="https://img.shields.io/discord/402098213114347520.svg" alt="Discord server" /></a>
    <a href="https://zaklaus.itch.io/neon-86"><img src="https://img.shields.io/badge/NEON86-Download%20on%20itch.io-red" alt="itch.io website" /></a>
</div>

<br />
<div align="center">
  A toy game engine reflecting on simplicity and retro approach to gamedev.
</div>

<div align="center">
  <sub>
    Brought to you by <a href="https://github.com/zaklaus">@zaklaus</a>
    and <a href="https://github.com/zaklaus/NEON86/graphs/contributors">contributors</a>
  </sub>
</div>

# Introduction

A toy game engine reflecting on simplicity and 
retro approach to gamedev. It is currently work in progress.

This game engine runs on Windows as a (used to be 32-bit) 64-bit executable. Renderer is backed by 
Direct3D9 and scripting is powered by Lua 5.3.

Engine is DCC-driven and supports model scene graphs and hierarchy manipulations. 

Engine also provides a hardware lighting support that is very simple to use, it also 
offers a shader support that is easily expendable and provides rich visuals at low costs.

## Compilation

You can use **main.bat** to quickly access and operate the engine workflow, there is also a solution file located in `code/neon86.sln` you can open to compile the project.

Debug build requires **d3dx9d_42.dll** to be present in your system (Which is part of the DirectX SDK February 2010 package), you can alternatively define `NEON_FORCE_D3DX9` to force the usage of redistributable DLLs instead, this is what the Release build uses by default and is used for shipping.

## Documentation

You can review NEON86 API by reading [the cheatsheet](docs/neon86_cheatsheet.pdf). It is still work in progress.

## License

This software is licensed under the **3-Clause BSD License**, see **LICENSE** file.
