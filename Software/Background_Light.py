from gpiozero import Button, LED
from time import sleep
from signal import pause


def toggleLed():
    """
    Toggle the LED and delay with 200ms.
    This should prevent the led to be turned off from an bouncing button or switch
    """

    led.toggle()
    sleep(0.2)


button = Button(2)  #Init the button
led = LED(18)       #Init the led

button.when_pressed = toggleLed #Link the toggleLed funktion to button pressed

pause() #Keep the script running