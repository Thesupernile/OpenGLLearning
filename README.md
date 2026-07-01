# Overview
This program was created to help me learn OpenGL. I followed this tutorial for the majority of the programming (https://www.youtube.com/playlist?list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2) however some things (most notibly the circle generation code) were done entirely by myself. 

Since each stage builds off of each other not all screenshots below are generatable with the current program. Screenshots from the current program are marked separately from legacy screenshots for this reason.

Project is left in visual studio format since it's a learning project and not an independent program I'd expect other people to want to run

# Current Screenshots:

Screenshot showing rendered circle as well as ImGui UI 

![Screenshot showing a fully rendered and textured circle in the top left and a GUI in the top right](Screenshots/AshensignWithImGui.png)

# Legacy Screenshots:

Screenshot below shows the textured circle being correctly rendered on a non square display. Achieved via the use of an orthographic projection matrix

![Screenshot showing Ashensign (current github profile picture as of writing)](Screenshots/AshensignWithProjectionMatrix.png)

Screenshot below shows my first textured object. The texture is rendered over a circle (although it's not obvious from the screenshot)

![Screenshot showing red Ashensign texture rendered correctly](Screenshots/AshensignTexture.png)


Screenshot shows my orange circle generated using 129 verticies and appropriate indexes (allowing the circle to be rendered without data duplication)

![Orange circle rendered on a black background](Screenshots/Circle.png)


Screenshot showing an orange triangle with smoothed edges due to enabling MSAA

![Orange triangle with smooth edges](Screenshots/TriangleWithMSAA.png)


Screenshot showing an orange triangle with rough edges

![Orange triangle with rough edges](Screenshots/TriangleNoMSAA.png)
