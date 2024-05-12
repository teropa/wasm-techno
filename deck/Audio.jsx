import React, { useEffect } from "react";
import { useDeck } from "mdx-deck";

let ctx = new AudioContext({ sampleRate: 48000, latencyHint: 0.1 });

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

  useEffect(() => {
    let version = versions[step];
    if (!version) return;
    let stillHere = true,
      cleanUp = () => {};
    Promise.all([wasmCode, workletLoad]).then(([wasmCode]) => {
      if (!stillHere) return;
      let processor = new AudioWorkletNode(ctx, "techno-processor");
      processor.port.postMessage({ cmd: "init", wasmCode, version });
      processor.connect(ctx.destination);
      console.log("connected", ctx.state);
      cleanUp = () => {
        processor.port.postMessage({ cmd: "destroy" });
        processor.disconnect();
      };
    });
    return () => {
      stillHere = false;
      cleanUp();
    };
  }, [JSON.stringify(versions), step]);

  return <></>;
};
