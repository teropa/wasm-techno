class TechnoProcessor extends AudioWorkletProcessor {
  constructor() {
    super();
    this.port.onmessage = async ({ data: { wasmCode } }) => {
      let {
        instance: { exports },
      } = await WebAssembly.instantiate(wasmCode);
      this.wasm = exports;
      this.mem = this.wasm.memory.buffer;
      this.wasm._initialize();
    };
  }

  process(_inputs, outputs) {
    if (this.wasm) {
      let ptr = this.wasm.makeSomeTechno();
      outputs[0][0].set(new Float32Array(this.mem, ptr, 128));
    }
    return true;
  }
}

registerProcessor("techno-processor", TechnoProcessor);
