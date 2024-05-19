emcc \
  --no-entry \
  -O3 \
  -s WASM=1 \
  -s EXPORTED_FUNCTIONS=_setStep,_makeSomeTechno \
  -o static/techno.wasm \
  technoSteps.c