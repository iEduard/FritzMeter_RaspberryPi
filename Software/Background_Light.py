from gpiozero import Button, LED
from time import sleep
from signal import pause


def toggleLed():
    """
    Toggle the LED and delay with 200ms.
    This should prevent the led to be turned off from an bouncing button or switch
    """

    ledUp.toggle()
    ledDown.toggle()
    sleep(0.2)


button = Button(17)  #Init the button
ledUp = LED(18)       #Init the led
ledDown = LED(27)       #Init the led

button.when_pressed = toggleLed #Link the toggleLed funktion to button pressed

pause() #Keep the script running