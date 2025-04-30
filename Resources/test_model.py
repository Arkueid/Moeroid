from live2d import v3 as live2d
import pygame
import os
import time

cd = os.path.split(__file__)[0]

pygame.init()
live2d.init()
size = (800, 600)
window = pygame.display.set_mode(size, pygame.DOUBLEBUF | pygame.OPENGL | pygame.RESIZABLE)
live2d.glInit()

model = live2d.Model()
model.LoadModelJson(os.path.join(cd, "魅魔喵/meimo1.model3.json"))

model.CreateRenderer(2)

lastUpdateTime = time.time()

model.AddExpression("双马尾")
model.AddExpression("发色")

partIds = model.GetPartIds()

drawableIds = model.GetDrawableIds()

for i in ['ArtMesh207_Skinning', 'ArtMesh208_Skinning', 'ArtMesh209_Skinning', 'ArtMesh213_Skinning', 'ArtMesh212_Skinning', 'ArtMesh214_Skinning', 'ArtMesh211_Skinning','ArtMesh210_Skinning', 'ArtMesh215_Skinning', 'ArtMesh216_Skinning', 'ArtMesh217_Skinning',
          'ArtMesh218_Skinning' 
          ]:
    model.SetPartMultiplyColor(partIds.index(i), 212/255, 94/255, 116/255, 1.0)
    print({"partIndex": partIds.index(i), "color": [212/255, 94/255, 116/255, 1.0]}, end=',\n')

model.SetDrawableMultiplyColor(drawableIds.index('ArtMesh13'), 212/255, 94/255, 116/255, 1.0)
print({"drawableIndex": drawableIds.index('ArtMesh13'), "color": [212/255, 94/255, 116/255, 1.0]}, end=',\n')
model.SetDrawableMultiplyColor(drawableIds.index('ArtMesh16'), 212/255, 94/255, 116/255, 1.0)
print({"drawableIndex": drawableIds.index('ArtMesh16'), "color": [212/255, 94/255, 116/255, 1.0]}, end=',\n')

print(212/255, 94/255, 116/255)

model.LoadExtraMotion("extra", 0, os.path.join(cd, "extra.motion3.json"))

running = True
while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
            break
        if event.type == pygame.MOUSEBUTTONUP:
            model.StartRandomMotion("extra")
            x, y = event.pos
            print(model.HitDrawable(x, y, True))
            
    
    if not running:
        break

    model.Resize(*pygame.display.get_window_size())
    live2d.clearBuffer()
    ct = time.time()
    
    deltaSecs = ct - lastUpdateTime
    deltaSecs = max(0.0001, deltaSecs)
    lastUpdateTime = ct

    # the following section is equal to LAppModel.Update()

    # === Section Start Update() ===
    # load cached parameters from last frame
    motionUpdated = False
    model.LoadParameters() # initialize params using cached values

    if not model.IsMotionFinished():
        motionUpdated = model.UpdateMotion(deltaSecs)

    # if SetParameterValue is called here, the parameter will be saved to the cache
    # model.SetParameterValue(StandardParams.ParamAngleX, params.AngleX, 1)

    model.SaveParameters() # save params to cache for next frame

    if not motionUpdated:
        # auto blink
        # update eye blink params if they are defined in the model3.json
        model.UpdateBlink(deltaSecs)  

    model.UpdateExpression(deltaSecs)

    model.UpdateDrag(deltaSecs)

    # auto breath
    # update breath params such as ParamBodyAngleX, ParamAngleX...
    model.UpdateBreath(deltaSecs)  

    # create physics effects according to current and previous param values
    # some params can be overridden by physics effects
    model.UpdatePhysics(deltaSecs)

    model.UpdatePose(deltaSecs)
    # === Section End Update() ===

    # Draw():
    #   1. update meshes according to the parameters
    #   2. draw meshes using opengl
    model.Draw()

    pygame.display.flip()

live2d.dispose()
pygame.quit()