# LocalChatCPP
A chatting application using a server-client architecture

## Building
To build this, you must have Visual Studio installed (v17 2022 preferred), as well as conan installed.
(To install conan, refer <a href="https://docs.conan.io/1/installation.html">this</a>)
Refer the following instructions to build this project
* Clone the repository. Perform a recursive clone (--recurse-submodule) to also clone the submodules used by this project
* This project depends on Windows Socket Library as well as OpenGL. So you would need an environment initialized by running `vcvarsall.bat [arch]` for your desired architecture to have the locations of these libraries loaded in the environment. Only then can you run CMake and properly generate your project files.
* <p>If you prefer command line environment and would like to do things from there.. then look up the path of `vcvarsall.bat` on your system <a href="https://stackoverflow.com/questions/55097222/vcvarsall-bat-for-visual-studio-2019">here</a>, and run it in cmd. <b>(Assuming you already have a proper C++ development environment setup in Visual Studio)</b></p></p>
* Look up the `CMakePresets.json` file. There under `configurePresets` you can find a list of configurations available for building the project. Choose which preset you want. Let's discuss the steps to build the project by taking an example of a build configuration `x64-release`
### Steps
* ##### Using a command line interface
1) Launch a cmd terminal and run `vcvarsall.bat` according to the environment you want to build.
   ```
   %VCVARSALL_DIR_PATH%\vcvarsall.bat amd64
   ```
2) Then create directories in the following order
   ```
   mkdir out
   mkdir out\build
   mkdir out\build\x64-release
   cd out\build\x64-release
   ```
3) Now run CMake
   ```
   cmake ..\..\..\ -G "Visual Studio 17 2022"
   ```
4) Wait for the project file generation to finish
5) After the project files generation is completed, enter the following to build the complete project
   ```
   cmake --build .
   ```
6) The generated binaries would be in the `bin` directory, which would be created under your current (x64-release) directory

* ##### Using Visual Studio GUI
1) Open the project folder you cloned inside Visual Studio.
2) Wait for Visual Studio to recognize the CMake project and finish its work. It would give some errors as we haven't installed conan in the repository yet.
3) Select the build configuration as x64-Release from the dropdown above in Visual Studio and then open Developer Power Shell, and type in the following commands
   ```
   cd out\build\x64-release
   conan install ..\..\..\
   ```
4) Now CMake should again try to regenerate project files automatically. If it doesn't happen, then restart Visual Studio.
5) Now choose any project (LCServer/LCClient) and build it. Note: You would probably need both LCServer and LCClient to be built. Don't worry about the other options shown to you.. they would be automatically build while building either of these two projects.
   
