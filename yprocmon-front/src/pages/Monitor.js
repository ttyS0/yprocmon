import "./Monitor.css";
import FileList from "../components/FileList";
import { Button, Container, Row, Col, Offcanvas } from "react-bootstrap";
import React, { useState, useEffect, useContext, useRef, useMemo } from "react";

import api from "../api";
import OperationList from "../components/OperationList";
import AppContext, { AppProvider } from "../AppContext";

function useInterval(callback, delay) {
  const savedCallback = useRef();

  useEffect(() => {
    savedCallback.current = callback;
  });

  useEffect(() => {
    function tick() {
      savedCallback.current();
    }

    let id = setInterval(tick, delay);
    return () => clearInterval(id);
  }, [delay]);
}

function Monitor(props) {
  const [canvasOpen, setCanvas] = useState(false);
  const [operationsLoading, setOperationsLoading] = useState(true);
  console.log('monitor inited')
  useEffect(() => {
    console.log('monitor rendered')
  }, [])
  const [filters, setFilters] = useState({

  })
  const { state, dispatch } = useContext(AppContext);
  const [operationsLock, setOperationsLock] = useState(false)
  const [isCapturing, setIsCapturing] = useState(true)
  const updateOperations = async () => {
    if (operationsLock) return;
    else { setOperationsLock(true); }
    // console.log(state.operations)
    const lastTimestamp = state.operations.length != 0 ? state.operations[state.operations.length - 1].timestamp : 0;
    const after = Math.max(state.operationClearAt, lastTimestamp);
    console.log(`Updating operations after ${after}.`)
    const payload = await api.operations(after)
    dispatch({
      type: "OPERATIONS_APPEND",
      payload
    });
    setOperationsLock(false)
    setOperationsLoading(false);
  }
  useEffect(() => {
    api.files().then((data) => {
      dispatch({
        type: "FILES_UPDATE",
        payload: data,
      });
    });
  }, []);
  useInterval(updateOperations, isCapturing ? 1500 : null);
  const canvasHide = () => {
    setCanvas(false);
  };
  // const OperationListMemo = React.memo(OperationList);
  return (
    <>
      <Container className="app-monitor">
        <Button
          variant="primary"
          onClick={() => alert(state.operations.length)}
        >
          Check Operations
        </Button>
        <div className="app-operation-list">
          {/* <OperationList loading={operationsLoading} operations={state.operations} /> */}
          <OperationList loading={operationsLoading} operations={state.operations} />
        </div>
      </Container>
      <Offcanvas show={canvasOpen} onHide={canvasHide}>
        <Offcanvas.Header closeButton>
          <Offcanvas.Title>Offcanvas</Offcanvas.Title>
        </Offcanvas.Header>
        <Offcanvas.Body>
          <div className="app-file-list">
            <FileList files={state.files} active="ymsgbox2.exe"></FileList>
          </div>
        </Offcanvas.Body>
      </Offcanvas>
    </>
  );
}

export default Monitor;
