class TechnoProcessor extends AudioWorkletProcessor {
  constructor() {
    super();
    this.gone = false;
    this.port.onmessage = async ({ data: { cmd, wasmCode, version } }) => {
      if (cmd === "init") {
        let {
          instance: { exports },
        } = await WebAssembly.instantiate(wasmCode);
        this.wasm = exports;
        this.mem = this.wasm.memory.buffer;
        this.method = version;
        this.wasm._initialize();
      } else if (cmd === "destroy") {
        this.gone = true;
      }
    };
  }

  process(_inputs, outputs) {
    if (this.gone) return false;
    if (!this.wasm) return true;
    let ptr = this.wasm[this.method]();
    outputs[0][0].set(new Float32Array(this.mem, ptr, 128));
    return true;
  }
}

registerProcessor("techno-processor", TechnoProcessor);
