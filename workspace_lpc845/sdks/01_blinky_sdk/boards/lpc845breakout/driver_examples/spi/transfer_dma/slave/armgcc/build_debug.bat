<<<<<<< HEAD
if exist CMakeFiles (RD /s /Q CMakeFiles)
if exist Makefile (DEL /s /Q /F Makefile)
if exist build.ninja (DEL /s /Q /F build.ninja)
if exist cmake_install.cmake (DEL /s /Q /F cmake_install.cmake)
if exist CMakeCache.txt (DEL /s /Q /F CMakeCache.txt)
cmake -DCMAKE_TOOLCHAIN_FILE="../../../../../../../tools/cmake_toolchain_files/armgcc.cmake" -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=debug  .
mingw32-make -j 2> build_log.txt 
=======
if exist CMakeFiles (RD /s /Q CMakeFiles)
if exist Makefile (DEL /s /Q /F Makefile)
if exist build.ninja (DEL /s /Q /F build.ninja)
if exist cmake_install.cmake (DEL /s /Q /F cmake_install.cmake)
if exist CMakeCache.txt (DEL /s /Q /F CMakeCache.txt)
cmake -DCMAKE_TOOLCHAIN_FILE="../../../../../../../tools/cmake_toolchain_files/armgcc.cmake" -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=debug  .
mingw32-make -j 2> build_log.txt 
>>>>>>> 499c6fc4aa5e186623e87db8d36a35287ffd5cbb
