export let myTheme = {
  googleFont:
    "https://fonts.googleapis.com/css2?family=Kode+Mono:wght@400..700&display=swap",
  fonts: {
    heading: '"Kode Mono", sans-serif',
    body: '"Kode Mono", sans-serif',
    monospace: '"Kode Mono", monospace',
  },
  colors: {
    background: "#000000",
    text: "#ffffff",
    primary: "#ffffff",
  },
  styles: {
    CodeSurfer: {
      pre: {
        color: "text",
        backgroundColor: "background",
      },
      code: {
        fontFamily: '"Kode Mono", monospace',
        fontWeight: "500",
        color: "text",
        backgroundColor: "background",
      },
      tokens: {
        [["comment", "prolog", "doctype", "cdata", "punctuation"].join(" ")]: {
          color: "#6c6783",
        },
        namespace: {
          opacity: 0.7,
        },
        [["tag", "operator", "number"].join(" ")]: {
          color: "#91e042",
        },
        [["property", "function"]]: {
          color: "#869afd",
        },
        [["tag-id", "selector", "atrule-id"]]: {
          color: "#eeebff",
        },
        [["attr-name"]]: {
          color: "#c4b9fe",
        },
        [[
          "boolean",
          "string",
          "entity",
          "url",
          "attr-value",
          "keyword",
          "control",
          "directive",
          "unit",
          "statement",
          "regex",
          "at-rule",
          "placeholder",
          "variable",
        ].join(" ")]: {
          color: "#ccff99",
        },
      },
      title: {
        backgroundColor: "background",
        color: "text",
      },
      subtitle: {
        color: "#d6deeb",
        backgroundColor: "rgba(10,10,10,0.9)",
      },
      unfocused: {
        // only the opacity of unfocused code can be changed
        opacity: 0.33,
      },
    },
  },
};
