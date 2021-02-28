# Shishkov M., 21.01.2021
# builds am64x library and reference projects
# Ubuntu 16.04, Debian 4.19, raspberrypi 4.19.75+
#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )" 
pushd $DIR

if ! [ -d "build/Release" ]; then
   mkdir -p build/Release
fi

 

#default considers ARM compilation
toolchain="/../_toolchains/lin_4_lin_arm.cmake"

case $( uname -m ) in
x86_64) toolchain="/../_toolchains/lin_4_lin.cmake";;
*)     echo ARM;;
esac

echo "Gulliversoft 2021, Copyright M. Shishkov manage with:" $toolchain

pushd ./build
echo "Build Product(s): am65x"
cmake \
-B ./Release/ \
-G Ninja \
-DPROJECT_ROOT:PATH=$(pwd)/.. \
-DCMAKE_TOOLCHAIN_FILE:FILEPATH=$(pwd)$toolchain \
-DCMAKE_MAKE_PROGRAM:FILEPATH=${ninja_program} \
-DCMAKE_BUILD_TYPE:STRING=Release \
..
popd 

pushd ./build/Release

ninja

popd 

popd 
