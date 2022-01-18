from gpiozero import Button, LED

while True:
    #Init the button
    button = Button(27)

    #Init the led
    led = LED(17)

    #link the button pressed event to the led toggle function
    button.when_pressed = led.toggle