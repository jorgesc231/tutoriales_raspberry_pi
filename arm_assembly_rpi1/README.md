# ARM Assembly en la Raspberry Pi

Solo tengo una **Raspberry Pi B+** que tiene un procesador que soporta **ARMv6**, pero debe poder ejecutarse en Raspberry Pis mas nuevas que soportan **ARMv7** y **ARMv8**.

Para compilar assembly en la Raspberry Pi necesitamos un **assembler** y un **linker**, usaremos **as** y **ld** que se instalan con las demás herramientas de desarrollo de GNU:


```
sudo apt install build-essentials
```

*(Talvez después ponga ejemplos más avanzados)*


## Primer Programa


Crear un archivo llamado `primero.s` con el siguiente contenido:

```
.global _start
_start:
    MOV R0, #5

    MOV R7, #1
    SWI 0
```


Compilar el programa:

```
as primero.s -o primero.o
```

Enlazarlo:

```
ld primero.o -o primero
```


**El programa retorna el numero decimal 5 en la terminal.**


Para verlo es necesario ejecutar el siguiente comando:

```
echo $?
```

### Explicación


`.global _start`
Hace que la "label" _start sea publica.

`_start:`
Es una etiqueta o "label" donde comienza la ejecución de nuestro código (por defecto).

`MOV R0, #5`
Coloca el numero 5 decimal en el registro 0.

`MOV R7, #1`
Coloca el numero 1 decimal en el registro R7 que es el registro en donde el SO espera el numero de la *syscall* a ejecutar.

`SWI 0`
Interrupción por Software para pasarle el control al OS y ejecute la *syscall* que indicamos en la linea anterior.



## Ejemplo: Hola Mundo

Mostrar `"Hola Mundo"` en la terminal:

Crear un archivo llamado `hola_mundo.s` con el siguiente contenido:

```
.global _start
_start:
    MOV R0, #1          // file descriptor donde queremos escribir (En este caso standard output)
    LDR R1, =message    // Carga la dirección de memoria del texto
    LDR R2, =len      
    MOV R7, #4          // syscall write
    SWI 0               // Software interrupt para que el OS ejecute la syscall

    // llama la syscall exit()
    MOV R7, #1        
    SWI 0            

.data
message:
    .asciz "Hola Mundo\n"

len = .-message                 // declara len y le asigna la longitud del texto.
```


Compilar el programa:

```
as hola_mundo.s -o hola_mundo.o
```

Enlazarlo:

```
ld hola_mundo.o -o hola_mundo
```


### Explicacion

`MOV`
copia el valor al registro indicado.

    https://developer.arm.com/documentation/dui0489/i/arm-and-thumb-instructions/mov

`LDR`
permite pasar datos de la memoria a los registros.

`.data`
Directiva que define el segmento de datos en la memoria del programa.

`.asciz "Hola Mundo\n"` 
Almacena en la seccion de datos una cadena de caracteres que termina con un caracter nulo.


## Ejemplo: Addressing Modes

Es la forma como obtenemos y enviamos datos a los distintos tipos de memoria, Registros y RAM.

En el ejemplo anterior usamos **Inmediate Addressing**:

```
    MOV R0, #1
```

*Carga el numero 1 decimal al registro.*


#### Register Direct Addresing Mode:

```
    MOV R1, R0
```

*Pasa datos de un registro a otro*

El campo de direccion de la instruccion es un registro de la CPU que contiene el operando. 

Es similar al *direct addressing mode*, con la unica diferencia es que opera un registro de la CPU en vez de la memoria principal.



#### Register Indirect Addressing Mode

Este ejemplo almacena una lista de numeros con la label list de tipo WORD en memoria y despues permite pasarlas a registros para procesarla.
*Usamos Register Indirect Addressing para obtener los valores de la memoria.*


    .global _start
    _start:
        LDR R0, =list           // Carga la direccion de memoria de list
        LDR R1, [R0]            // Usa Indirect Addressing Mode para cargar el contenido al que apunta R0
        LDR R2, [R0, #4]        // Usa Indirect Addressing Mode mas un offset para acceder al segundo elemento de la lista
        

    .data
    list:
      .word 4, 5, -9, 1, 0, 2, -3    // word es el tamano por defecto de un valor en memoria de la arquitectura, en este caso 32-bits (4 bytes)


`LDR` permite pasar datos de la memoria a los registros.

La directiva `.data` indica que las siguientes sentencias se tienen que poner en el segmendo de datos de la memoria.

Tambien se puede usar un offset:

```
    LDR R2, [R0, #4]
```
    
Para obtener el siguiente elemento de la lista en memoria.


## Ejemplo: Instrucciones condicionales


Usar intrucciones condicionales para sumar dos numeros, realmente no serian necesarias en este caso pero sirven como ejemplo.

Crear el archivo `suma.s` con el siguiente contenido:

```
.global _start
_start:

    MOV R0, #5          // Coloca los numeros a sumar, 5 y 15 decimales en los Registros 0 y 1.
    MOV R1, #15
      
    BL suma             // "Salto enlazado" hacia la "label" suma
    
    B end               // Salta hacia la "label" end

suma:
    ADD R0, R0, R1      // Suma el valor de los registros R0 y R1 y almacena el resultado en R0
    BX lr               // Retorna a donde salto (BL suma) y continua desde ahi la ejecucion


end:
    MOV R7, #1          // Retorna el resultado de la suma y termina el programa.
    SWI 0
```

**Se compila igual que los ejemplos anteriores.**

Para ver el resultado de la suma:

```
echo $?
```

*Debería mostrar 20.*


## Ejemplo: Ciclo


Por Hacer...


## Ejemplo: Funciones


Por Hacer...



## Depurar usando GDB

Tenemos que compilar el programa con símbolos de depuración:

```
as -g hola_mundo.s -o hola_mundo
```

Ejecutar con el depurador:

```
gdb hola_mundo
```


Establecer un *breakpoint* al principio del programa:

```
(gdb) break _start
```

Ejecutar el programa:

```
(gdb) run
```

Avanzar instrucción a instrucción:

```
(gdb) stepi
```

Ver el valor de un registro:

```
(gdb) info register R0
```

Continuar la ejecución del programa hasta el siguiente "breakpoint":

```
(gdb) continue
```


Salir de GDB:

```
(gdb) quit
```


Mas info en la documentación de GDB: [documentacion](https://www.sourceware.org/gdb/documentation/)



## Ver el assembly generado por GCC


Con el siguiente código en `hola.c`:

```
#include <stdio.h>

int main(int argc, char* argv)
{
    printf("Hola Mundo\n");

    return 0;
}
```


Podemos ver el assembly que genera el compilador así:


```
gcc hola.c -S
```

Esto genera el archivo llamado `hola.s` con el código assembly.

(Hay que tener en cuenta que esta sin optimizar, si usamos -O2 generara un assembly mucho mas corto).



## Mas información

Documentación de las instrucciones ARM:

    https://developer.arm.com/documentation/dui0489/i/arm-and-thumb-instructions/

Los números de las syscalls depende de la arquitectura, mas info:

    https://filippo.io/linux-syscall-table/
    https://marcin.juszkiewicz.com.pl/download/tables/syscalls.html


Compilador online que permite ver el código que generan distintos compiladores para distintas arquitecturas:

    https://godbolt.org/


Documentación de GCC donde explican las opciones del compilador para ARM:

    https://gcc.gnu.org/onlinedocs/gcc/ARM-Options.html