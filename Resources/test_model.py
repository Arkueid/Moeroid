import live2d.v3 as live2d
import pygame
import os

cd = os.path.split(__file__)[0]

pygame.init()
live2d.init()
size = (800, 600)
window = pygame.display.set_mode(size, pygame.DOUBLEBUF | pygame.OPENGL | pygame.RESIZABLE)
live2d.glInit()

model = live2d.LAppModel()
model.LoadModelJson(os.path.join(cd, "小九/小九皮套（紫）/小九.model3.json"))

running = True
while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
            break
        if event.type == pygame.MOUSEBUTTONUP:
            model.StartRandomMotion()
    
    if not running:
        break

    model.Resize(*pygame.display.get_window_size())
    live2d.clearBuffer()
    model.Update()
    model.Draw()

    pygame.display.flip()

live2d.dispose()
pygame.quit()
