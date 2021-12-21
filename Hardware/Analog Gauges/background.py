# importing pycairo
#To install p<cairo we also need teh tependencies of the C Headers: 
# > brew install cairo pkg-config
import cairo
import math


def drawLine(context, startX, startY, radius, angle, length):
	"""
	Draw the lines on the arc
	"""

	#Get the position on the arc
	delta_x1 = math.sin(angle) * radius
	delta_y1 = math.sqrt(radius * radius - delta_x1 * delta_x1)

	#Get the ent position of the lione
	radius = radius + length
	delta_x2 = math.sin(angle) * radius
	delta_y2 = math.sqrt(radius * radius - delta_x2 * delta_x2)

	#Move to the arc and draw the line
	context.move_to(startX - delta_x1, startY - delta_y1)
	context.line_to(startX - delta_x2, startY - delta_y2)


def drawGaugeBackground(drawingContext, intMin, intMax, strDescriptioin):
	"""
	Draw the gauge backgrounds
	"""

	startX = 250
	startY = 350
	radius = 200


	lineWidth = 2
	context.set_source_rgb(0, 0, 0)
	context.set_line_width(lineWidth)

	#------------------------------------------
	#Draw the arc
	arcWidthOffset = (math.pi * 0.2)
	angleStart = math.pi + arcWidthOffset
	angleEnd = (2 * math.pi) - arcWidthOffset
	context.arc(startX, startY, radius, angleStart, angleEnd)

	#------------------------------------------
	#Draw the lines
	rad = ((math.pi*2)/360) * 53.7
	drawLine(context, startX, startY, radius, rad, 10)


	rad = ((math.pi*2)/360) * -53.7
	drawLine(context, startX, startY, radius, rad, 10)

	#------------------------------------------
	#Write the description
	context.select_font_face("Georgia", cairo.FONT_SLANT_NORMAL, cairo.FONT_WEIGHT_BOLD)
	context.set_font_size(24)
	(x, y, width, height, dx, dy) = context.text_extents(strDescriptioin)
	context.move_to(startX - width/2, startY - 60)
	context.show_text(strDescriptioin)

	#------------------------------------------
	#Write the unit
	context.set_font_size(12)
	(x, y, width, height, dx, dy) = context.text_extents("[Mbps]") 
	context.move_to(startX - width/2, startY - 40)
	context.show_text("[Mbps]")

	#------------------------------------------
	#Write to the svg
	context.stroke()



# creating a SVG surface
# here geek is file name & 700, 700 is dimension
with cairo.SVGSurface("geek.svg", 500, 500) as surface:

	# creating a cairo context object
	context = cairo.Context(surface)

	# setting scale of the context
	#context.scale(700, 700)

	
	#on_draw(context)
	drawGaugeBackground(context, 0, 20, "Upload")



# printing message when file is saved
print("File Saved")
