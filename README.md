# OpenGlFrameWork

A simple framework to simplyfy making basic games/ apps for begginers 

Required Libs SDL2 SDL2_Image SDL2_Mixer SDL2_TTF Glad Glm
Required CPP version at least 23

# Provides:

- UI creation
- Text rendering
- Texture Manager
- Sound Manager
- Scene System
- Collision Detection
- Basic Objects
- Objects to simplyfy 2d grid managment
- PreBuild basic game class and game loop
- Distances Calculation for 2d grid 
- Custom rendering in some cases 17 times faster than pure sdl2

1.0 
- Same posiblilities as pure sdl2 version

1.01
- File explorer improvements
- Better text rendering from right
- Better hovering of selected element in file explorer
- More object orientet set of rectangles
- Faster and simpler alpha rendering

1.02
- Texture nullability check when rendering
- Experimental agressive rendering about 3 times faster for render copy
- Camera focus function

1.03
- New font loading option allowing to load any ttf into png and json
- Simpler and about 20 % faster circle rendering
- New clipping options (useful in some interface rendering)
- Camera enable/disable functions
- Draw Line Renderer Function
- Cleaner ClickBoxList
- Flipping in Ex functions

1.04
- 4 Times faster rendering in debug
- About 15 % faster rendering in realse
- Options for centered text rendering only on X or Y axis
- Improved and much faster experimental flat rendering (still unstable)
- Experimental function for creating temporary font directly from ttf
- Option to Set Base Font for UI
- Function to get all textures and fonts from Texture Manager and UI
- New function for easier mouse pos retrieval

1.05
- Animations are simpler to make and faster and no longer use keys just pointers
- Option to render raw text with UI class
- Z layer rendering for ui (toogle)
- More friendly ui elements managment
- Faster any button removal
- Faster ui element creation
- Altreantive element creation
- ClickBoxList Rework it is now UIList and can store also Button or TextBox
- Faster Compilation

1.06
- New renderer function for rendering modern curved ui elements
- UI now only use scancode
- UI can now create rounded buttons
- SoundMan now can load sound with depp load recursive function
- SoundMan can now play stereo sound and raw sounds
- Simple class UI Section for better big ui managment

1.07
- Simpler Scene Manager Interface
- Experimental Render Copy Should should be safe in all OpenGL versions
- Any RenderEx function render is much better quaility