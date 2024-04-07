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
After successful build the application will be placed in "GCSDllImporter/bin" directory