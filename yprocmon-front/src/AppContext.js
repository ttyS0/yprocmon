import React, { Context } from "react";
import { Badge } from "react-bootstrap";

const context = React.createContext();

export default context;
export const AppProvider = context.Provider;
export const initalState = {
  operations: [],
  rules: [],
  files: [],
  instances: [],
//   typemaps: {
//     MessageBox: <Badge variant="info">MessageBox</Badge>,
//     CreateFile: <Badge variant="">CreateFile</Badge>,
//     WriteFile: <Badge variant="">WriteFile</Badge>,
//     ReadFile: <Badge variant="">ReadFile</Badge>,
//     HeapCreate: <Badge variant="">HeapCreate</Badge>,
//     HeapDestroy: <Badge variant="">HeapDestroy</Badge>,
//     HeapFree: <Badge variant="">HeapFree</Badge>,
//     RegCreateKeyEx: <Badge variant="">RegCreateKeyEx</Badge>,
//     RegSetValueEx: <Badge variant="">RegSetValueEx</Badge>,
//     RegDeleteValue: <Badge variant="">RegDeleteValue</Badge>,
//     RegCloseKey: <Badge variant="">RegCloseKey</Badge>,
//     RegOpenKeyEx: <Badge variant="">RegOpenKeyEx</Badge>,
//     socket: <Badge variant="">socket</Badge>,
//     bind: <Badge variant="">bind</Badge>,
//     send: <Badge variant="">send</Badge>,
//     connect: <Badge variant="">connect</Badge>,
//     recv: <Badge variant="">recv</Badge>,
//     memcpy: <Badge variant=""></Badge>,
//   },
  operationClearAt: 0,
  isCapturing: true
};
// export const normalizeOperations = (ops, rules) => {
//   ops.map(o => {
//     return {
//       ...o,
//       summary
//     }
//   })
// }
