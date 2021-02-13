from PIL import Image, ImageDraw
import os
import pathlib

class GifGen:
    def __init__(self, width, height):
        self.images = []
        self.width = width
        self.height = height

    def addFrame(self, points, iterationNum):
        # If this is the first image, calculate the coordinate scale to use throughout.
        if iterationNum == 0:
            xs = [body["x"] for body in points]
            ys = [body["y"] for body in points]

            self.minX = abs(min(xs))
            self.minY = abs(min(ys))
            self.maxX = max(xs)
            self.maxY = max(ys)

        width = self.width
        height = self.height
        minX = self.minX
        minY = self.minY
        maxX = self.maxX
        maxY = self.maxY

        # Set up a new image for processing.
        image = Image.new("P", (self.width, self.height), (255))
        draw = ImageDraw.Draw(image)

        xCenter = self.width / 2
        yCenter = self.height / 2

        # For each point to draw, calculate the top-left and bottom-right point coordinates for drawing a circle-shaped ellipse
        # Each coordinate is calulated using the following:
        # image center + (point-space coordinates converted to image-space coordinates) - half-body mass.
        for body in points:
            x1 = xCenter + ((body["x"] / (maxX + minX)) * self.width - body["mass"] / 2)
            x2 = xCenter + ((body["x"] / (maxX + minX)) * self.width + body["mass"] / 2)
            y1 = yCenter + ((body["y"] / (maxY + minY)) * self.height - body["mass"] / 2)
            y2 = yCenter + ((body["y"] / (maxY + minY)) * self.height + body["mass"] / 2)
            draw.ellipse((x1, y1, x2, y2), fill = "black", outline ="black")

        # The image comes out inverted, so it needs to be flipped.
        image = image.transpose(Image.FLIP_TOP_BOTTOM)
        image.info["duration"] = 5

        self.images.append(image)

    def saveAsGif(self, path, frameDuration):
        savePath = f"{os.path.dirname(path)}/nbody_animation.gif"
        self.images[0].save(savePath, format="GIF", save_all=True, optimize=False, append_images=self.images[1:], loop=0, duration=int(frameDuration * 1000))

    def generateGif(self, path, iterations, frameDuration):
        for iteration, bodies in enumerate(iterations):
            self.addFrame(bodies, iteration)
        self.saveAsGif(path, frameDuration)
            
        
