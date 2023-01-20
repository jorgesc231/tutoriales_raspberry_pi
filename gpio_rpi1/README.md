# Usar los pines GPIO de la Raspberry Pi con C

Al parecer la biblioteca `wiringpi` ya no se encuentra en los repositorios y tampoco en la pagina del creador. 

Por lo tanto **usare una alternativa**: [lgpio](https://abyz.me.uk/lg/index.html).

Mostrare como se instala, un ejemplo basico de como se usa y otro ejemplo de como implementar botones fisicos para la Raspberry Pi.


Documentación de lgpio: [https://abyz.me.uk/lg/lgpio.html]().


## Compilar lgpio en Raspberry Pi

Siguiendo las intrucciones de la documentacion: [https://abyz.me.uk/lg/download.html]().

*En este caso no instalaremos los prerequisitos de los modulos de Python.*

- Instalar herramientas necesarias:

```
sudo apt install build-essentials
```

- Descargar el codigo y descomprimirlo:

```
wget http://abyz.me.uk/lg/lg.zip
unzip lg.zip
cd lg
```

- Compilar

```
make
```

- instalar la biblioteca a /usr/local/:

```
sudo make install
```

Si al ejecutar `make install` sale el siguiente error:

```bash
Traceback (most recent call last):
  File "/home/jorge/lg/PY_RGPIO/setup.py", line 3, in <module>
    from setuptools import setup
ModuleNotFoundError: No module named 'setuptools'
*** install of Python3 rgpio.py failed ***
/usr/bin/python3
``` 

Esto es por no instalar los requerimientos para el modulo de Python, **a pesar de esto la biblioteca para C ya se instalo correctamente por lo
que si no se va usara Python se puede ignorar**.


## Ejemplo: Leer y escribir a los pines GPIO

Leer de un pin y escribirlo en otro.

Crear el archivo: `read_write_gpio.c` con el siguiente contenido:

    ```C
    #define INPUT_PIN 20
    #define OUTPUT_PIN 21
    #define ALERT_PIN 22

    void funcion_callback(int event, lgGpioAlert_p evt, void *user_data)
    {
        printf("ALERTA\n");
    }

    int main(int agrc, char *argv[])
    {
        int h, value = 0;
        int lFlags = 0;   // line flags
    
        // Obtener un handle al chip GPIO
        h = lgGpiochipOpen(0);

        // Configurar un pin GPIO para ENTRADA
        lgGpioClaimInput(h, lFlags, INPUT_PIN);

        // Configurar un pin GPIO para SALIDA
        lgGpioClaimOutput(h, lFlags, OUTPUT_PIN, 0)   // initial level 0

        
        // Configura un pin GPIO para alertas (Llame a una funcion definida por nosotros cada vez que ocurra un evento)
        lgGpioClaimAlert(h, lFlags, LG_BOTH_EDGES, ALERT_PIN, -1);

        // Establece la funcion callback para las alertas
        lgGpioSetAlertsFunc(h, ALERT_PIN, funcion_callback, NULL);

        while (1) {
            value = lgGpioRead(h, INPUT_PIN);       // Leer desde el pin
            
            // Escribir el valor que leemos al pin se salida
            lgGpioWrite(h, OUTPUT_PIN, value);
            
            lguSleep(0.1);    // Duerme el programa por 100 ms
        }
    }
    ```

En el pin de entrada se puede colocal un boton y en el de salida un LED para comprobar que funciona.



- Compilar:

    ```
    gcc -o read_write_gpio read_write_gpio.c -llgpio
    ```


- Ejecutar:

    ```
    ./read_write_gpio
    ```

*(Ejemplo inspirado por: [https://elinux.org/C]())*


## Implementar botones

En `botones.c` se implementan **tres** botones fisico usando los pines GPIO de la Raspberry Pi.

**Cada boton se implementa de forma diferente como ejemplo**, el `boton rojo` detecta cuando se presiona y cuando se suelta, el `blanco` cuando se mantiene presionado y el `azul` solo cuando se presiona.

**Por defecto estan conectados en los pines GPIO 16, 20 y 21.**


Este codigo es una adaptacion del de [este tutorial](https://www.youtube.com/watch?v=9Znf0CnsbXI) pero en vez de usar `wiringpi` lo implemento usando la [biblioteca lgpio](https://abyz.me.uk/lg/index.html).

(No uso LEDs como en el tutorial porque no tengo...)


- Descargar:

```
git clone 
```

- Compilar

```
gcc -o botones botones.c -llgpio -lpthread
```

- Ejecutar:

```
./botones
```


## Mas ejemplos

Hay mas ejemplos en el directorio `examples` del codigo que descargamos al principio.

## Referencia


[Documentacion](https://abyz.me.uk/lg/lgpio.html)

Otra biblioteca alternativa es: [pigpio](https://abyz.me.uk/rpi/pigpio/index.html).

