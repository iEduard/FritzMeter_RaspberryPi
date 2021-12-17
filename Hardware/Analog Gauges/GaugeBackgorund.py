import math
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont

def main():
    """Main programm"""

    _width = 595 * 4
    _height = 842 * 4

    _frontColor = "black"
    _backgroundColor = (255,255,255) 
    _fontLarge = ImageFont.truetype("Georgia.ttf", size=24)
    _fontShort = ImageFont.truetype("Georgia.ttf", size=12)

    # create an image
    image = Image.new("RGB", (_width, _height), _backgroundColor)

    # create rectangle image
    draw = ImageDraw.Draw(image)  

    #draw the arc
    shape = [(100, 120), (400, 400)]
    draw.arc(shape, start = 230, end = 310, fill =_frontColor)

    #Draw the text
    
    ## Center big font
    text = "UP"
    font_width, font_height = _fontLarge.getsize(text)
    draw.text((_width/2 - font_width/2, _height/2 - 100), text, font=_fontLarge, fill=_frontColor)


    ## Center litle font
    text = "[Mbps]"
    font_width, font_height = _fontShort.getsize(text)
    draw.text((_width/2 - font_width/2, _height/2 + 25 -100 ), text, font=_fontShort, fill=_frontColor)


    image.save("Test.pdf", format="PDF", quality=100 )


if __name__ == '__main__':

    #Start the main programm
    main()