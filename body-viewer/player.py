import threading
import PySimpleGUI as sg
from gifGen import GifGen
from PIL import ImageGrab
class NbodyPlayer:
    def __init__(self, iterations, width, height, frameLength, loopMode, srcPath):
        self.iterations = iterations
        self.width = width
        self.height = height
        self.frameLength = frameLength
        self.loopMode = loopMode
        self.playbackLength = len(iterations) * frameLength
        self.totalIterations = len(iterations)
        self.currIteration = 0

        self.window = None
        self.closed = False
        self.graphTimer = None

        self.play = True
        self.srcPath = srcPath

        self.imager = GifGen(width, height)

    def initGraph(self):
        width = self.width
        height = self.height

        playerLayout = [
            [sg.Graph(key="canvas", canvas_size=(min(width, 1280), min(height, 720)), graph_bottom_left=(-width, -height), graph_top_right=(width, height), background_color="white")],
            [sg.Text("", key="play-time", size=(50, 1))],
            [sg.Text("Rendering...", key="rendering", size=(50, 1))],
            [sg.Button(key="play-pause", button_text="Pause", enable_events=True)]
        ]

        playerWindow = sg.Window(title="nbody output", layout=playerLayout)
        playerWindow.Finalize()

        self.window = playerWindow

        # Set the initial state of the playback text.
        self.updatePlaybackText()

        self.imager.generateGif(self.srcPath, self.iterations, self.frameLength)
        playerWindow["rendering"].update(f"Animation saved")


    def update(self):

        if self.window == None:
            self.initGraph()

        # Populate the graph with all the bodies.
        self.updateGraph()

        # Main loop for the animation playback window.
        while True:
            event, values = self.window.read()
            
            # Handle the play/pause button.
            if event == "play-pause":
                self.play = not self.play

                if self.play:
                    self.window["play-pause"].update(text="Pause")
                    self.updateGraph()
                else:
                    self.window["play-pause"].update(text="Play")

            # Shut down the player if the user closes it.
            if event in (sg.WIN_CLOSED, "Exit", "Cancel"):
                if self.graphTimer:
                    self.graphTimer.cancel()

                self.closed = True
                self.window.close()
                break

    def updateGraph(self):
        # If we still have frames to draw and we're still playing...
        if self.currIteration < self.totalIterations and self.play and not self.closed:
            graph = self.window["canvas"]

            # Draw all the points in the given frame.
            graph.erase()
            for body in self.iterations[self.currIteration]:
                # Handle situation where window closed while updating.
                if self.closed:
                    break

                if (body["x"] >= -self.width and body["x"] <= self.width) and (body["y"] >= -self.height and body["y"] <= self.height):
                    graph.DrawCircle((body["x"], body["y"]), body["mass"], fill_color="black", line_color="blue")
            
            # Handle situation where window closed while updating.
            if self.closed:
                return

            # Update the play time label
            self.updatePlaybackText()

            # Set up the next frame
            self.currIteration += 1
            self.graphTimer = threading.Timer(self.frameLength, self.updateGraph)
            self.graphTimer.start()
        
        # If we want to loop again, reset the frame counter and start over.
        if self.currIteration == self.totalIterations and self.loopMode:
                self.currIteration = 0
                self.updateGraph()

    def updatePlaybackText(self):
        # Draw the playback text including elapsed time.
        currTime = f"{round(self.currIteration * self.frameLength, 2)}s".ljust(7, " ")
        self.window["play-time"].update(f"Playback: {currTime}: {self.playbackLength}s")
