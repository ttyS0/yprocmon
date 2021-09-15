import React, { Context } from "react";

const context = React.createContext();

export default context;
export const AppProvider = context.Provider;
// export const normalizeOperations = (ops, rules) => {
//   ops.map(o => {
//     return {
//       ...o,
//       summary
//     } 
//   })
// }
