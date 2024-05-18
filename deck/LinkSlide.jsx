// example Layout.js
import React from "react";

export const LinkSlide = ({ children }) => (
  <div
    style={{
      width: "100vw",
      height: "100vh",
      padding: "40px",
      display: "flex",
      flexDirection: "column",
      alignItems: "center",
      justifyContent: "center",
      textAlign: "center",
      gap: "60px",
    }}
  >
    <h1 style={{ margin: 0 }}>Thank You</h1>
    <img style={{ width: "20%" }} src="adobe-express-qr-code.png" />
    <a
      style={{ textDecoration: "none", color: "white" }}
      href="https://github.com/teropa/wasm-techno"
    >
      github.com/teropa/wasm-techno
    </a>
  </div>
);
