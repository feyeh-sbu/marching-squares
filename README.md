# Marching Squares

An OpenGL implementation of the Marching Squares algorithm for drawing contours.

## Demo

[![Marching Squares Demo](res/marching-squares-demo.gif)](https://youtu.be/9HluleYXrGw)

## Details

Using [Perlin's improved noise](https://cs.nyu.edu/~perlin/noise/) to provide a scalar field $f(x, y, t)$ that changes with time, we use Marching Squares at each frame $t$ to draw the contour $f(x, y) = 0$.

## Todo

- [ ] add CMake support
- [ ] refactor 'global' variables (e.g. `window_width`, `window_height`)
