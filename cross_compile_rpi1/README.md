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