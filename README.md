# Charn
by Nick Black (nick.black@sprezzatech.com)

![image](doc/charn.gif)

>It is silent now. But I have stood here when the whole air was full of the
>noises of Charn; the trampling of feet, the creaking of wheels, the cracking of
>the whips and the groaning of slaves, the thunder of chariots, and the
>sacrificial drums beating in the temples. I have stood here (but that was near
>the end) when the roar of battle went up from every street and the river of
>Charn ran red. All in one moment one woman blotted it out forever.
>
> C.S. Lewis, *The Magician's Newphew*

## Requirements

* Autotools, autoconf-archive, and C development tools
* Docbook with XSL support
* OpenGL
* GLEW (www.opengl.org/sdk/libs/GLEW/)
* GLUT (www.opengl.org/resources/libraries/glut/)
* FTGL
* SOIL
* XDG-basedir
* XCB
* XKB
* XI2

On Debian-derived systems, this includes:

* autoconf
* autoconf-archive
* autoconf-gl-macros
* docbook-xsl
* libgl1-mesa-dev
* libglew-dev
* freeglut3-dev
* libftgl-dev
* libsoil-dev
* libxdg-basedir-dev
* libx11-xcb-dev
* xcb-proto
* libxcb-ewmh1-dev
* libxcb-keysyms1-dev
* libxcb-randr0-dev
* libxcb-util0-dev
* libxkbcommon-dev

## Building

* From git: autoreconf -fis && ./configure && make && sudo make install
* From tarball: ./configure && make && sudo make install

## Reading

* http://www.sprezzatech.com/blog/0014-new-directions-in-window-management-p1.html
* http://www.sprezzatech.com/blog/0015-new-directions-in-window-management-p2.html
