import os.path
from os import path

import PySimpleGUI as sg

class filePrompt:
    def filePromptMain(self):
        # Components of the file prompter.
        filePromptMain = [
            [sg.Text("Select an input file to open")],
            [sg.In(key="file-path", enable_events=True), sg.FileBrowse()],
            [
                sg.Text("Width: "),
                sg.Input(key="width", size=(10, 1), default_text="500", enable_events=True),
                sg.Text("Height: "),
                sg.Input(key="height", size=(10, 1), default_text="500", enable_events=True)
            ],
            [sg.Text("Frame duration (in seconds): "), sg.Input(key="frame-length", size=(10, 1), default_text="0.005", enable_events=True)],
            [sg.Checkbox("Loop animation: ", key="loop-mode", default=False, enable_events=True)],
            [sg.Open(key="submit"), sg.Cancel()]
        ]
        filePrompt = sg.Window(title="Select points file", layout=filePromptMain)

        while True:
            isSuccess = True
            event, values = filePrompt.read()
            
            # Close the prompt if the user closes it.
            if event in (sg.WIN_CLOSED, "Exit", "Cancel"):
                break

            filePath = values["file-path"]
            width = values["width"]
            height = values["height"]
            frameLength = values["frame-length"]
            loopMode = values["loop-mode"]

            # Handle the submit event and form validation.
            if event == "submit":
                # Check if the path exists.
                if filePath and path.exists(filePath) and path.isfile(filePath):
                    data = self.loadPoints(filePath)
                else:
                    # Print an error if the file doesn't exist.
                    sg.popup_quick_message("Bad path", f"The path \"{filePath}\" is not a valid")
                    isSuccess = False
                
                # Handle width
                if width:
                    try:
                        width = int(width)
                    except:
                        # Print an error width parsing failed.
                        sg.popup_quick_message("Bad width", f"The width must be a valid whole number. \"{width}\" is not valid.")
                        isSuccess = False
                else:
                    # Print an error if the width.
                    sg.popup_quick_message("No width", "A width in pixels needs to be entered.")
                    isSuccess = False
                
                # Handle height
                if height:
                    try:
                        height = int(height)
                    except:
                        # Print an error height parsing failed.
                        sg.popup_quick_message("Bad height", f"The height must be a valid whole number. \"{height}\" is not valid.")
                        isSuccess = False
                else:
                    # Print an error if the height.
                    sg.popup_quick_message("No height", "A height in pixels needs to be entered.")
                    isSuccess = False
                
                # Handle frame length
                if frameLength:
                    try:
                        frameLength = float(frameLength)
                    except:
                        # Print an error frameLength parsing failed.
                        sg.popup_quick_message("Bad frame duration", f"The frame must be a valid number number. \"{frameLength}\" is not valid.")
                        isSuccess = False
                else:
                    # Print an error if the frameLength is not entered.
                    sg.popup_quick_message("No frame duration", "A frame length in seconds needs to be entered.")
                    isSuccess = False

                # If validation was successful, continue.
                if isSuccess:
                    filePrompt.close()

                    return data, width, height, frameLength, loopMode
                else:
                    isSuccess = True
    
    def loadPoints(self, filePath):
        pointFile = open(filePath, "r")
        pointCount = iterationCount = 0
        allPoints = []
        iterations = []

        # Try read the number of points in the file.
        try:
            pointCount = int(pointFile.readline())
        except:
            raise Exception("Failed to read the file header. Invalid file. Expected format \"%d\\n\"")

        for line in pointFile:
            # Parse the mass, x and y coordinates into floats
            m, x, y = [float(val) for val in line.split()]
            allPoints.append({ "mass": m, "x": x, "y": y })

        # Get the number of iterations
        iterationCount = int(len(allPoints) / pointCount)
        # Initialise an empty 2d array iterationCount x pointCount in size.
        iterations = [[0] * pointCount for _ in range(iterationCount)]
        
        for i, point in enumerate(allPoints):
            pointIndex = i % pointCount
            iterationIndex = i // pointCount
            iterations[iterationIndex][pointIndex] = point

        # We're finished. Close the file.
        pointFile.close()

        return iterations