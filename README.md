# Gumbo model

## Build instructions

```bash
g++ -Wall -o Scene Scene.cpp -lGL -lGLU -lglut -lGLEW
```

Then run it using `./Scene`.

## Instructions for use

The up and down arrow keys move the camera forwards and backwards, respectively. Page Up and Page Down move the camera forwards and backwards ten times as fast.

The W key toggles the elephant between wireframe and solid fill modes.

The spacebar triggers an explosion of the elephant; it can also be used to pause and resume the explosion while it is happening.

The R key resets the entire scene to its initial state.

# Terrain scene

## Build instructions

```bash
g++ -Wall -o Terrain Terrain.cpp -lGL -lGLU -lglut -lGLEW
```

Then run it using `./Terrain`.

## Instructions for use

The up and down arrow keys move the camera forwards and backwards, respectively. Page Up and Page Down move the camera forwards and backwards ten times as fast.

The W key toggles the terrain between wireframe and solid fill modes.

The 1 and 2 keys switch to the mountainous and flat height maps, respectively.

The A and D keys rotate the light 2° anti-clockwise and clockwise, respectively, around the terrain. The S key toggles auto-rotation of the light — the light rotates clockwise at approximately 40° per second.

The O and P keys lower and raise the water level, respectively. The K and L keys lower and raise the snow level, respectively.
