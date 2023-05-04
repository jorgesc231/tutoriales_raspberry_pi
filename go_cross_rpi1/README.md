Cross-compile de programa en Go a Raspberry Pi desde PC de escritorio
==================================================================================

La *compilación cruzada* o **cross-compile** permite desarrollar y depurar el programa en un PC mas potente y compilar a un binario que se puede ejecutar de forma nativa en otro procesador, en este caso la **Raspberry PI B+**.

Una ventaja de Go sobre Python es que no es necesario instalar nada en la Raspberry para ejecutar, porque todas las dependencias se compilan de 
forma estatica en el ejecutable final, *ademas es mas facil de integrar con C*.


Con el archivo `prueba.go` con el siguiente codigo:

```
package main

import "fmt"


func main() {
    fmt.Println("Hola Mundo")
}
```


Normalmente se compilaria usando:

```
go build
```


**Pero eso compilaria un binario compatible con el computador que lo compila, *PC Windows con procesador x86*.**

Para compilar para otra arquitectura, en este caso `ARMv6` de una **Raspberry Pi B+**, es necesario establecer las siguientes variables de entorno en la terminal:


- Windows

```
set GOOS=linux
set GOARCH=arm
set GOARM=6
```

- Linux

```
env GOOS=linux GOARCH=arm GOARM=6
```


**Para Raspberry Pi 2, 3 tambien funciona `GOARM=7` y para la 4 deberia funciona `GOARM=8`**


Compilar con el siguiente comando:

```
go build -x prueba.go
```


Deberia generar un ejecutable que, por supuesto, **no se puede ejecutar en el PC pero si en la Raspberry.**

Para pasarlo a la Raspberry usaremos `scp`:

```
scp <programa> <usuario>@<ip_raspberry>:/home/<usuario>/
```


Reemplazar `<programa>` con el nombre del ejecutable, `<usuario>` con el nombre de la cuenta de usuario en la rasberry pi y `<ip_raspberry>` con la IP de la Raspberry.

Talvez es necesario darle permisos de ejecucion al ejecutable:

```
chmod 0770 <programa>
```


Ejecutar:

```
./<programa>
```


## Ejemplo usando `Fiber` y `Gorm` para hacer un servidor Web

Usare mi proyecto de una API de usuarios con Go que usa Fiber y gorm que esta en este repositorio: ()[].

```
git clone https://github.com/jorgesc231/go_api_jwt.git
cd go_api_jwt
```

Para poder hacer compilacion cruzada de modulos que necesitan de `cgo` es necesario un `cross-compiler` de C y definir las siguientes variables de entorno:

*(Ademas de las definidas anteriormente)*

*En Windows y en el caso de la Raspberry Pi B+*

```
set CGO_ENABLED=1
set CC_FOR_TARGET=arm-linux-gnueabihf
set CC=C:\gnu\SysGCC\bin\arm-linux-gnueabihf-gcc
```

*Linux*

```
env CGO_ENABLED=1 CC_FOR_TARGET=arm-linux-gnueabihf CC=C:\gnu\SysGCC\bin\arm-linux-gnueabihf-gcc
```


Compilar:

```
go build
```


**Pasar el binario generado a la Raspberry Pi como se explico en la seccion anterior y ejecutarlo.**



## Ver Arquitecturas soportadas por Go


Para ver cuales arquitecturas soporta:

```
go tool dist list
```


Para ver las arquitecturas que soporta para Linux:

```
go tool dist list | grep linux
```



## Integrar con C (y otros lenguajes)


https://pkg.go.dev/cmd/cgo



#### Llamar codigo C desde Go

(TODO)

#### Llamar codigo Go desde C

(TODO)