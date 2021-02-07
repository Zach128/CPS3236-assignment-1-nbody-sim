import os.path
from os import path
import PySimpleGUI as sg

def loadPoints(filePath):
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

def filePromptMain():
    # Components of the file prompter.
    filePromptMain = [
        [sg.Text("Select an input file to open")],
        [sg.In(key="file-path"), sg.FileBrowse()],
        [sg.Open(key="submit"), sg.Cancel()]
    ]
    filePrompt = sg.Window(title="Select points file", layout=filePromptMain)

    while True:
        event, values = filePrompt.read()
        
        # Close the prompt if the user closes it.
        if event in (sg.WIN_CLOSED, "Exit", "Cancel"):
            break

        print(values)

        filePath = values["file-path"]

        # Check if the file exists.
        if event == "submit" and filePath and path.exists(filePath) and path.isfile(filePath):
            data = loadPoints(filePath)

            filePrompt.close()

            return data
        else:
            # Print an error if the file doesn't exist.
            sg.popup_quick_message("Bad path", f"The path \"{filePath}\" is not a valid")

def playbackMain(iterations):
    playerLayout = [
        [sg.Graph(key="canvas", canvas_size=(256,256), graph_bottom_left=(-500,-500), graph_top_right=(500, 500), background_color="white")],
        [sg.Cancel()]
    ]
    playerWindow = sg.Window(title="nbody output", layout=playerLayout)
    playerWindow.Finalize()

    graph = playerWindow["canvas"]

    # Populate the graph with all the bodies.
    for body in iterations[0]:
        graph.DrawCircle((body["x"], body["y"]), body["mass"], fill_color="black", line_color="blue")

    # Main loop for the animation playback window.
    while True:
        event, values = playerWindow.read()
        
        # Close the prompt if the user closes it.
        if event in (sg.WIN_CLOSED, "Exit", "Cancel"):
            break

iterations = filePromptMain()
playbackMain(iterations)
