import "./FileList.css";
import {
  Container,
  Badge,
  Table,
  OverlayTrigger,
  Tooltip,
  Placeholder,
} from "react-bootstrap";
import { useEffect, useState, useRef } from "react";
import { BsAppIndicator, BsPlayFill } from "react-icons/bs";
import { SwitchTransition, CSSTransition } from "react-transition-group";
import { AutoSizer, List, Column } from "react-virtualized";
import "react-virtualized/styles.css";

const ListPlaceholder = Array(10)
  .fill(0)
  .map((_, i) => (
    <tr key={i}>
      {Array(5)
        .fill(0)
        .map((_, i) => (
          <td key={i}>
            <Placeholder as="div" animation="glow">
              <Placeholder xs={12} />
            </Placeholder>
          </td>
        ))}
    </tr>
  ));

const typemaps = {
  spawn: <Badge bg="dark">spawn</Badge>,
  MessageBox: <Badge variant="info">MessageBox</Badge>,
  ...Object.fromEntries(
    ["CreateFile", "WriteFile", "ReadFile"].map((v) => [
      v,
      <Badge variant="info">{v}</Badge>,
    ])
  ),
  ...Object.fromEntries(
    ["HeapCreate", "HeapDestroy", "HeapFree"].map((v) => [
      v,
      <Badge variant="info">{v}</Badge>,
    ])
  ),
  ...Object.fromEntries(
    [
      "RegCreateKeyEx",
      "RegSetValueEx",
      "RegDeleteValue",
      "RegCloseKey",
      "RegOpenKeyEx",
    ].map((v) => [v, <Badge variant="info">{v}</Badge>])
  ),
  ...Object.fromEntries(
    ["socket", "bind", "send", "connect", "recv"].map((v) => [
      v,
      <Badge variant="info">{v}</Badge>,
    ])
  ),
  memcpy: <Badge variant="info">memcpy</Badge>,
};

function OperationList(props) {
  const [ready, setReady] = useState(false);
  const bottomElement = useRef(null);
  useEffect(() => {
    setReady(true);
  }, []);
  useEffect(() => {
    if (bottomElement.current) {
      bottomElement.current.scrollIntoView({ behavior: "smooth" });
    }
  }, [props.following, props.operations, props.filters, ready, props.loading]);
  const showPlaceholder = !ready || props.loading;
  const showData =
    !showPlaceholder && props.operations && props.operations != 0;
  const showText = !showPlaceholder && !showData;
  const rowRenderer = ({ key, index, isScrolling, isVisible, style }) => {
    const o = props.operations[index];
    return (
      <tr key={key} style={style}>
        <td>{o.index}</td>
        <td>{o.time}</td>
        <td>{o.pid}</td>
        <td>{typemaps[o.display.type] || o.display.type}</td>
        <td>
          {typeof o.display.summary == "string"
            ? o.display.summary
            : o.display.summary.map((s) => (
                <OverlayTrigger
                  placement="bottom"
                  delay={{ show: 250, hide: 400 }}
                  overlay={(props) => <Tooltip {...props}>{s.tooltip}</Tooltip>}
                >
                  <Badge bg="light" text="dark" key={s.key}>
                    <span>{s.key}:</span>
                    <Badge pill bg="info">
                      {s.value}
                    </Badge>
                  </Badge>
                </OverlayTrigger>
              ))}
        </td>
      </tr>
    );
  };
  const rowGetter = ({ index }) => {
    const o = props.operations[index];
    return {
      ...o,
      type: typemaps[o.display.type],
      summary: o.display.summary.map((s) => (
        <OverlayTrigger
          placement="bottom"
          delay={{ show: 250, hide: 400 }}
          overlay={(props) => <Tooltip {...props}>{s.tooltip}</Tooltip>}
        >
          <Badge bg="light" text="dark" key={s.key}>
            <span>{s.key}:</span>
            <Badge pill bg="info">
              {s.value}
            </Badge>
          </Badge>
        </OverlayTrigger>
      )),
    };
  };
  // return (
  //   <div>
  //     <AutoSizer>
  //       {({ height, width }) => (
  //         <Table
  //           width={width}
  //           height={height}
  //           headerHeight={20}
  //           rowHeight={50}
  //           rowCount={props.operations.length}
  //           rowGetter={rowGetter}
  //         >
  //           {/* <Column label="ID" dataKey="index" /> */}
  //           <Column label="Time" dataKey="time" width={300} />
  //           <Column label="PID" dataKey="pid" width={100} />
  //           <Column label="Type" dataKey="type" width={100} />
  //           <Column label="Summary" dataKey="summary" width={100} />
  //           {/* <Column width={200} label="Description" dataKey="description" /> */}
  //         </Table>
  //       )}
  //     </AutoSizer>
  //   </div>
  // );
  // return (
  // <Container>
  //   {/* <div style={{ display: "flex", flexDirection: "column" }}> */}
  //   <Table hover>
  //     <thead>
  //       <tr>
  //         <th>ID</th>
  //         <th>Time</th>
  //         <th>Process</th>
  //         <th>Type</th>
  //         <th>Summary</th>
  //       </tr>
  //     </thead>
  //     <tbody>
  //       {showPlaceholder && ListPlaceholder}
  //       <div>
  //       {showData && (
  //         <AutoSizer>
  //           {({ height, width }) => (
  //             <List
  //               width={width}
  //               height={height}
  //               rowCount={props.operations.length}
  //               rowHeight={20}
  //               rowRenderer={rowRenderer}
  //             />
  //           )}
  //         </AutoSizer>
  //       )}
  //       </div>
  //     </tbody>
  //   </Table>
  //   {showText && (
  //     <div style={{ textAlign: "center" }}>No operations are recorded.</div>
  //   )}
  //   <div ref={bottomElement}></div>
  // </Container>
  // );
  return (
    <Container>
      {/* <div style={{ display: "flex", flexDirection: "column" }}> */}
      <Table hover>
        <thead>
          <tr>
            <th>ID</th>
            <th>Time</th>
            <th>Process</th>
            <th>Type</th>
            <th>Summary</th>
          </tr>
        </thead>
        <tbody>
          {showPlaceholder && ListPlaceholder}
          {showData && props.operations.map(o => {
            // if (o.type == 'HOOK') return [];
            return(
              <tr key={o.index}>
                <td>{o.index}</td>
                <td>{o.time}</td>
                <td>{o.pid}</td>
                <td>{typemaps[o.display.type] || o.display.type}</td>
                <td>
                  {
                    typeof o.display.summary == "string"
                    ? o.display.summary
                    : o.display.summary.map((s) => [
                      <OverlayTrigger
                        placement="bottom"
                        delay={{ show: 250, hide: 400 }}
                        overlay={(props) => <Tooltip {...props}>{s.tooltip}</Tooltip>}
                      >
                        <Badge bg="light" text="dark" key={s.key}>
                          <span>{s.key}:</span>
                          <Badge pill bg="info">
                            {s.value}
                          </Badge>
                        </Badge>
                      </OverlayTrigger>,
                      ' '
                    ])
                  }
                </td>
              </tr>
            )
          })}
        </tbody>
      </Table>
      {showText && (
        <div style={{ textAlign: "center" }}>No operations are recorded.</div>
      )}
      <div ref={bottomElement}></div>
    </Container>
  );
}

export default OperationList;
