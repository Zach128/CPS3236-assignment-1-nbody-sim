import re
import os.path
from os import path

import PySimpleGUI as sg

class filePrompt:
    def filePromptMain(self):
        # Components of the file prompter.
        filePromptMain = [
            [sg.Text("Select an input file to open")],
            [sg.In(key="file-path", enable_events=True), sg.FolderBrowse()],
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
                if filePath and path.exists(filePath) and path.isdir(filePath):
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

                    return data, width, height, frameLength, loopMode, filePath
                else:
                    isSuccess = True
    
    def loadPoints(self, inDir):
        # Lambda function for getting the iteration suffix in an iteration file.
        iterationKey = lambda text: [
            # Get the number as a digit.
            int(num) if num.isdigit() else num
            # Get the last digit (iteration number) in the file.
            for num in re.split(r'(\d+)(?!.*\d)', text)
        ]
        
        pointCount = iterationCount = 0
        allPoints = []
        iterations = []

        # Read all the points in the file.
        files = [f.path for f in os.scandir(inDir) if f.is_file]
        files.sort(key=iterationKey)

        for f in files:
            with open(f, "r") as pointFile:
                allPoints = []
                for line in pointFile:
                    # Parse the mass, x and y coordinates into floats
                    m, x, y = [float(val) for val in line.split()]
                    allPoints.append({ "mass": m, "x": x, "y": y })
                iterations.append(allPoints)

        iterationCount = len(files)
        pointCount = len(iterations[0])

        return iterations