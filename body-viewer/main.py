from filePrompt import filePrompt
from player import NbodyPlayer

iterations, width, height, frameLength, loopMode, filePath = filePrompt().filePromptMain()

player = NbodyPlayer(iterations, width, height, frameLength, loopMode, filePath)

player.update()
