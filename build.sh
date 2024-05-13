emcc \
  --no-entry \
  -O3 \
  -s WASM=1 \
  -s EXPORTED_FUNCTIONS=_step1Sine,_step2LinearEnv,_step3ExponentialEnv,_step4KickPitchEnv1,_step5KickPitchEnv2,_step6BassSine,_step7BassSaturation1,_step8BassSaturation2,_makeSomeTechno \
  -o deck/static/techno.wasm \
  techno.c