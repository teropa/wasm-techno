emcc \
  --no-entry \
  -O3 \
  -s WASM=1 \
  -s EXPORTED_FUNCTIONS=_makeSomeTechno \
  -o deck/static/techno.wasm \
  techno.c