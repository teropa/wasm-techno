// example Layout.js
import React from "react";

export const QuoteSlide = ({ children }) => (
  <div
    style={{
      width: "100vw",
      height: "100vh",
      padding: "40px",
      display: "flex",
      flexDirection: "column",
      justifyContent: "center",
    }}
  >
    <p style={{ fontSize: "2.4rem" }}>
      &ldquo;You'll get steamrolled if you try to compete with AI, but you can
      choose not to. ima continue to prompt my computer using extremely long and
      detailed text prompts (in C).
    </p>
    <p style={{ fontSize: "2.4rem" }}>
      Exploring gen AI is cool too. Just pointing out that art is not an arms
      race, and bleeding edge tech is neither necessary nor sufficient condition
      for making good art. It's about the social context of how/why an artist
      uses whatever tools they do, whether simple or complex.&rdquo;
    </p>
    <div>
      -{" "}
      <a
        style={{ textDecoration: "none", color: "white" }}
        href="https://x.com/_deafbeef/status/1758521702110122133"
      >
        0xDEAFBEEF
      </a>
    </div>
  </div>
);
