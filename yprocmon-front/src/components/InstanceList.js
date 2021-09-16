import './InstanceList.css';
import {
  Badge,
  Button,
  Row,
  Col,
  ListGroup,
  Spinner
} from 'react-bootstrap'
import { useState } from 'react';
import { BsAppIndicator, BsPlayFill, BsUpload } from 'react-icons/bs'

function InstanceList(props) {
  return (
    <ListGroup className="instance-list-box">
    {
      (props.instances && props.instances.length != 0) ? props.instances.sort((x, y) => x.timestamp - y.timestamp).map((f, idx) => {
        return (
          <ListGroup.Item variant={f.status == "exited" ? "danger" : "success"} key={f.pid} onClick={() => props.onSelect && props.onSelect(f)}>
            <span><BsAppIndicator /></span>{' '}
            <span>{ f.name }</span>
            <div className="instance-list-item-tail">
              {
                f.status == "exited" ? <Badge pill bg="danger">Exitcode: { f.exitCode }</Badge> : <Badge pill bg="success">PID: { f.pid }</Badge>
              }
            </div>
          </ListGroup.Item>
        )
      }) : (<ListGroup.Item><div className='instance-list-placeholder'>No instances.</div></ListGroup.Item>)
    }
    </ListGroup>
  );
}

export default InstanceList;
