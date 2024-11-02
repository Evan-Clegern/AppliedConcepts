# Applied Concepts
This is just a series of C++ program implementations for utilities and data structures, using only `<cstdint>` and `<vector>` libraries and base C++ functionality.
It's mostly as a proof of concept, but it's also good for me to keep around in the event that these templated headers become useful for a project.

In the event that anyone wants to use them, go ahead. It's under GPLv3, but frankly the code here isn't special. Just a lot of pointer voodoo, if I'm being honest.

## What's It Got, Huh?
- Binary Tree. Implementation of a binary tree system, with a generation function and with implementations of In-Order, Reverse-Order, Pre-Order and Post-Order traversal algorithms.
- Graph. Implementation of a directed graph system, with creation utilities (all hail the initializer list) and with implementations of Breadth-First and Depth-First search algorithms.
- Function Hooks. Implementation of C#-style function hooks for value-returning and void-returning functions, which is dependent on the hooked functions themselves to invoke the next level in some form.
- Test. A few test programs I made to test the things for the stuff. Very helpful, I know.
