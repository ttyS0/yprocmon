import { Form } from "react-bootstrap";

function Settings(props) {
  return (
    <Form>
      <Form.Group as={Row} className="mb-3" controlId="formServer">
        <Form.Label column sm="2">
          Email
        </Form.Label>
        <Col sm="10">
          <Form.Control plaintext readOnly defaultValue="@" />
        </Col>
      </Form.Group>
    </Form>
  );
}

export default Settings;
