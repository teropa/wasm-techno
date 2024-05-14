emcc \
  --no-entry \
  -O3 \
  -s WASM=1 \
  -s EXPORTED_FUNCTIONS=_step1Sine,_step2LinearEnv,_step3ExponentialEnv,_step4KickPitchEnv1,_step5KickPitchEnv2,_step6BassSine,_step7BassSaturation1,_step8BassSaturation2,_step9BassSidechain1,_step10BassSidechain2,_step11ChordsSine,_step12ChordsBeats,_step13ChordsPattern,_step14ChordsSaw2,_step15ChordsSaw3,_step16ChordsSaw4,_step17ChordsSaw5,_step18ChordsSaw6,_makeSomeTechno \
  -o deck/static/techno.wasm \
  techno.c