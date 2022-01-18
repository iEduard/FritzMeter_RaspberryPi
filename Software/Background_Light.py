from gpiozero import Button, LED


#Initialisierung von GPIO27 als Button (Eingang)
button = Button(27)

#Initialisierung von GPIO17 als LED (Ausgang)
led = LED(17)

#Wen der Button gedrückt wird
button.when_pressed = led.toggle