class TechnoProcessor extends AudioWorkletProcessor {
  constructor() {
    super();
    this.gone = false;
    this.port.onmessage = async ({ data: { cmd, wasmCode, step, reset } }) => {
      if (cmd === "init") {
        let {
          instance: { exports },
        } = await WebAssembly.instantiate(wasmCode);
        this.wasm = exports;
        this.mem = this.wasm.memory.buffer;
        this.wasm._initialize();
      } else if (cmd === "setStep") {
        this.wasm?.setStep(step, reset);
      } else if (cmd === "destroy") {
        this.gone = true;
      }
    };
  }

  process(_inputs, outputs) {
    if (this.gone) return false;
    if (!this.wasm) return true;
    let ptr = this.wasm.makeSomeTechno();
    outputs[0][0].set(new Float32Array(this.mem, ptr, 128));
    return true;
  }
}

registerProcessor("techno-processor", TechnoProcessor);
