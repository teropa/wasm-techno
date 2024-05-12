export class SpectrumAnalyser {
  constructor(audioCtx, node, canvas) {
    this.audioCtx = audioCtx;
    this.canvas = canvas;

    this.analyser = audioCtx.createAnalyser();
    this.analyser.fftSize = 16384;
    this.analyser.smoothingTimeConstant = 0.5;
    node.connect(this.analyser);
    this.analysisData = new Uint8Array(this.analyser.frequencyBinCount);

    let maxLog = Math.log10(audioCtx.sampleRate / 2) - 1;

    this.canvas.width = this.canvas.offsetWidth * window.devicePixelRatio;
    this.canvas.height = this.canvas.offsetHeight * window.devicePixelRatio;
    this.analysisXs = this.calculateAnalysisXs(maxLog);
    this.resizeObserver = new ResizeObserver(() => {
      this.canvas.width = this.canvas.offsetWidth * window.devicePixelRatio;
      this.canvas.height = this.canvas.offsetHeight * window.devicePixelRatio;
      this.analysisXs = this.calculateAnalysisXs(maxLog);
    });
    this.resizeObserver.observe(this.canvas);
  }

  calculateAnalysisXs(maxLog) {
    return Array.from(this.analysisData).map((_, i) => {
      let freq =
        (i / this.analysisData.length) * (this.audioCtx.sampleRate / 2);
      return Math.floor(((Math.log10(freq) - 1) / maxLog) * this.canvas.width);
    });
  }

  analyse() {
    let frame = () => {
      if (this.disposed) return;
      this.analyser.getByteFrequencyData(this.analysisData);
      this.draw();
      requestAnimationFrame(frame);
    };
    requestAnimationFrame(frame);
  }

  draw() {
    let w = this.canvas.width,
      h = this.canvas.height,
      yScale = this.canvas.height / 255;

    let ctx = this.canvas.getContext("2d");
    if (!ctx) throw new Error("Could not get a canvas context!");

    ctx.clearRect(0, 0, w, h);

    let path = new Path2D();
    path.moveTo(0, h);
    for (let i = 0; i < this.analysisData.length; i++) {
      let y = Math.floor(h - this.analysisData[i] * yScale);
      path.lineTo(this.analysisXs[i], y);
    }
    path.lineTo(w, h);

    ctx.fillStyle = "rgba(50, 50, 50, 0.7)";
    ctx.fill(path);
  }

  dispose() {
    this.disposed = true;
    this.analyser.disconnect();
    this.resizeObserver.disconnect();
    let ctx = this.canvas.getContext("2d");
    if (ctx) ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);
  }
}
