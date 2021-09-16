import './FileList.css';
import {
  Badge,
  Button,
  Row,
  Col,
  ListGroup,
  Spinner
} from 'react-bootstrap'
import { useState } from 'react';
import { useDropzone } from 'react-dropzone';
import { BsAppIndicator, BsPlayFill, BsUpload } from 'react-icons/bs'

function FileList(props) {
  const {
    open,
    getRootProps,
    getInputProps,
    isDragActive,
    isDragAccept,
    isDragReject
  } = useDropzone({
    accept: '.exe',
    onDropAccepted: (f) => {
      if(!props.uploading) {
        props.onUpload && props.onUpload(f)
      }
    },
    noClick: true
  });
  return (
    <ListGroup className="file-list-box" {...getRootProps({isDragActive, isDragAccept, isDragReject})}>
    {
      (props.files && props.files.length != 0) ? props.files.map((f, idx) => {
        const active = props.active == f.name
        return (
          // <ListGroup.Item action key={f.name} variant={active ? "success" : undefined}>
          <ListGroup.Item action key={f.name} onClick={() => props.onSelect && props.onSelect(f)}>
            <span><BsAppIndicator /></span>{' '}
            <span>{ f.name }</span>
            <div className="file-list-item-tail">
              <Badge pill bg="success"><BsPlayFill /></Badge>{' '}
            </div>
          </ListGroup.Item>
        )
      }) : (<ListGroup.Item><div className='file-list-placeholder'>No files available.</div></ListGroup.Item>)
    }
    <ListGroup.Item>
      <input {...getInputProps()} />
      <Button variant="secondary" size="sm" onClick={open}>
        Drag 'n' drop or click to select files.
      </Button>
    </ListGroup.Item>
    <div className="file-list-overlay" style={(isDragActive && !props.uploading) ? {} : { display: 'none' }}>
      <div className="text-white">
        <BsUpload />{' '}<span>Drag 'n' drop some files here.</span>
      </div>
    </div>
    <div className="file-list-overlay" style={props.uploading ? {} : { display: 'none' }}>
      <Spinner animation="border" role="status">
        <span className="visually-hidden">Uploading...</span>
      </Spinner>
    </div>
    </ListGroup>
  );
}

export default FileList;
