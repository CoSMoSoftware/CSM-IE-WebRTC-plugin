# CSM-IE-WebRTC-plugin
CoSMo Open Source Webrtc Plugin for IE 11

One can find a precompiled version of the plugin here
TODO: [here](https://drive.google.com/drive/folders/1Gkcg_94VnM0h84ZCJH9XbFyKRSm65wqb?usp=sharing)
Alternatively, you can compile the plugin following the instructions below.

One need to register the plugin as follows (with administrator rights):
```
regsvr32 /i WebRTCPlugin.dll
```

You can check if the plugin has been registered correctly, by running the following command in IE11 console with ActiveX enabled:
```
WebRTCProxy = new ActiveXObject("Cosmo.WebRTCProxy.1");
```

You can debug ActiveX Plugins (with MSVC 2019) this way:
https://docs.microsoft.com/en-us/visualstudio/debugger/how-to-debug-an-activex-control?view=vs-2019

then on to the [JS shim](https://github.com/CoSMoSoftware/CSM-IE-WebRTC-plugin-shim) 

## Compiling The code

### Libwebrtc Dependency

Compiling this code requires a 32 bits build of libwebrtc branch head 73.
It has been developped and teted with MSVC 2019 (16.2)

#### Libwebrtc Installer
TODO: Corresponding installer can be found
[here](https://cosmosoftware.io).

#### libwebrtc Compilation from source
For those familiar with the GN build system, and already know how to build a specific branch head (m73), here are the arguments we pass to the "gn gen" command.

treat_warnings_as_errors = false
is_debug = true
enable_iterator_debugging = true
target_cpu = "x86"
use_rtti = true



