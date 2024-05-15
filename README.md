# GCS Dll Importer
This project is made to check all DLL dependencies for PE files

## Dependencies
1. OC Microsoft Windows
2. CMake
3. C++ Compiler (C++17 is required)

## Important things to know
The application was tested on OC Microsoft Windows 10 21H2

## Build
```
git clone https://github.com/Gaymocoder/GCSDllImporter.git
cd GCSDllImporter
mkdir build && cd build
cmake ..
cmake --build .
```

## Using
After successful build or installation the application will be placed in "GCSDllImporter/bin" directory
1. Add path to "bin" folder into PATH environment variable;
2. Run it from cmd as "importDll <path-to-your-app>";
3. Run your app, which path you've entered to "importDll"-command;
4. To make sure that all neccecary dll-s were detected, run every runable thing in your app;
5. Close your app;
6. In cmd, where you've entered "importDll"-command, will be printed the list of dll-files which were ever loaded by your app from it's start to it's exit.