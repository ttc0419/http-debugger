#!/bin/zsh
QT_PATH='/Users/william/Tools/QT'
QT_VERSION='6.3.0'
ARCH='x86_64;arm64'

rm -rf build

cmake -DCMAKE_BUILD_TYPE='Release' -DCMAKE_OSX_ARCHITECTURES=$ARCH -DCMAKE_PREFIX_PATH="$QT_PATH/$QT_VERSION/macos/" -S ../ -B build -G Ninja

cd build
ninja

mv http-debugger.app 'HTTP Debugger.app'
macdeployqt 'HTTP Debugger.app' -dmg

mv 'HTTP Debugger.dmg' ../
cd ..
rm -rf build
