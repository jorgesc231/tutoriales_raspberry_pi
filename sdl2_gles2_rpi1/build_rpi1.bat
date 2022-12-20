REM Ruta por defecto de instalacion, cambiar si se instalo en otro lugar.
set SYSROOT=C:/SysGCC/arm-linux-gnueabihf/sysroot

arm-linux-gnueabihf-g++ -O2 rpi1_sdl2_gles2.cpp -o rpi1_sdl2_gles2 -I$SYSROOT/usr/local/include/SDL2 -I$SYSROOT/opt/vc/include -I$SYSROOT/opt/vc/include/interface/vcos/pthreads -I$SYSROOT/opt/vc/include/interface/vmcs_host/linux -D_REENTRANT -L$SYSROOT/usr/local/lib -L$SYSROOT/opt/vc/lib -Wl,-rpath,$SYSROOT/usr/local/lib -Wl,--enable-new-dtags -lbcm_host -lbrcmEGL -lbrcmGLESv2 -lSDL2 -lSDL2_image