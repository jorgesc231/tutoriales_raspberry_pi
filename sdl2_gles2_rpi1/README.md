# Compilar SDL2 para la Raspberry Pi

Podría descargar el SDL2 del repositorio pero suele tener una versión antigua y no se como fue configurada (y no me funciono ...).  

En esta guía voy a dar los pasos de como compilar SDL2 para desarrollar en la Raspberry Pi, y como compilar desde Windows usando un cross-compiler.  

Compilare **SDL2, SDL2_image, SDL2_mixer y SDL2_ttf** que es lo necesario para compilar mis otros proyectos.  
   
Ademas hay un ejemplo de como usar SDL2 con OpenGL ES 2. 
  
_Lo dejo escrito porque en el futuro se me olvidara como hacerlo_  

## Requisitos  

Un un **PC con Windows 10** y una __Raspberry Pi 1 B__+ con __Raspberry Pi OS Lite__ _Debian version: 11 (bullseye)_ recien instalado.  

- Tener instalado **Git** en la Raspberry Pi y en Windows  
- Saber lo basico de C/C++ y como funciona el proceso de compilacion.  
- Conociminetos basicos de SDL2  
- (Opcional) Conociminetos basicos de OpenGL  


## Compilación cruzada

_Compilar en un computador distinto a en el que se va ejecutar el programa es hacer compilación cruzada. (cross-compile)._  
  
Compilare SDL2 en la Raspberry PI y un ejemplo de como usarlo para luego transferir la biblioteca compilada al PC con Windows para realizar el desarrollo y compilar el programa usando un cross-compiler.

**IMPORTANTE: En una raspberry pi 1 demora varias horas, así que es buena idea dejarla compilando mientras se hacen otras cosas.**  
  
Lo mejor seria compilar SDL2 y los programas en un PC con Linux y traspasar el programa compilado a la raspberry pero por limitaciones de mi PC no puedo instalar Linux usando maquinas virtuales o WSL.  
Ya lo intente un par de veces en Windows y hasta ahora no he podido compilarlo sin tener varios errores.  
Pero son problemas que solo se dan al compilar SDL, desarrollar programas que se enlacen a la biblioteca ya compilada funciona correctamente.

Esto también sirve como ejemplo de como se pueden compilar otras bibliotecas.  
**Todo esto se puede automatizar bastante.**  


## Descargar los drivers

Usare los drivers privativos porque funcionan mejor (por lo menos en la Raspi 1) que los de código abierto. 

_Talvez es solo un problema con la Raspberry Pi 1 y nada de esto es necesario en la 2, 3 o 4._
  
Las ultimas versiones de Rasperry Pi OS solo trae los de código abierto, por lo que tenemos que desactivarlos y descargar los privativos.  
  
**NOTA: En versiones superiores a la Raspi 1 los drivers de código abierto probablemente funcionen mejor, por lo que todo esto seria innecesario.**  
  
Primero hay que desactivar los drivers de código abierto. (_vc4 OpenGL driver._)

En el archivo `/boot/config.txt` de la Raspberry, comentar las lineas `dtoverlay=vc4-fkms-v3d` o `dtoverlay=vc4-kms-v3d` que estan casi al final del archivo.

Por defecto debería estar así:

```bash
#Enable DRM VC4 V3D driver
dtoverlay=vc4-kms-v3d
max_framebuffers=2
```

Cambiarlo para que quede así:

```bash
#Enable DRM VC4 V3D driver
#dtoverlay=vc4-kms-v3d
#max_framebuffers=2
```

Reiniciar la raspberry pi:

```bash
sudo reboot now
```


Mientras **en el PC** es necesario descargar el siguiente repositorio con los drivers:

```bash
https://github.com/raspberrypi/firmware
```

Lo que necesitamos esta en el directorio `firmware/hardp/`.

Pasarlo a la Raspberry Pi:

```bash
scp -r firmware/hardp/opt usuario@<ip_raspberry_pi>:/home/usuario
```

Reemplazar `usuario` e `ip_raspberry_pi` con los que corresponda.

Ahora **en la Raspberry Pi** copiarlos a donde deben estar:

```bash
sudo cp -r opt /
```

Con los drivers en el directorio correcto, SDL2 debería poder detectarlos cuando lo compilemos.  
  
También son necesarios si queremos compilar programas que usen OpenGL ES 2.


## Compilar SDL2 en la Raspberry Pi 1

Para empezar necesitamos las siguientes dependencias:

```bash
sudo apt install build-essential libfreeimage-dev libopenal-dev libudev-dev libasound2-dev libjpeg8-dev libtiff5-dev libwebp-dev automake
```


Descargar el codigo fuente de SDL2:


```bash
mkdir sdl_rpi1
wget https://github.com/libsdl-org/SDL/releases/download/release-2.26.1/SDL2-2.26.1.tar.gz
tar zxvf SDL-2.26.1.tar.gz
```

(El codigo esta disponible aqui: https://github.com/libsdl-org/SDL/releases)

Crear el directorio para los archivos de compilacion:

```bash
cd SDL-2.26.1
mkdir build
cd build
```

Configurar:

```bash
../configure --disable-pulseaudio --disable-esd --disable-video-wayland --disable-video-x11 --disable-video-opengl --disable-video-vulkan
```

Estas opciones desactivan cosas que no usaremos de SDL2, y dejan el soporte para aceleracion por hardware usando OpenGL ES 2 nativo con ejecucion en pantalla completa desde la terminal.

Si no hubo ningun error, ahora se puede compilar:

```bash
make -j 2
```

En la Raspberry Pi 1 usar 2 parece lo optimo, si se usan mas hilos, el sistema no tiene suficiente memoria y ralentizara la compilacion usando la memoria swap.

**NOTA: Esto va demorar como 15 minutos o mas**

 Instalar la biblioteca :

```bash
sudo make install
```
_(en /usr/local/lib)_


Ahora puedes compilar cualquier programa que use SDL asi:

```bash
g++ -o programa programa.cpp `sdl2-config --cflags --libs`
```

El comando `sdl2-config --cflags --libs` permite obtener como string las flags del compilador necesarias.

_Las demas bibliotecas parte de SDL se compilan de forma similar._


#### Compilar SDL_image

Volver al directorio que creamos antes __sdl_rpi1___.

Descargar el codigo fuente de SDL2_image:

```bash
cd sdl_rpi1
wget https://github.com/libsdl-org/SDL_image/releases/download/release-2.6.2/SDL2_image-2.6.2.tar.gz
tar zxvf SDL2_image-2.6.2.tar.gz
```

(El codigo esta disponible aqui: https://github.com/libsdl-org/SDL_image/releases)

Crear el directorio para los archivos de compilacion:

```bash
cd SDL2_image-2.6.2
mkdir build
cd build
```

Configurar:

```bash
../configure
```


Compilar e instalar:

```bash
make -j 2
sudo make install
```

Si tu programa SDL usa SDL_image hay que enlazarlo asi:

```bash
g++ -o programa programa.cpp `sdl2-config --cflags --libs` -SDL2 -lSDL2_image
```


#### Compilar SDL_mixer

Esta biblioteca permite cargar y reproducir audio.

Volver al directorio que creamos antes __sdl_rpi1___.

Descargar el codigo fuente de SDL2_mixer:

```bash
cd sdl_rpi1
wget https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.6.2/SDL2_mixer-2.6.2.tar.gz
tar zxvf SDL2_mixer-2.6.2.tar.gz
```

(El codigo esta disponible aqui: https://github.com/libsdl-org/SDL_mixer/releases)

Crear el directorio para los archivos de compilacion:

```bash
cd SDL2_mixer-2.6.2
mkdir build
cd build
```

Configurar:

```bash
../configure
```

Compilar e instalar:

```bash
make -j 2
sudo make install
```

Si tu programa SDL usa SDL_mixer hay que enlazarlo asi:

```bash
g++ -o program_name program_name.cpp `sdl2-config --cflags --libs` -lSDL2 -lSDL2_mixer
```


#### Compilar SDL_ttf

Esta biblioteca permite renderizar texto.

**NOTA: Esta biblioteca demora bastante mas que las otras, por lo menos 30min o 1 hora.**

Volver al directorio que creamos antes __sdl_rpi1___.

Descargar el codigo fuente de SDL2_ttf:

```bash
cd sdl_rpi1
wget https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.20.1/SDL2_ttf-2.20.1.tar.gz
tar zxvf SDL2_ttf-2.20.1.tar.gz
```

(El codigo esta disponible aqui: https://github.com/libsdl-org/SDL_mixer/releases)

Crear el directorio para los archivos de compilacion:

```bash
cd SDL2_ttf-2.20.1.tar.gz
mkdir build
cd build
```

Configurar:

```bash
../configure
```

Compilar e instalar:

```bash
make -j 2
sudo make install
```

Si tu programa SDL usa SDL_ttf hay que enlazarlo asi:

```bash
g++ -o program_name program_name.cpp `sdl2-config --cflags --libs` -lSDL2 -lSDL2_ttf
```


### Probar

Para probar que todo funciona correctamente, usare como prueba uno de mis proyectos que usa SDL2, SDL2_image, SDL2_mixer y SDL2_ttf:

https://github.com/jorgesc231/timberman

Clonar el repositorio del proyecto dentro de sdl_rpi1:

```bash
cd sdl_rpi1
git clone https://github.com/jorgesc231/timberman.git
```

Ingresar al directorio del proyecto:

```bash
cd timberman
```

Compilar con el siguiente comando:

```bash
g++ -O2 -o timberman Timberman.cpp `sdl2-config --cflags --libs` -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -DRPI1
```

Tambien se puede usar el script __build_rpi1.sh___ que esta en el repositorio.

Ejecutar el juego con:

```bash
./timberman
```


__Al ejecutar el juego van a salir los siguientes errores:__

```bash
timberman: error while loading shared libraries: libSDL2-2.0.so.0: cannot open shared object file: No such file or directory
```

Esto es por que el enlazador dinamico no encruenta la biblioteca de SDL2 (esto no seria problema si lo compilaramos de forma estatica).

o

```
No message system available
Error: No se pudo inicializar la ventana o el renderer
```

Esto es porque SDL2 no encuentra la biblioteca dinamicas de OpenGL ES 2.

Para resolverlo se tiene que agregar los directorios donde estan las bibliotecas de SDL2 y la del drivers de OpenGL ES a las rutas del linker dinamico antes de que se ejecute el programa.

Esto se hace estableciendo la variable de entorno `LD_LIBRARY_PATH` antes de ejecutar el binario con:

```bash
export LD_LIBRARY_PATH=/opt/vc/lib:/lib:/usr/local/lib:/usr/lib
```

Esto lo agrega por la sesion de esa terminal, si se reinicia la Raspberry se elimina y hay que agregarlo otra vez.


Para **agregar de forma permanente** hay que incluir las rutas en el archivo `/etc/ld.so.conf`:

```bash
sudo nano /etc/ld.so.conf
```

Agregar al final del archivo las rutas:

```bash
/usr/local/lib
/opt/vc/lib
```

Despues hay que ejecutar el siguiente comando para refrescar la caché de `ld`:

```bash
sudo ldconfig
```


Ahora si se deberia ejecutar correctamente el programa:

```bash
./timberman
```


## Compilar en Windows


Compilar en la misma Raspberry es lento por lo que usare un PC con Windows para compilar y copiare el binario a la Raspberry para ejecutarlo.

**Tambien vamos a compilar Timberman, pero esta vez desde Windows.**

**Nota importante: Todos los comandos que se usan en Windows son con la sintaxis de la linea de Comandos de Windows, si se usa una terminal que soporta bash, hay que adaptarlos**

### Instalar el Compilador

Este compilador produce binarios para la Raspberry Pi.

Podemos descargar el compilador desde el siguiente enlace:

https://gnutoolchains.com/raspberry/

Descargaremos la version raspberry-gcc-10.2.1.exe que tiene como base _raspios-bullseye_.

Link directo: https://sysprogs.com/getfile/1742/raspberry-gcc10.2.1.exe

Instalar:

Recomendable en la raiz de un disco, la ruta por defecto esta bien:

	C:\SysGCC\raspberry

o crear una carpeta que contenga todo para compilar para la raspberry (libs, proyectos, etc):

	C:\raspberry\	

Ahi instalar el compilador y el codigo fuente de cualquier cosa que se quiere compilar para la raspberry.

_Las demas opciones por defecto estan bien._


### Traspasar las bibliotecas necesarias desde la Raspberry al PC

__Esto se tiene que hacer una sola vez.__

Tenemos que obtener las bibliotecas y archivos header de la Raspberry con las que queremos enlazar nuestro programa, en este caso las de SDL2, que se instalan en la siguiente ruta:

```bash
/usr/local/lib
/usr/local/include/SDL2
```


La forma mas facil es obtener el directorio `/usr/local` completo usando `scp` desde Windows:

```bash
scp -r jorge@192.168.0.5:/usr/local/ .
```

En el directorio `sdl_rpi1`.

En Windows, copiar el directorio `/usr/local` que acabamos de obtener al directorio `sysroot` del compilador que esta en el siguiente directorio:

```bash
<directorio_donde_instalamos_el_compilador>/SysGCC/arm-linux-gnueabihf/sysroot/
```

Tambien necesitamos las bibliotecas de los drivers que descargamos en la seccion de _Instalar Drivers._

Copiamos el directorio `firmware/hardfp/opt` a `<directorio_donde_instalamos_el_compilador>/SysGCC/arm-linux-gnueabihf/sysroot/`.


### Compilar timberman

Descargar el repositorio en algun directorio:

```bash
git clone https://github.com/jorgesc231/timberman.git
```


Abrir la consola de Windows e ir al directorio donde clonamos el repositorio de Timberman:

```
cd timberman
```

El compilador se agrega al PATH durante la instalacion, por lo que ejecutar:

```bash
arm-linux-gnueabihf-gcc
```


Deberia mostrar algo como:

```bash
arm-linux-gnueabihf-gcc: fatal error: no input files
compilation terminated.
```


Crear una variable con la ruta hacia el sysroot:

```bash
set SYSROOT=<directorio_donde_instalamos_el_compilador>/SysGCC/arm-linux-gnueabihf/sysroot/
```

Ejecutar el siguiente comando para compilar:

```bash
arm-linux-gnueabihf-gcc -O2 Timberman.cpp -o timberman_rpi -I%SYSROOT%/usr/local/include/SDL2 -I%SYSROOT%/opt/vc/include -I%SYSROOT%/opt/vc/include/interface/vcos/pthreads -I%SYSROOT%/opt/vc/include/interface/vmcs_host/linux -D_REENTRANT -L%SYSROOT%/usr/local/lib -Wl,-rpath,%SYSROOT%/usr/local/lib -Wl,--enable-new-dtags -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -DRPI1
```

Esto se puede guardar en un archivo `.bat` para no tener que copiar y pegar.

Esto genera un binario llamado `timberman_rpi` que es ejecutable por la Raspberry Pi (no Windows).


### Traspasar el binario a la Raspberry Pi 1

Pasar el binario usando `scp`:

```bash
scp timberman_rpi usuario@192.168.0.5:/home/usuario
```

pasar los assets:

```bash
scp -r assets usuario@192.168.0.5:/home/usuario
```

Dar permisos de ejecucion al binario:

```bash
chmod 0770 timberman_rpi
```

Ejecutar:

```bash
./timberman_rpi
```

Deberia conseguir el mismo resultado que antes.


## Usar con OpenGL ES 2

Ejemplo de como programar aplicaciones con OpenGL ES 2 usando SDL2 como una interfaz con el SO , sin tener que usar el renderer de SDL, es util cuando se quiere hacer un motor grafico propio o juegos 3D, entre otras cosas.

**Esto no es necesario si solo quieres usar SDL2**

Como ya tenemos instalado SDL2 con soporte OpenGL ES 2, por lo que hicimos antes, enlazando con las bibliotecas correctas podemos usarlo sin problemas.

**Compilaremos de Windows a la Raspberry Pi.**


#### Primer ejemplo

**En Windows**

Como primer ejemplo compilare un programa corto que carga una textura en el centro de la pantalla que esta en este mismo directorio: `rpi1_sdl2_gles2.cpp`.

Establecer la variable hacia el `sysroot`:

```bash
set SYSROOT=<directorio_donde_instalamos_el_compilador>/SysGCC/arm-linux-gnueabihf/sysroot/
```

Compilar:

```bash
arm-linux-gnueabihf-g++ -O2 rpi1_sdl2_gles2.cpp -o rpi1_sdl2_gles2 -I$SYSROOT/usr/local/include/SDL2 -I$SYSROOT/opt/vc/include -I$SYSROOT/opt/vc/include/interface/vcos/pthreads -I$SYSROOT/opt/vc/include/interface/vmcs_host/linux -D_REENTRANT -L$SYSROOT/usr/local/lib -L$SYSROOT/opt/vc/lib -Wl,-rpath,$SYSROOT/usr/local/lib -Wl,--enable-new-dtags -lbcm_host -lbrcmEGL -lbrcmGLESv2 -lSDL2 -lSDL2_image
```

Pasar el binario a la Raspberry Pi usando `scp`:

```bash
scp rpi1_sdl2_gles2 usuario@192.168.0.5:/home/usuario
```

pasar la textura que va a mostrar en pantalla:

```bash
scp crate1_diffuse.png usuario@192.168.0.5:/home/usuario
```

**En la Raspberry Pi**

Dar permisos de ejecucion al binario:

```bash
chmod 0770 rpi1_sdl2_gles2
```

Ejecutar:

```bash
./rpi1_sdl2_gles2
```


**Deberia mostrar una textura de una caja en medio de la pantalla.**

_Para salir del programa, presiona cualquier tecla._


#### Segundo ejemplo

Como segundo ejemplo, compilare imgui para la Raspberry PI.

_imgui es una biblioteca que permite crear interfaces graficas_

**En Windows**

Descargar el codigo fuente de imgui desde:

```bash
https://github.com/ocornut/imgui/releases
```

Descomprimir en el directorio que creamos antes `sdl_rpi1` e ingresar al directorio de ejemplos:

```bash
cd imgui-1.89.1\examples\example_sdl_opengl3
```

Crear una variable con la ruta hacia el sysroot:

```bash
set SYSROOT=<directorio_donde_instalamos_el_compilador>/SysGCC/arm-linux-gnueabihf/sysroot/
```

Compilar con el siguiente comando:

```bash
arm-linux-gnueabihf-g++ -O2 main.cpp -o imgui_rpi1 -I .. -I ../.. -I ../../backends ../../backends/imgui_impl_sdl.cpp ../../backends/imgui_impl_opengl3.cpp ../../imgui.cpp ../../imgui_demo.cpp ../../imgui_draw.cpp ../../imgui_tables.cpp ../../imgui_widgets.cpp -I%SYSROOT%/usr/local/include/SDL2 -I%SYSROOT%/opt/vc/include -I%SYSROOT%/opt/vc/include/interface/vcos/pthreads -I%SYSROOT%/opt/vc/include/interface/vmcs_host/linux -D_REENTRANT -DIMGUI_IMPL_OPENGL_ES2 -L%SYSROOT%/opt/vc/lib -L%SYSROOT%/usr/local/lib -Wl,-rpath,%SYSROOT%/usr/local/lib -Wl,--enable-new-dtags -lSDL2 -ldl -lbcm_host -lbrcmEGL -lbrcmGLESv2
```

Pasar el binario a la Raspberry Pi:

```bash
scp imgui_rpi1 usuario@192.168.0.5:/home/usuario
```


**En la Raspberry Pi**

Dar permiso de ejecucion:

```bash
chmod 0770 imgui_rpi
```

Ejecutar:

```bash
./imgui_rpi
```


**Deberia mostrar la demo de imgui.**

_Para desarrollador un programa que use imgui para la  raspberry pi, solo copia el main.cpp del ejemplo y modificalo._

(_Nota: Yo tampoco se como cerrar la demo, reinicia la raspberry pi o conectate usando ssh y usa htop para cerrar la demo_)


#### Mas ejemplos

En el directorio donde estan los las bibliotecas de los drivers OpenGL que descargamos anteriormente hay un sub-directorio donde hay ejemplos de como usar OpenGL ES 2 sin tener que usar SDL2:

```bash
src/hello_pi/
```

Se compilan de igual forma que los ejemplos anteriores.