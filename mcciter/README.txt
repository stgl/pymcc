Building the Program
--------------------

1)  Read the file "../README.txt" about prerequisites.

2)  Generate the Project Files

    Run CMake to configure and generate the project files for the program.
    Configure the project to do an out-of-source build.  We recommend naming
    the binary directory after the platform and placing it in the MCC-LiDAR
    "build/" directory, for example:

      Source directory:  {MCC LiDAR root}/src

      Binary directory:  {MCC LiDAR root}/build/{platform}
               example:  {MCC LiDAR root}/build/os-x
               example:  {MCC LiDAR root}/build/windows

3)  If the Boost libraries are not installed in a standard location, then CMake
    may not find them when configuring the project.  If so, you'll have to
    specify their location with the "Boost_INCLUDE_DIR" property in the
    Advanced View of the CMake GUI.

    On Windows, CMake has trouble finding BoostPro installations where the
    subminor version numer is 0 (e.g., 1.39.0).  So, on Windows, set the
    environment variable BOOST_ROOT to the folder where Boost is installed,
    (e.g., "C:\Program Files\boost\boost_1_39").

4)  Build the Program

    On OS X, open the Xcode project file, for example,

      {MCC LiDAR root}/build/os-x/MCC_LIDAR.xcodeproj

    and click Build icon in the toolbar.

    On Windows, open the Visual Studio solution in the build directory, and
    build the solution.
