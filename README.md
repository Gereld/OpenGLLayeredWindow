# OpenGLLayeredWindow

This is the OpenGLLayeredWindow project.

This toy program demonstrate how to use OpenGL to render a layered window. The window has no borders with alpha transparency. It is derived from [this](https://www.dhpoware.com/demos/glLayeredWindows.html) example with some differences :
- the code is in C++
- it uses OpenGL 3.3
- it displays a Wavefront 3D mesh loaded from the resources
- the rendering is done into a frame buffer
- it supports alpha transparency
- the content of the frame buffer is copied into a DIBSECTION which eliminate an additional copy and a vertical flip

Dependencies :
It depends on [glad](https://glad.dav1d.de/) to load OpenGL functions and [glm](https://github.com/g-truc/glm) as math library. They are included for convenience.

References :
<https://www.dhpoware.com/demos/glLayeredWindows.html>

# Building and installing

See the [BUILDING](BUILDING.md) document.

# Licensing

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <https://unlicense.org>
