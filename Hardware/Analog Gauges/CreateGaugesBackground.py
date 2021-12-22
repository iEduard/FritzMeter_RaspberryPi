# importing pycairo
#To install p<cairo we also need teh tependencies of the C Headers: 
# > brew install cairo pkg-config
import cairo
import math


def drawText(ctx, string, pos, theta = 0.0, face = 'Georgia', font_size = 18):
    ctx.save()

    # build up an appropriate font
    ctx.select_font_face(face , cairo.FONT_SLANT_NORMAL, cairo.FONT_WEIGHT_NORMAL)
    ctx.set_font_size(font_size)
    fascent, fdescent, fheight, fxadvance, fyadvance = ctx.font_extents()
    x_off, y_off, tw, th = ctx.text_extents(string)[:4]
    nx = -tw/2.0
    ny = fheight/2

    ctx.translate(pos[0], pos[1])
    ctx.rotate(theta)
    ctx.translate(nx, ny)
    ctx.move_to(0,0)
    ctx.show_text(string)
    ctx.restore()

def drawLine(context, startX, startY, radius, angle, lineLength, lineWidth, label):
	"""
	Draw the lines on the arc
	"""
	context.set_line_width(lineWidth)

	#Get the position on the arc
	delta_x1 = math.sin(angle) * radius
	delta_y1 = math.sqrt(radius * radius - delta_x1 * delta_x1)

	#Get the ent position of the lione
	radiusEnd = radius + lineLength
	delta_x2 = math.sin(angle) * radiusEnd
	delta_y2 = math.sqrt(radiusEnd * radiusEnd - delta_x2 * delta_x2)

	#Move to the arc and draw the line
	context.move_to(startX - delta_x1, startY - delta_y1)
	context.line_to(startX - delta_x2, startY - delta_y2)

	context.stroke()

	#Set the Text to the 
	radiusText = radiusEnd + 16
	delta_x2 = math.sin(angle) * radiusText
	delta_y2 = math.sqrt(radiusText * radiusText - delta_x2 * delta_x2)

	#If the label is available write it on top of the line
	if len(label) > 0:
		drawText(context, label, (startX - delta_x2, startY - delta_y2), (2 * math.pi) - angle, "Georgia", 24)


def drawGaugeBackground(drawingContext, intMin, intMax, strDescriptioin):
	"""
	Draw the gauge backgrounds
	"""

	startX = 250
	startY = 270
	radius = 200


	lineWidth = 4
	drawingContext.set_source_rgb(0, 0, 0)
	drawingContext.set_line_width(lineWidth)

	#------------------------------------------
	#Draw the arc

	angleOffsetDeg = 50

	angleStartSinusDeg = angleOffsetDeg
	angleStartArcDeg = 270 - angleOffsetDeg
	angleStartSinusRad = ((math.pi*2)/360) * angleStartSinusDeg
	angleStartArcRad = ((math.pi*2)/360) * angleStartArcDeg

	angleEndSinusDeg = -angleOffsetDeg
	angleEndArcDeg = 270 + angleStartSinusDeg
	angleEndSinusRad = ((math.pi*2)/360) * angleEndSinusDeg
	angleEndArcRad = ((math.pi*2)/360) * angleEndArcDeg


	drawingContext.arc(startX, startY, radius, angleStartArcRad, angleEndArcRad)
	drawingContext.stroke()

	#------------------------------------------
	#Draw the lines and labels of the line
	count = 4

	angleSumDeg = 2 * angleOffsetDeg
	angleDelta = angleSumDeg / (count * 5)
	speedDelta = intMax / count
	angleDeg = angleStartSinusDeg
	speedLabel = 0

	for idx in range(21):
		rad = ((math.pi*2)/360) * angleDeg
		
		if int(idx/5) == idx/5:
			length = 20
			lineWidth = 6
			label = str(int(speedLabel))
			speedLabel = speedLabel + speedDelta
		else:
			length = 10
			lineWidth = 4
			label = ""

		drawLine(drawingContext, startX, startY, radius, rad, length, lineWidth, label)
		angleDeg = angleDeg - angleDelta

	#------------------------------------------
	#Write the description
	drawText(drawingContext, strDescriptioin, (startX, startY - ((radius/8)*5)), 0, "Georgia", 32)


	#------------------------------------------
	#Write the unit
	drawText(drawingContext, "[Mbps]", (startX, startY - ((radius/8)*5) + 30), 0, "Georgia", 20)

	#------------------------------------------
	#Write to the svg
	context.stroke()



# creating a SVG surface
# here geek is file name & 700, 700 is dimension
with cairo.SVGSurface("upload.svg", 500, 300) as surface:

	# creating a cairo context object
	context = cairo.Context(surface)

	# setting scale of the context
	#context.scale(700, 700)

	#on_draw(context)
	drawGaugeBackground(context, 0, 30, "Upload")

# creating a SVG surface
# here geek is file name & 700, 700 is dimension
with cairo.SVGSurface("download.svg", 500, 300) as surface:

	# creating a cairo context object
	context = cairo.Context(surface)

	# setting scale of the context
	#context.scale(700, 700)
	
	#on_draw(context)
	drawGaugeBackground(context, 0, 300, "Download")

# printing message when file is saved
print("Files Saved")
