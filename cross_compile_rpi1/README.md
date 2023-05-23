# Compilacion cruzada para Raspberry Pi 1 en Windows

Este tutorial explica como hacer compilación cruzada de Windows a Raspberry PI.  
  
La razón de no hacerlo desde Linux es que no tengo ningún PC con Linux disponible.  
Podría usar una maquina virtual, pero mi PC es antiguo y va demasiado lento, a si que esta es la única opción que me queda.  
  
En este caso usare como objetivo la Raspberry Pi 1 B que es la única que tengo a mano.  
  
Pero es un proceso similar para cualquier otra Raspberry PI o dispositivo ARM.  
  
Para compilar código C para la plataforma objetivo (Raspberry PI), necesitamos un par de cosas:  
  
- Un cross-compiler y sus herramientas  
- Las bibliotecas estándar compiladas para la plataforma objetivo  
  
El cross-compiler (o compilador cruzado?) podemos compilarlo desde el código fuente, al igual que las demás herramientas necesarias.  
  
_Pero para hacerlo mas fácil descargaremos las herramientas ya compiladas para Windows._

## Descargar el compilador

Este compilador produce binarios para la Raspberry Pi.

Podemos obtener el compilador ya compilado para Windows desde el siguiente enlace:

https://gnutoolchains.com/raspberry/

Descargaremos la versión raspberry-gcc-10.2.1.exe que tiene como base _raspios-bullseye_.

Link directo: https://sysprogs.com/getfile/1742/raspberry-gcc10.2.1.exe

Instalar:

Recomendable en la raíz de un disco, la ruta por defecto esta bien:

	C:\SysGCC\raspberry

o crear una carpeta que contenga todo para compilar para la raspberry (libs, proyectos, etc):

	C:\raspberry\	

Ahí instalar el compilador y el código fuente de cualquier cosa que se quiere compilar para la raspberry.  
  
Las demás opciones por defecto están bien.


## Probar un programa compilado con el cross-compiler en la Raspberry Pi 1

Crear una carpeta para el codigo que queremos compilar para la raspberry:

	mkdir prueba_rp1

En la carpeta crear un crear el archivo de código fuente llamado:

	hola_mundo.c

Con el siguiente contenido:

```C
#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("Hola Mundo desde una raspberry pi 1\n");

    return 0;
}
```


Compilar con el siguiente comando:

```shell
arm-linux-gnueabihf-gcc hola_mundo.c -o hola_mundo
```


Tambien se puede compilar de forma estatica con la biblioteca estandar de C:

```shell
arm-linux-gnueabihf-gcc -static hola_mundo.c -o hola_mundo
```


Traspasar a la raspberry pi usando scp:

```shell
scp hola_mundo <usuario>@192.168.0.5:/home/<usuario>/
```

Reemplazar `usurio` con el nombre de la cuenta de usuario y la `ip` con los de la rpi1.

El comando anterior almacena el programa en el home del usuario.

Dar permisos de ejecucion:

```shell
chmod 0770 hola_mundo
```

Ejecutar:

```shell
./hola_mundo
```


Deberia ejecutarse sin problemas.


## Compilar un programa que usa una biblioteca externa

Usare como ejemplo:  SQLite


#### Descargar el codigo de SQLite


En este caso lo compilare de forma estática agregando el código de SQlite en la aplicación, también se puede como biblioteca dinámica que explicare en otro momento:

Descargar el codigo de SQLite desde:

https://www.sqlite.org/download.html


Cualquiera de los dos que esta en la parte de Source Code:

En este caso vamos a usar: sqlite-amalgamation-version.zip
(la version con autoconf es mejor si se quiere compilar como biblioteca en Linux)


__Descomprimir en un lugar que sea facil de acceder.__


(Si se quisiera de forma dinamica, tendriamos que compilar sqlite como lib dinamica (.so))
(TODO: Explicar como compilar de forma dinamica)


#### Crear un programa que use SQLite

Crearemos un archivo llamado:

```shell
prueba_sqlite.c
```

Con el siguiente contenido:

Este programa de ejemplo crea una base de datos SQLite en memoria y muestra la version de SQLite de dos formas distintas.

```C
#include <sqlite3.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    sqlite3 *db;
    sqlite3_stmt *res;
    
    printf("%s\n", sqlite3_libversion()); 
    
    int rc = sqlite3_open(":memory:", &db);

    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return 1;
    }

    rc = sqlite3_prepare_v2(db, "SELECT SQLITE_VERSION()", -1, &res, 0);    
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return 1;
    }

    rc = sqlite3_step(res);
    
    if (rc == SQLITE_ROW) {
        printf("%s\n", sqlite3_column_text(res, 0));
    }

    sqlite3_finalize(res);
    sqlite3_close(db); 
    
    return 0;
}
```


#### Compilar el programa para la Raspberry Pi desde Windows

Este comando deberia compilar el programa y sqlite.

```shell
arm-linux-gnueabihf-gcc.exe prueba_sqlite.c sqlite-autoconf-3390400\sqlite3.c -o prueba_sqlite -Isqlite-autoconf-3390400\ -lpthread -ldl
```


#### Probar el programa

Pasar a la Raspberry PI 1:

```shell
scp prueba_sqlite <usuario>@192.168.3.25:/home/<usuario>
```

Dar permisos de ejecucion:

```shell
chmod 0770 prueba_sqlite
```

Ejecutar:

```shell
./prueba_sqlite
```

### Ejemplo: Compilar libcurl de forma estatica

*(Por hacer...)*


#### Compilar de forma dinamica

Hay que tener en cuenta que la mayoria de bibliotecas tendremos que compilarlas de forma dinamica.

Necesitamos Windows Subsytem for Linux o MSYS porqué instalan una terminal que funciona como si fuera una de Linux.

Usare MSYS2 porque WSL crashea mi PC...

Se puede descargar desde aqui:

https://www.msys2.org/

Seguir las instrucciones de instalacion de la misma pagina.
La parte 6 de instalar mingw-w64 GCC saltarsela porque no es importante para esto.

*(Por hacer...)*


## Compilar desde una maquina virtual Linux

*(Por hacer...)*


## Usar el IDE Eclipse para compilacion cruzada y depuracion remota


Descargar **Eclipse IDE for Embedded C/C++ Developers**:

[https://www.eclipse.org/downloads/packages/release/2023-03/r/eclipse-ide-embedded-cc-developers]()


Descomprimirlo, ejecutar, y configurar el *workspace*.


Crear un nuevo proyecto:

```
File -> New -> Project... -> C/C++ -> C/C++ Project -> C++ Managed Build 
```

- Seleccionar nombre del proyecto
- Project type -> Executable -> Hello World C++ Proyect
- Toolchains -> Cross GCC

En las 2 siguientes ventanas dejarlo por defecto.

En la siguiente ventana *Cross GCC Command* rellenar con lo siguiente:

En Cross compiler prefix:

```
arm-linux-gnueabihf-
```

En Cross compiler path:

```
C:\SysGCC\raspberry\bin
```

*(Ruta hacia el cross-compiler)*

**Estos son para el Toolchain que instalamos anteriormente.**


Finalizar.


El nuevo proyecto se abrira con codigo de ejemplo.

Se puede compilar presionando el boton del *martillo* arriba a la izquierda.

Debe crear una carpeta llamada *Debug* y generar el binario con el nombre del proyecto dentro.

Para poder ejecutarlo de forma remota en la Raspberry Pi, primero, tiene que estar conectada en la misma red local y con un
servidor SSH habilitado y **gdbserver** instalado.

Para ejecutar y depurar el programa de forma remota tenemos que crear una configuracion de depuracion.

Hacer click en el boton *"Debug"* que esta en la barra de herramientas y tiene la imagen de un bicho.
Debe abrir una ventana para administrar las configuraciones de ejecucion.

Seleccionar **C/C++ Remote Application** y presionar sobre el boton **New launch configuration** arriba a la izquierda.

La configuracion por defecto deberia estar correcta excepto por la conexion.

Ahi seleccionar el boton *New...* que esta a la derecha.

En el menu desplegable de *connection type* seleccion SSH y Ok.

En la siguiente ventana colocar como nombre: *Raspberry Pi*.
En Host la IP de la Raspberry Pi y en User el nombre de la cuenta de usuario con la que se quiere conectar a la Raspberry.
En auntenticacion seleccionar *Password based authentication* e ingresar la contraseña de usuario.
y *Finish*.

En el desplegable *Connection* seleccionar la conexion que recien creamos.

En el campo *Remote Absolute File Path for C/C++ Application* presionar el boton *Browse...* a la derecha.
Si se logra conectar correctamente deberia mostrar el sistema de archivos de la Raspberry Pi.
Ahi seleccionar el directorio donde se quiere almacenar el ejecutable.

Presionar *Apply* y despues *Debug*.

Esto deberia transferir el programa a la Raspberry Pi y comenzar la ejecucion, deteniendo la ejecucion al principio del programa desde
donde se puede avanzar paso a paso y depurar el programa.

La salida en la terminal del programa se mostrara en la consola de eclipse.


Para agregar archivos de cabezera:

```
Boton secundario sobre el proyecto -> Properties -> C/C++ Build -> Settings -> Cross G++ Compiler -> Includes
```


Para agregar libs:

```
Boton secundario sobre el proyecto -> Properties -> C/C++ Build -> Settings -> -> Cross G++ Linker -> Libraries
```


**la raspberry pi deben tener las bibliotecas dinamicas con las que se enlace y demas dependencias para poder ejecutar la aplicacion.**


#### Como compilar y ejecutar la misma aplicacion usando Mingw64 en Windows

Ejecutar el programa en el mismo PC para poder depurarlo y probarlo. Es mucho mas rapido.
Esto sirve cuando no se usan libs que dependan del hardware de la Raspberry Pi (como libs de pines GPIO).

Ejemplo, mi proyecto de un juego que se ejecuta tanto en Windows como en la Raspberry Pi:


```
https://github.com/jorgesc231/snake
```


Crear una nueva config llamada *win32_debug* y en compiler prefix dejarlo en blanco y en el path seleccionar donde **MSYS2** almacena
el binario de GCC.

Es recomendable compilar de forma estatica **std** para no tener que incluir el dll.