# ML.Colorimeter

This a project for controlling a Colorimeter instrument used for measuring a pair of AR lenses.

+ add visualStudio.gitignore
  https://github.com/github/gitignore/blob/main/VisualStudio.gitignore

## Getting Started 
1. download git lfs to upload and pull large file. (DEPRECATED)
https://git-lfs.github.com/

2. initialize Git LFS (DEPRECATED)
```
git lfs install
```

3. Tracking large files, replace "*.dll" with the pattern of the file type you want to track (DEPRECATED)
```
git lfs track "*.dll"
```

4. Add and submit large files (DEPRECATED)
```
git add opencv_world451.dll
git commit -m "add large file using git lfs"
```

5. Add opencv_world451d.dll

Before you run the project using debug, you should add ``opencv_world451d.dll`` in the path "src\extlib\opencv4.5.1\build\x64\vc15\bin_\".

## Build Instructions

### Visual Studio

Install:

+ Visual Studio 2019
+ Visual Studio components:
    + qt-vsaddin

### QT

+ Install QT5.15.2

### PI API

+ Download the PI software suit from here:
   https://www.pi-usa.us/en/products/software-suite

### FilterWheel Drivers
+ Download Modbus USB to UART Bridge VCP Drivers:
   https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads


