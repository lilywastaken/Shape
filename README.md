# Shape

# A shape recognition program

This tool was developped to simplify conception of pictures. It can be used to sum up complex images by the important information.
I've made this for later use with an AI.

This program is currently set to read a few hand-written numbers and identify the important lines.

When you launch it, it will display the original picture. Press any key on the other window to advance through the important lines.


To compile the code, make sure you've installed the following:

sudo apt install g++
sudo apt install libopencv-dev


And run the command:

g++ shape.cpp utils.cpp -o shape `pkg-config --cflags opencv4` `pkg-config --libs opencv4`

