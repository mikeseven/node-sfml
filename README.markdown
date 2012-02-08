NodeJS bindings to SFML
=======================

This projects attempts to provide platform-independent (as far as supported by SFML -- Windows, Mac, Linux) access to windowing system and input devices on desktop/laptop computers.

Installation
------------
You need SFML on your machine. Go download it at https://github.com/LaurentGomila/SFML.
SFML uses cmake. So make sure you have it on your machine, then type:

>cmake-gui .

Check the options, save the CMakefile, and run

>cmake .

Then

>make

Currently, we support NodeJS 0.6.5 or above but not 0.7.x.

