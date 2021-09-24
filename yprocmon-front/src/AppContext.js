import React, { Context } from "react";
import { Badge } from "react-bootstrap";

const context = React.createContext();

export default context;
export const AppProvider = context.Provider;
export const initalState = {
  messages: [],
  rules: [],
  files: [],
  instances: [],
  instancesMap: {},
  operationClearAt: 0,
  isCapturing: true,
  settings: {
    server: 'http://localhost:8051'
  },
  typemaps: {
    ...Object.fromEntries(
      [
        "MessageBoxA",
        "MessageBoxW",
        "RegCreateKeyExA",
        "RegCreateKeyExW",
        "RegSetValueExA",
        "RegSetValueExW",
        "RegDeleteValueA",
        "RegDeleteValueW"
      ].map(v => [v, v.slice(0, -1)])
    )
  },
  sensitive: {
    ...Object.fromEntries(
      [
        "RegCreateKeyExA",
        "RegCreateKeyExW",
        "RegSetValueExA",
        "RegSetValueExW",
        "RegDeleteValueA",
        "RegDeleteValueW",
        "bind"
      ].map(v => [v, true])
    )
  }
};

export const processors = {
  // network: (o) => {
    
  // }
}

// export const normalizeOperations = (ops, rules) => {
//   ops.map(o => {
//     return {
//       ...o,
//       summary
//     }
//   })
// }
