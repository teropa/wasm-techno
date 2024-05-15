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
  let audioRef = useRef({});

  useEffect(() => {
    console.log("Initialising worklet");
    let stillHere = true,
      cleanUp = () => {};
    Promise.all([wasmCode, workletLoad]).then(([wasmCode]) => {
      if (!stillHere) return;
      let processor = new AudioWorkletNode(ctx, "techno-processor");
      processor.port.postMessage({ cmd: "init", wasmCode });
      let gain = ctx.createGain();
      gain.gain.value = 0.0;
      processor.connect(gain);
      gain.connect(ctx.destination);

      let analyser = new SpectrumAnalyser(ctx, gain, canvasRef.current);
      analyser.analyse();

      audioRef.current.processor = processor;
      audioRef.current.gain = gain;
      audioRef.current.on = false;

      cleanUp = () => {
        processor.port.postMessage({ cmd: "destroy" });
        processor.disconnect();
        gain.disconnect();
        analyser.dispose();
        audioRef.current = {};
      };
    });
    return () => {
      stillHere = false;
      cleanUp();
    };
  }, []);

  useEffect(() => {
    let version = versions[step];
    if (!audioRef.current.processor || !audioRef.current.gain) return;
    if (typeof version === "number") {
      audioRef.current.processor.port.postMessage({
        cmd: "setStep",
        step: version,
      });
      if (!audioRef.current.on) {
        audioRef.current.gain.gain.setValueAtTime(0.0, ctx.currentTime);
        audioRef.current.gain.gain.setTargetAtTime(1.0, ctx.currentTime, 0.05);
      }
      audioRef.current.on = true;
    } else {
      if (audioRef.current.on) {
        audioRef.current.gain.gain.setValueAtTime(1.0, ctx.currentTime);
        audioRef.current.gain.gain.setTargetAtTime(0.0, ctx.currentTime, 0.05);
        audioRef.current.on = false;
      }
    }
  }, [JSON.stringify(versions), step]);

  return (
    <canvas
      ref={canvasRef}
      style={{
        position: "absolute",
        right: 20,
        bottom: 20,
        width: "30%",
        height: "35%",
      }}
    ></canvas>
  );
};
