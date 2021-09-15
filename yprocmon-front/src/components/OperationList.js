import "./FileList.css";
import { Container, Badge, Table, Placeholder } from "react-bootstrap";
import { useEffect, useState, useRef } from "react";
import { BsAppIndicator, BsPlayFill } from "react-icons/bs";
import { SwitchTransition, CSSTransition } from "react-transition-group";

const ListPlaceholder = Array(10).fill(0).map((_, i) => (
  <tr key={i}>
  {
    Array(5).fill(0).map((_, i) => (
      <td key={i}>
        <Placeholder as="div" animation="glow"><Placeholder xs={12} /></Placeholder>
      </td>
    ))
  }
  </tr>
))

function OperationList(props) {
  const [ready, setReady] = useState(false);
  const bottomElement = useRef(null);
  console.log('operation list inited')
  useEffect(() => {
    setReady(true);
  }, []);
  useEffect(() => {
    if(bottomElement.current) { bottomElement.current.scrollIntoView({ behavior: "smooth" }) };
  }, [props.following, props.operations, props.filters, ready, props.loading]);
  const showPlaceholder = !ready || props.loading;
  const showData = !showPlaceholder && (props.operations && props.operations != 0);
  const showText = !showPlaceholder && !showData;
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
        {
          showPlaceholder && ListPlaceholder
        }
        {
          showData && props.operations.map((o, i) => {
                return (
                  <tr variant="success" key={i}>
                    <td>{i}</td>
                    <td>{o.time}</td>
                    <td>{o.pid}</td>
                    <td>{o.display.type}</td>
                    <td>{o.display.summary}</td>
                  </tr>
                )
              }
            )
        }
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
