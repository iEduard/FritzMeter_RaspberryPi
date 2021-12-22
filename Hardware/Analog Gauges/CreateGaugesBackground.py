# importing pycairo
#To install p<cairo we also need teh tependencies of the C Headers: 
# > brew install cairo pkg-config
import cairo
import math

#Define the metrics of the gauge
MAX_DOWNLOAD_SPEED = 200
MAX_UPLOAD_SPEED = 30

BACKGROUND_SIZE_X = 650
BACKGROUND_SIZE_Y = 400


#Define the style of the gauges  
FONT_LABEL = "Helvetica"
FONT_LABEL_SIZE = 40
FONT_DESCRIPTION = "Helvetica"
FONT_DESCRIPTION_SIZE = 48

def main():
	"""Main programm"""

	#Draw the upload gauge background
	with cairo.SVGSurface("upload.svg", BACKGROUND_SIZE_X, BACKGROUND_SIZE_Y) as surface:

		# creating a cairo context object asd asd 
		context = cairo.Context(surface)
		drawGaugeBackground(context, MAX_UPLOAD_SPEED, "UP",  6, BACKGROUND_SIZE_X, BACKGROUND_SIZE_Y)

	#Draw the download gauge background
	with cairo.SVGSurface("download.svg", BACKGROUND_SIZE_X, BACKGROUND_SIZE_Y) as surface:

		# creating a cairo context object
		context = cairo.Context(surface)
		drawGaugeBackground(context, MAX_DOWNLOAD_SPEED, "DOWN", 4, BACKGROUND_SIZE_X, BACKGROUND_SIZE_Y)

	# printing message when files are saved
	print("Files Saved")


def drawGaugeBackground(drawingContext, intMax, description, devidedBy, sizeBackgroundX, sizeBackgroundY):
	"""
	Draw the gauge backgrounds
	"""

	startX = sizeBackgroundX/2
	startY = sizeBackgroundY
	radius = 320


	lineWidth = 4

	drawingContext.set_source_rgb(0, 0, 0)
	drawingContext.set_line_width(lineWidth)

	#------------------------------------------
	#Draw the frame (TOP)
	drawingContext.move_to(0, 0)
	drawingContext.line_to(sizeBackgroundX, 0)

	#Draw the frame (Left)
	drawingContext.move_to(0, 0)
	drawingContext.line_to(0, sizeBackgroundY - 90)

	#Draw the frame (Bottom)
	drawingContext.move_to(0, sizeBackgroundY - 90)
	drawingContext.line_to(50, sizeBackgroundY - 90)

	drawingContext.move_to(50, sizeBackgroundY - 90)
	drawingContext.line_to(100, sizeBackgroundY)

	drawingContext.move_to(100, sizeBackgroundY)
	drawingContext.line_to(200, sizeBackgroundY)

	drawingContext.move_to(200, sizeBackgroundY)
	drawingContext.line_to(250, sizeBackgroundY - 100)

	drawingContext.move_to(250, sizeBackgroundY - 100)
	drawingContext.line_to(400, sizeBackgroundY - 100)

	drawingContext.move_to(400, sizeBackgroundY - 100)
	drawingContext.line_to(450, sizeBackgroundY)

	drawingContext.move_to(450, sizeBackgroundY)
	drawingContext.line_to(550, sizeBackgroundY)

	drawingContext.move_to(550, sizeBackgroundY)
	drawingContext.line_to(600, sizeBackgroundY - 90)

	drawingContext.move_to(550, sizeBackgroundY)
	drawingContext.line_to(600, sizeBackgroundY - 90)

	drawingContext.move_to(600, sizeBackgroundY - 90)
	drawingContext.line_to(sizeBackgroundX, sizeBackgroundY - 90)

	#Draw the frame (Right)
	drawingContext.move_to(sizeBackgroundX, 0)
	drawingContext.line_to(sizeBackgroundX, sizeBackgroundY - 90)


	drawingContext.stroke()

	#------------------------------------------
	#Draw the arc

	angleOffsetDeg = 45

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
	count = devidedBy

	angleSumDeg = 2 * angleOffsetDeg
	angleDelta = angleSumDeg / (count * 5)
	speedDelta = intMax / count
	angleDeg = angleStartSinusDeg
	speedLabel = 0

	for idx in range(5 * count + 1):
		rad = ((math.pi*2)/360) * angleDeg
		
		if int(idx/5) == idx/5:
			length = 30
			lineWidth = 5
			label = str(int(speedLabel))
			speedLabel = speedLabel + speedDelta
		else:
			length = 15
			lineWidth = 2
			label = ""

		drawLine(drawingContext, startX, startY, radius, rad, length, lineWidth, label)
		angleDeg = angleDeg - angleDelta

	#------------------------------------------
	#Write the description
	drawText(drawingContext, description, (startX, startY - ((radius/8)*6)), 0, FONT_DESCRIPTION, FONT_DESCRIPTION_SIZE)

	#Write the unit
	drawText(drawingContext, "[Mbps]", (startX, startY - ((radius/8)*6) + 55), 0, FONT_DESCRIPTION, FONT_DESCRIPTION_SIZE - 12)

	#------------------------------------------
	#Write to the svg
	drawingContext.stroke()

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
	#If the label is available write it on top of the line
	if len(label) > 0:

		radiusText = radiusEnd + 25
		delta_x2 = math.sin(angle) * radiusText
		delta_y2 = math.sqrt(radiusText * radiusText - delta_x2 * delta_x2)

		drawText(context, label, (startX - delta_x2, startY - delta_y2), (2 * math.pi) - angle, FONT_LABEL, FONT_LABEL_SIZE)

def drawText(ctx, string, pos, theta = 0.0, face = 'Georgia', font_size = 18):
	ctx.save()

	# build up an appropriate font
	ctx.select_font_face(face , cairo.FONT_SLANT_NORMAL, cairo.FONT_WEIGHT_BOLD)
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


if __name__ == '__main__':

	#Start the main programm
	main()
