![Logo](/doc/img/title.svg)

# HTTP Debugger
A cross-platform postman like tool to debug your http servers.
<p align="center">
  <img src="/doc/img/demo.png">
</p>

## Download
|                                              Windows (x86_64)                                               |                                  MacOS (x86_64 and arm64 Universal Binary)                                  |  Linux   |
|:-----------------------------------------------------------------------------------------------------------:|:-----------------------------------------------------------------------------------------------------------:|:--------:|
| [http-debugger.zip](https://github.com/Galaxy0419/http-debugger/releases/download/v1.0.0/http-debugger.zip) | [HTTP.Debugger.dmg](https://github.com/Galaxy0419/http-debugger/releases/download/v1.0.0/HTTP.Debugger.dmg) | N/A, WIP |

## Software Stack
|  C++  |  QT   |
|:-----:|:-----:|
| C++17 | 6.3.0 |

## Build Steps
### Windows
To build on Windows, you need to install QT and Visual Studio with "Desktop development with C++".
Then, change the ```QT_PATH``` and ```QT_VERSION``` variable to match your QT installation in the ```windows-deploy.bat```
script under the scripts folder.  After that, you can just click the script to build the app.

### MacOS
To build on MacOS, you need to install QT, clang and ninja.  You can get all of them using Homebrew or installing
command line tools to get apple version of clang.  Then, change the ```QT_PATH``` and ```QT_VERSION``` variable
to match your QT installation in the ```macos-deploy.sh```.  Finally, run the script, and it will
produce a dmg file called ```HTTP Debugger.dmg```

### Linux
The app should work on Linux, however, the packaging script is still WIP.

## Sponsor
It takes a lot of time to do researches.  If you think the project helped you, could you buy me a cup of coffee? 😉  
You can use any of the following methods to donate:  

| [![PayPal](/doc/img/paypal.svg)](https://www.paypal.com/paypalme/tianchentang)<br/>Click [here](https://www.paypal.com/paypalme/tianchentang) to donate | ![Wechat Pay](/doc/img/wechat.jpg)<br/>Wechat Pay | ![Alipay](/doc/img/alipay.jpg) Alipay |
|---------------------------------------------------------------------------------------------------------------------------------------------------------|---------------------------------------------------|---------------------------------------|
