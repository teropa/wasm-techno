import React, { useEffect, useRef } from "react";
import { useDeck } from "mdx-deck";

import { SpectrumAnalyser } from "./SpectrumAnalyser";

let ctx = new AudioContext({ sampleRate: 48000, latencyHint: "playback" });

document.documentElement.addEventListener("click", async () => {
  if (ctx.state !== "running") ctx.resume();
});
document.addEventListener("keydown", async () => {
  if (ctx.state !== "running") ctx.resume();
});

let wasmCode = fetch("techno.wasm").then((res) => res.arrayBuffer());
let workletLoad = ctx.audioWorklet.addModule("TechnoProcessor.js");

export const Audio = ({ versions }) => {
  let { step } = useDeck();
  let canvasRef = useRef();

  useEffect(() => {
    let version = versions[step];
    if (!version) return;
    console.log("Starting", version);
    let stillHere = true,
      cleanUp = () => {};
    Promise.all([wasmCode, workletLoad]).then(([wasmCode]) => {
      if (!stillHere) return;
      let processor = new AudioWorkletNode(ctx, "techno-processor");
      processor.port.postMessage({ cmd: "init", wasmCode, version });
      let gain = ctx.createGain();
      gain.gain.value = 1.0;
      processor.connect(gain);
      gain.connect(ctx.destination);

      let analyser = new SpectrumAnalyser(ctx, processor, canvasRef.current);
      analyser.analyse();

      cleanUp = () => {
        gain.gain.setValueAtTime(1.0, ctx.currentTime);
        gain.gain.linearRampToValueAtTime(0.0, ctx.currentTime + 0.1);
        setTimeout(() => {
          processor.port.postMessage({ cmd: "destroy" });
          processor.disconnect();
          gain.disconnect();
        }, 100);
        analyser.dispose();
      };
    });
    return () => {
      stillHere = false;
      cleanUp();
    };
  }, [JSON.stringify(versions), step]);

  return (
    <canvas
      ref={canvasRef}
      style={{ position: "absolute", width: "100%", height: "100%" }}
    ></canvas>
  );
};
