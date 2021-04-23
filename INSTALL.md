Install
-------

Compile Raylib:

```bash
cd libs/raylib/src
make clean && make PLATFORM=PLATFORM_ANDROID ANDROID_ARCH=arm ANDROID_API_VERSION=30 ANDROID_HOME=/home/itarato/Downloads/cmdline-tools/bin ANDROID_NDK=/home/itarato/Downloads/android-ndk
```

Compile code:

```bash
make -f Makefile.Android.Linux clean && make -f Makefile.Android.Linux
```
