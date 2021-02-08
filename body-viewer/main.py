from filePrompt import filePrompt
from player import NbodyPlayer

iterations, width, height, frameLength, loopMode = filePrompt().filePromptMain()

player = NbodyPlayer(iterations, width, height, frameLength, loopMode)

player.update()
