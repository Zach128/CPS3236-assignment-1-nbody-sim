import threading
import PySimpleGUI as sg

class NbodyPlayer:
    def __init__(self, iterations, width, height, frameLength, loopMode):
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

    def initGraph(self):
        width = self.width
        height = self.height

        playerLayout = [
            [sg.Graph(key="canvas", canvas_size=(width, height), graph_bottom_left=(-width, -height), graph_top_right=(width, height), background_color="white")],
            [sg.Text(f"Playback: {self.currIteration * self.frameLength}s : {self.playbackLength}s", key="play-time", size=(50, 1))],
            [sg.Button(key="play-pause", button_text="Pause", enable_events=True)]
        ]

        playerWindow = sg.Window(title="nbody output", layout=playerLayout)
        playerWindow.Finalize()

        self.window = playerWindow

        # Set the initial state of the playback text.
        self.updatePlaybackText()

    def update(self):

        if self.window == None:
            self.initGraph()

        # Populate the graph with all the bodies.
        self.updateGraph()

        # Main loop for the animation playback window.
        while True:
            event, values = self.window.read()
            
            if event == "play-pause":
                self.play = not self.play

                if self.play:
                    self.window["play-pause"].update(text="Pause")
                    self.updateGraph()
                else:
                    self.window["play-pause"].update(text="Play")

            # Close the prompt if the user closes it.
            if event in (sg.WIN_CLOSED, "Exit", "Cancel"):
                if self.graphTimer:
                    self.graphTimer.cancel()

                self.closed = True
                self.window.close()
                break

    # Minimal update function
    def updateGraph(self):
        if self.currIteration < self.totalIterations and self.play and not self.closed:
            graph = self.window["canvas"]

            # Draw all the points at the given iteration.
            graph.erase()
            for body in self.iterations[self.currIteration]:
                # Handle situation where window closed while updating.
                if self.closed:
                    break

                graph.DrawCircle((body["x"], body["y"]), body["mass"], fill_color="black", line_color="blue")
            
            # Handle situation where window closed while updating.
            if self.closed:
                return

            # Update the play time label
            self.updatePlaybackText()

            self.currIteration += 1

            # Set up the next frame
            self.graphTimer = threading.Timer(self.frameLength, self.updateGraph)
            self.graphTimer.start()
        
        # If we want to loop again, reset the frame counter and start over.
        if self.currIteration == self.totalIterations and self.loopMode:
            self.currIteration = 0
            self.updateGraph()

    def updatePlaybackText(self):
        # timeStr = str(round(self.currIteration * self.frameLength, 3))
        currTime = f"{round(self.currIteration * self.frameLength, 2)}s".ljust(7, " ")
        self.window["play-time"].update(f"Playback: {currTime}: {self.playbackLength}s")
