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
- Rounded buttons now support border rendering (also rounded)
- Agresive Render Copy is about 20 percent faster
- Render Copy AS function same as rendercopy but uses diffrent shader
- UI now uses build in MT::Color and MT::ColorA

1.08
- Added New Set of universal rendering functions they are slower than normal ones but never change shader
	what makes them much faster when someting is rendered using different functions
- Upr is now about 10 % faster
- UI and font now use upr for rendering
- Small UI Speed Improvements
- Sound man can now return full sounds list
- UI can now dump elements to json
- Ui can now load elements from json
- Added light wrapper for std::chrono in addons

1.09
- File Explorer srolling speed is now adjustable and base is now 2 times faster
- File Explorer can accept vector of supported formats as parameter
- UI section now supports PopUpBoxes
- Loading json now return UI Section so you can easly get reference to what was loaded
- About 15 % faster render Rectangle
- Sound man code is cleaner and allows setting volumes for individual plays
- Removed memory leak when loading textures
- Texture Manager allows for refreshing textures (it does not change existing ones if png has changed) only if it 
was removed or the new one was added to folder
- New UI function ConsumeIFExist(name) that finally allows for safe status retrieval
- Button->setName() has been removed since it was unsafe now you need to use safe ui->renameElem()
- Faster euclideanDistance calculation
- Agressive render copy is now about 30 % faster
- Agressive rnder copy renamed to FlatRenderCopy and is no longer marked as unstable and experimental
- Flat renderCopy is now even faster aby out 15 %
- SoundMan can refresh sounds just like Texture Manager

1.10
- New tool Path Maker make for simple and fast custom path creation
- Retiveing elemnts from UI is 2 times faster
- Removing ui elements is safer and no longer requires element type
- New UI Helper container TagUISection designed to vastly simplyfy managing large interfaces