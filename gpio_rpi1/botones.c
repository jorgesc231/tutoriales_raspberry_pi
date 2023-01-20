#include <lgpio.h>
#include <time.h>

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

// NOTA: Si no se coloca \n al final de los strings de formatos de printf no se muestra el texto en la terminal hasta pasado un rato.
// talvez tiene un buffer donde acumula antes de mostrar el texto...


// ping GPIO
#define RED_BUTTON 16
#define WHITE_BUTTON 20
#define BLUE_BUTTON 21

void red_button_callback();
void blue_button_callback();
void white_button_callback();

int main()
{
    int h, status = 0;
    int lFlags = LG_SET_PULL_UP;   // line flags

    h = lgGpiochipOpen(0);

    lgGpioSetAlertsFunc(h, RED_BUTTON, red_button_callback, 0);
    status = lgGpioClaimAlert(h, lFlags, LG_BOTH_EDGES, RED_BUTTON, -1);
    if (status < 0) { printf("Error: %s (%d)\n", lgErrorText(status), status); return 0; }

    lgGpioSetAlertsFunc(h, BLUE_BUTTON, blue_button_callback, 0);
    status = lgGpioClaimAlert(h, lFlags, LG_FALLING_EDGES, BLUE_BUTTON, -1);
    if (status < 0) { printf("Error: %s (%d)\n", lgErrorText(status), status); return 0; }

    lgGpioSetAlertsFunc(h, WHITE_BUTTON, white_button_callback, 0);
    status = lgGpioClaimAlert(h, lFlags, LG_FALLING_EDGES, WHITE_BUTTON, -1);
    if (status < 0) { printf("Error: %s (%d)\n", lgErrorText(status), status); return 0; }

    while (1) {
        lguSleep(0.1); // Sleep for 100 miliseconds
    }

    lgGpiochipClose(h);

    return 0;
}


unsigned short int isPressed(unsigned short int button)
{
    static struct timespec lastCall;
    struct timespec thisCall;
    float timeDiff;

    clock_gettime(CLOCK_REALTIME, &thisCall);

    timeDiff = (thisCall.tv_sec + thisCall.tv_nsec / 1E9 - lastCall.tv_sec - lastCall.tv_nsec / 1E9) * 5;
    lastCall = thisCall;

    return timeDiff > 1 ? 1 : 0;
}

unsigned short int isHeld(unsigned short int button, unsigned short int holdTime)
{
    unsigned short int sample;
    unsigned short int sampleCount = holdTime / 25;
    unsigned short int delayInterval = holdTime / 40;

    for (sample = 0; sample < sampleCount; sample++)
    {
        // El primer argumento de Read es el handle del chip, casi siempre va a ser 0 pero
        // de todas formas deberia encontrar una forma de pasarlo. (Teniendo en cuenta que esto probablemente se ejecute en otro Thread)
        if (lgGpioRead(0, button))
            break;

        lguSleep(delayInterval / 1000.0f);
    }

    return sample == sampleCount ? 1 : 0;
}

void blink (unsigned short int led, unsigned short int interval)
{
    double ms_interval = interval / 1000.0f;

    for (;;) {
        printf("WHITE ALTO\n");
        lguSleep(ms_interval);
        printf("WHITE BAJO\n");
        lguSleep(ms_interval);
    }
}

void *callback(void *args)
{
    blink(WHITE_BUTTON, 250);
    return 0;
}

void red_button_callback()
{
    printf("Red Pressed\n");
}

void blue_button_callback()
{
    if (isPressed(BLUE_BUTTON)) {
        printf("Blue Pressed\n");
    }
}

void white_button_callback()
{
    static pthread_t threadID;

    // if (iPressed(WHITE_BUTTON))
    if (isHeld(WHITE_BUTTON, 1000)) {
        if (threadID) {
            pthread_cancel(threadID);
            threadID = 0;
        } else {
            pthread_create(&threadID, NULL, callback, NULL);
        }
    }
}