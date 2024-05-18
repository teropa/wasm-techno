// example Layout.js
import React from "react";

export const TitleSlide = ({ children }) => (
  <div
    style={{
      width: "100vw",
      height: "100vh",
      padding: "40px",
      display: "flex",
      flexDirection: "column",
      justifyContent: "space-between",
    }}
  >
    <h1 style={{ margin: 0 }}>
      Making Minimal Techno with WebAssembly And a C Compiler
    </h1>
    <div
      style={{
        display: "grid",
        gridTemplateColumns: "max-content max-content",
        gridAutoRows: "1fr",
        gridColumnGap: "40px",
      }}
    >
      <h2 style={{ margin: 0 }}>Tero Parviainen</h2>
      <h2 style={{ margin: 0 }}>
        <a
          style={{ textDecoration: "none", color: "white" }}
          href="https://twitter.com/teropa"
        >
          @teropa
        </a>
      </h2>
      <h2 style={{ margin: 0 }}>Counterpoint</h2>
      <h2 style={{ margin: 0 }}>
        <a
          style={{ textDecoration: "none", color: "white" }}
          href="https://ctpt.co"
        >
          ctpt.co
        </a>
      </h2>
    </div>
  </div>
);
