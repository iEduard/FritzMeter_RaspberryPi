# importing pycairo
#To install p<cairo we also need teh tependencies of the C Headers: 
# > brew install cairo pkg-config
import cairo
import math



def drawGaugeBackground(drawingContext, intMin, intMax, strDescriptioin):
	"""
	Draw the gauge backgrounds
	"""

	startX = 0.5
	startY = 0.5
	radius = 0.4

	arcWidthOffset = (math.pi * 0.2)

	angleStart = math.pi + arcWidthOffset
	angleEnd = (2 * math.pi) - arcWidthOffset

	lineWidth = 0.01

	#Draw the arc
	context.arc(startX, startY, radius, angleStart, angleEnd)
	context.set_source_rgb(0, 0, 0)
	context.set_line_width(lineWidth)

	context.select_font_face("Georgia", cairo.FONT_SLANT_NORMAL, cairo.FONT_WEIGHT_BOLD)
	context.set_font_size(13)

	context.move_to(startX, startY)
	context.show_text(strDescriptioin)

	context.stroke()


# creating a SVG surface
# here geek is file name & 700, 700 is dimension
with cairo.SVGSurface("geek.svg", 700, 700) as surface:

	# creating a cairo context object
	context = cairo.Context(surface)

	# setting scale of the context
	context.scale(700, 700)

	drawGaugeBackground(context, 0, 20, "Upload")



# printing message when file is saved
print("File Saved")
