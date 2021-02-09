# CLX000CANBus
Plugin for the [Qt5 Serial Bus](https://doc.qt.io/qt-5/qtserialbus-index.html) module to allow for CL1000, CL2000 and CL3000 loggers from CSS Electronics to stream data into applications utilizing the Qt5 Serial Bus framework.

## Building
If not targeting a system-wide Qt installation, the vcpkg (integrated as a git submodule) can be utilized. Current configuration is only enabled for [CMake](https://cmake.org/).

### Building with vcpkg
First, bootstrap the [vcpkg](https://github.com/microsoft/vcpkg) installation in the submodule.

```
./vcpkg/bootstrap-vcpkg.sh
```

Create a folder for out-of-tree building, and configure cmake.

```
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake ..
```

During configuration, all required dependencies should be downloaded, built and installed locally in the `vcpkg` folder. After configuration, build the plugin with

```
cmake --build --target clx000canbus
```

## Installation

### System-wide usage
Copy the module to the system Qt5 installation into the `plugins/canbus` directory.

### Application local usage.
Create the same structure as above under the applications plugins folders, such that the module is present in `plugins_folder/canbus`. The application must have been built with the local path for loading modules.
