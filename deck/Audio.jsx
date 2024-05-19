import React, { useEffect, useRef } from "react";
import { useDeck } from "mdx-deck";

import { SpectrumAnalyser } from "./SpectrumAnalyser";

let _ctx;
function getCtx() {
  if (!_ctx) {
    let audioContext = new AudioContext({
      sampleRate: 48000,
      latencyHint: "playback",
    });
    let wasmCode = fetch("techno.wasm").then((res) => res.arrayBuffer());
    let workletLoad = audioContext.audioWorklet.addModule("TechnoProcessor.js");
    _ctx = { audioContext, wasmCode, workletLoad };
  }
  return _ctx;
}

if (typeof document !== "undefined") {
  document.documentElement.addEventListener("click", async () => {
    if (getCtx().audioContext.state !== "running")
      getCtx().audioContext.resume();
  });
  document.addEventListener("keydown", async () => {
    if (getCtx().audioContext.state !== "running")
      getCtx().audioContext.resume();
  });
}

export const Audio = ({ versions }) => {
  let { step } = useDeck();
  let canvasRef = useRef();
  let audioRef = useRef({});

  useEffect(() => {
    console.log("Initialising worklet");
    let { audioContext, wasmCode, workletLoad } = getCtx();
    let stillHere = true,
      cleanUp = () => {};
    Promise.all([wasmCode, workletLoad]).then(([wasmCode]) => {
      if (!stillHere) return;
      let processor = new AudioWorkletNode(audioContext, "techno-processor");
      processor.port.postMessage({ cmd: "init", wasmCode });
      let gain = audioContext.createGain();
      gain.gain.value = 0.0;
      processor.connect(gain);
      gain.connect(audioContext.destination);

      let analyser = new SpectrumAnalyser(
        audioContext,
        gain,
        canvasRef.current
      );
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
    let { audioContext } = getCtx();
    if (typeof version === "number") {
      audioRef.current.processor.port.postMessage({
        cmd: "setStep",
        step: version,
      });
      if (!audioRef.current.on) {
        audioRef.current.gain.gain.setValueAtTime(
          0.0,
          audioContext.currentTime
        );
        audioRef.current.gain.gain.setTargetAtTime(
          1.0,
          audioContext.currentTime,
          0.05
        );
      }
      audioRef.current.on = true;
    } else {
      if (audioRef.current.on) {
        audioRef.current.gain.gain.setValueAtTime(
          1.0,
          audioContext.currentTime
        );
        audioRef.current.gain.gain.setTargetAtTime(
          0.0,
          audioContext.currentTime,
          0.05
        );
        audioRef.current.on = false;
      }
    }
  }, [JSON.stringify(versions), step]);

  return (
    <canvas
      ref={canvasRef}
      style={{
        position: "absolute",
        right: -30,
        top: "-9%",
        width: "30%",
        height: "35%",
      }}
    ></canvas>
  );
};
