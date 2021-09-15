import './FileList.css';
import {
  Badge,
  Row,
  Col,
  ListGroup
} from 'react-bootstrap'
import { useState } from 'react';
import { useDropzone } from 'react-dropzone';
import { BsAppIndicator, BsPlayFill } from 'react-icons/bs'

function FileList(props) {
  const {
    getRootProps,
    getInputProps,
    isDragActive,
    isDragAccept,
    isDragReject
  } = useDropzone({
    accept: '.exe',
    onDropAccepted: files => console.log(files)
  });
  return (
    <ListGroup>
      {
        props.files && props.files.map((f, idx) => {
          const active = props.active == f.name
          return (
            <ListGroup.Item action key={f.name} variant={active ? "success" : undefined}>
              <span><BsAppIndicator /></span>{' '}
              <span>{ f.name }</span>
              <div className="file-list-item-tail">
                {/* <Badge pill bg="primary">
                  { new Date(f.timestamp * 1000).toLocaleString() }
                </Badge>{' '} */}
                <Badge pill bg="success"><BsPlayFill /></Badge>{' '}
              </div>
            </ListGroup.Item>
          )
        })
      }
      <ListGroup.Item>
        <div {...getRootProps({})}>
          <input {...getInputProps()} />
          <p>Drag 'n' drop some files here, or click to select files</p>
        </div>
      </ListGroup.Item>
    </ListGroup>
  );
}

export default FileList;
