import "./Monitor.css";
import FileList from "../components/FileList";
import { Button, Container, Row, Col, Offcanvas, Tabs, Tab } from "react-bootstrap";
import React, { useState, useEffect, useContext, useRef, useMemo } from "react";

import api from "../api";
import OperationList from "../components/OperationList";
import AppContext, { AppProvider } from "../AppContext";
import InstanceList from "../components/InstanceList";

import { BsListNested } from 'react-icons/bs'

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
  const [canvasOpen, setCanvasOpen] = useState(false);
  const [operationsLoading, setOperationsLoading] = useState(true);
  const [uploading, setUploading] = useState(false);
  useEffect(() => {
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
  const updateInstances = async () => {
    const payload = await api.instances()
    dispatch({
      type: "INSTANCES_UPDATE",
      payload
    });
  }
  useEffect(() => {
    api.files().then((data) => {
      dispatch({
        type: "FILES_UPDATE",
        payload: data,
      });
    });
  }, []);
  useInterval(() => {
    updateOperations()
    updateInstances()
  }, isCapturing ? 1500 : null);
  const onUpload = async (files) => {
    setUploading(true);
    await Promise.all(files.map(f => api.upload(f)))
    api.files().then((data) => {
      dispatch({
        type: "FILES_UPDATE",
        payload: data,
      });
      setUploading(false);
    });
  }
  const onFileSelect = async (file) => {
    await api.run(file.name, file.name);
    await updateInstances();
  }
  // const OperationListMemo = React.memo(OperationList);
  return (
    <>
      <Container className="app-monitor">
        <div className="app-toolbar">
          <Button
            variant="primary"
            onClick={() => setCanvasOpen(true)}>
            <BsListNested />{' '}Task Manager
          </Button>
        </div>
        <div className="app-operation-list">
          {/* <OperationList loading={operationsLoading} operations={state.operations} /> */}
          <OperationList loading={operationsLoading} operations={state.operations} />
        </div>
      </Container>
      <Offcanvas show={canvasOpen} onHide={() => setCanvasOpen(false)}>
        <Offcanvas.Header closeButton>
          <Offcanvas.Title>Task Manager</Offcanvas.Title>
        </Offcanvas.Header>
        <Offcanvas.Body>
          <Tabs defaultActiveKey="files">
            <Tab eventKey="files" title="Files">
              <FileList uploading={uploading} files={state.files} active="ymsgbox2.exe" onUpload={onUpload} onSelect={onFileSelect}></FileList>
            </Tab>
            <Tab eventKey="instances" title="Instances">
              <InstanceList instances={state.instances}  onSelect={f => alert(f)}></InstanceList>
            </Tab>
          </Tabs>
        </Offcanvas.Body>
      </Offcanvas>
    </>
  );
}

export default Monitor;
