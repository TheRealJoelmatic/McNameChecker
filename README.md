# 🌕 Minecraft Name Checker 🌕
This is the fastest free tool to check thousands of Minecraft names in minutes. This has a built-in proxy scraper and proxy checker. The best part is that it's made in c++ to have optimal performance.

# Pictures

![image](https://i.ibb.co/HKTwKtH/1.jpg)
#
![image](https://i.ibb.co/2y86pYq/2.jpg)
#
![image](https://i.ibb.co/tx3K6FJ/3.jpg)

# Getting started

## Download
Download the latest release at the [releases section](https://github.com/TheRealJoelmatic/Matic-Clicker/releases/tag/Release).

## Bugs or suggestions
Bugs or suggestions should be reported in the [issues section](https://github.com/TheRealJoelmatic/Matic-Clicker/issues).

## Credits
Joelmatic - https://github.com/TheRealJoelmatic

## Compiling
### Prerequisites
Microsoft Visual Studio (Preferably the latest version) with C++ installed.

### Compiling from the source
Open the solution file `McAccountChecker.sln`, then select `Release | x64` on the Build configuration and press Build solution.

### Compiling errors
You need curl and raidjosn to Compile this! I recommend using it to install the required dependencies.
- https://vcpkg.io/en/getting-started.html

## Errors
### vcruntime140.dll and msvcp140.dll errors
That happens because you don't have the runtime and libraries required for this to run.  
Please install preferably both `x86` and `x64` packages of the VC++ 2019 runtime linked below.
- https://aka.ms/vs/16/release/vc_redist.x86.exe
- https://aka.ms/vs/16/release/vc_redist.x64.exe

### d3d9.dll error
Install the following
- https://www.microsoft.com/en-us/download/confirmation.aspx?id=35
