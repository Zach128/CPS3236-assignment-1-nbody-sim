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

    print(iterations)

    # We're finished. Close the file.
    pointFile.close()

fileOpenerlayout = [
    [sg.Text("Select an input file to open")],
    [sg.In(key="file-path"), sg.FileBrowse()],
    [sg.Open(key="submit"), sg.Cancel()]
]

filePrompt = sg.Window(title="Hello World", layout=fileOpenerlayout)

while True:
    event, values = filePrompt.read()
    
    # Close the prompt if the user closes it.
    if event in (sg.WIN_CLOSED, "Exit", "Cancel"):
        break

    print(values)

    filePath = values["file-path"]

    # Check if the file exists.
    if event == "submit" and filePath and path.exists(filePath) and path.isfile(filePath):
        loadPoints(filePath)

    else:
        # Print an error if the file doesn't exist.
        sg.popup_quick_message("Bad path", f"The path \"{filePath}\" is not a valid")
        

