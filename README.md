# CSM-IE-WebRTC-plugin
CoSMo Open Source Webrtc Plugin for IE 11

One can find a precompiled version of the plugin here
TODO: [here](https://drive.google.com/drive/folders/1Gkcg_94VnM0h84ZCJH9XbFyKRSm65wqb?usp=sharing)
Alternatively, you can compile the plugin following the instructions below.

One need to register the plugin as follows (with administrator rights):
```
regsvr32 /i WebRTCPlugin.dll
```

You can check if the plugin has been registered correctly, by runnign the following command in IE11 console with ActiveX enabled:
```
WebRTCProxy = new ActiveXObject("Cosmo.WebRTCProxy.1");
```

then on to the [JS shim](https://github.com/CoSMoSoftware/CSM-IE-WebRTC-plugin-shim) 

## Compiling The code

### Libwebrtc Dependency

Compiling this code requires a 32 bits build of libwebrtc branch head 73.
It has been developped and teted with MSVC 2019 (16.2)

#### Libwebrtc Installer
TODO: Corresponding installer can be found
[here](https://cosmosoftware.io).

#### libwebrtc Compilation from source
For those familiar with the GN build system,and already knwo how to build a specific branch head, here are the arguments we pass to the "gn gen" command.

- target_cpu = "x86"
- treat_warnings_as_errors=false
- disable_libfuzzer=true
- is_win_fastlink=true
- msvc_use_absolute_paths=true
- symbol_level=2
- use_rtti=true



