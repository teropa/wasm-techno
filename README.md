# Making Minimal Techno with WebAssembly And a C Compiler

Code and slides from my [JSHeroes 2024](https://jsheroes.io/) presentation.

## Running the example

Just start a static HTTP server from this directory, e.g.

```
npx serve
```

## Hacking

To compile the C code to WebAssembly you'll need to have [Emscripten](https://emscripten.org/) installed and [an Emscripten environment activated](https://emscripten.org/docs/getting_started/downloads.html) (i.e. `emcc` should be in your path).

You'll find the C code in `techno.c`, the Audio Worklet processor in `TechnoProcessor.js`, and page init in `index.html`.

After making changes in the C code, compile it by running `npm run build`.
