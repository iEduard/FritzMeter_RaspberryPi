# importing pycairo
import cairo

# creating a SVG surface
# here geek is file name & 700, 700 is dimension
with cairo.SVGSurface("geek.svg", 700, 700) as surface:

	# creating a cairo context object
	context = cairo.Context(surface)

	# creating a rectangle(square) for left eye
	context.rectangle(100, 100, 100, 100)

	# creating a rectangle(square) for right eye
	context.rectangle(500, 100, 100, 100)

	# creating position for the curves
	x, y, x1, y1 = 0.1, 0.5, 0.4, 0.9
	x2, y2, x3, y3 = 0.4, 0.1, 0.9, 0.6

	# setting scale of the context
	context.scale(700, 700)

	# setting line width of the context
	context.set_line_width(0.04)

	# move the context to x,y position
	context.move_to(x, y)

	# draw the curve for smile
	context.curve_to(x1, y1, x2, y2, x3, y3)

	# setting color of the context
	context.set_source_rgba(0.4, 1, 0.4, 1)

	# stroke out the color and width property
	context.stroke()


# printing message when file is saved
print("File Saved")

