# maze
A quick maze generator using the Eller's Algorithm described in
https://weblog.jamisbuck.org/2010/12/29/maze-generation-eller-s-algorithm.

I believe this is the algorithm an old basic program from Creative Computing's BASIC Computer Programs from the late 70s used.
That code is particularly dense but generates excellent mazes on machines of the era fairly quickly. I remember printing large mazes
in my childhood on a Kaypro II z80 CP/M machine on a Epson MX-100 dot matrix line printer.

What's amazing (pun intended) about the algorihm is it operates one line at a time and uses no state outside of the current line. It's O(N^2)
but scales mainly with the *width* of the maze, not the height. Of course on a modern machine, in C++ it runs nearly instantaneously, but
could easily scale to a ridiculously large maze.
