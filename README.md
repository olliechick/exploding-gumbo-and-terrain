# Gumbo model

## Build instructions

```bash
g++ -Wall -o Scene Scene.cpp -lGL -lGLU -lglut -lGLEW
```

Then run it using `./Scene`.

## Instructions for use

The <kbd>↑</kbd> and <kbd>↓</kbd> arrow keys move the camera forwards and backwards, respectively.
<kbd>Page Up</kbd> and <kbd>Page Down</kbd> move the camera forwards and backwards ten times as fast.

The <kbd>W</kbd> key toggles the elephant between wireframe and solid fill modes.

The <kbd>Space</kbd> key triggers an explosion of the elephant; it can also be used to pause and resume the explosion while it is happening.

The <kbd>R</kbd> key resets the entire scene to its initial state.

## Screenshot

![Gumbo model](https://olliechick.co.nz/images/gumbo.png "Gumbo model")

# Terrain scene

## Build instructions

```bash
g++ -Wall -o Terrain Terrain.cpp -lGL -lGLU -lglut -lGLEW
```

Then run it using `./Terrain`.

## Instructions for use

The <kbd>↑</kbd> and <kbd>↓</kbd> arrow keys move the camera forwards and backwards, respectively.
<kbd>Page Up</kbd> and <kbd>Page Down</kbd> move the camera forwards and backwards ten times as fast.

The <kbd>W</kbd> key toggles the terrain between wireframe and solid fill modes.

The <kbd>1</kbd> and <kbd>2</kbd> keys switch to the mountainous and flat height maps, respectively.

The <kbd>A</kbd> and <kbd>D</kbd> keys rotate the light 2° anti-clockwise and clockwise, respectively, around the terrain.
  The <kbd>S</kbd> key toggles auto-rotation of the light — the light rotates clockwise at approximately 40° per second.

The <kbd>O</kbd> and <kbd>P</kbd> keys lower and raise the water level, respectively.
The <kbd>K</kbd> and <kbd>L</kbd> keys lower and raise the snow level, respectively.

## Screenshot

![Terrain model](https://olliechick.co.nz/images/terrain.png "Terrain model")
