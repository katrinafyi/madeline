# madeline&thinsp;/&thinsp;Proof General
_an integrated prover environment for our favourite deductive logic games._

Each game level is a field of unknown cells and facts providing information about the cells. Facts are used to prove cells are safe and more facts are revealed to the player as they clear cells.

In this way, we aim to support many board-based deductive puzzle games such as Sudoku, Minesweeper, Hexcells, and Tametsi. 

The game provides an abstraction of a proof assistant, allowing the player to attempt a proof on a given cell using selected facts. Then, an SMT solver is consulted to verify the sufficiency of the player's proof. Critically, the solver is only given information the player knows to ensure its proof machinery does not trivialise the game.

This was a project within the UQCS Hackathon 2023.

## build

The project is built using cmake. Dependencies include Dear ImGui, SDL, and Z3. These should be fetched and compiled as needed (z3 will take some time).

```
cmake -B build .
cmake --build build -j4
./build/madeline
```

## screenshot

Currently, the prototype ships with only a single fixed level: the first level of Hexcells.

This shows the main interface and a partial proof in the right panel. An earlier proof's result and the SMT2 encoding can be seen in the console behind.

![image_20230827223610](https://github.com/katrinafyi/madeline/assets/39479354/3b106b79-a9de-4f17-99c6-8050c0451baf)


## todo

- ui improvements:
  - clean up interface and fix bugs on initial startup.
  - hotkeys for common actions and proof interaction.
  - zooming of main game canvas.
  - improved tutorial text above relevant panels.
  - misc styling and polish.
- levels:
  - fix hexcells level 1-1
  - support for choosing a level
  - loading of levels from game-specific file formats
  - automatic level generation 
- gameplay:
  - scoring by number of lemma usages, possibly tie-broken by number of coordinates mentioned in each lemma.
  - restart / save functionality
- internals:
  - performance profiling. reduce copies, especially within widgets.
  - compile to web with emscripten. investigate z3 failure via wasm.
