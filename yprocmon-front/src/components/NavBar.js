import {
  Nav,
  Navbar,
  NavDropdown,
  Container
} from 'react-bootstrap'
import {
  Link,
} from 'react-router-dom';

function NavBar(props)
{
  return (
    <Navbar bg="dark" variant="dark">
      <Container>
        <Navbar.Brand href="#home">
            <img
            alt=""
            src={props.logo}
            width="30"
            height="30"
            className="d-inline-block align-top"
            />{' '}
        {props.title}
        </Navbar.Brand>
        <Navbar.Toggle />
        <Navbar.Collapse className="justify-content-end">
          <Nav className="me-auto">
            <Nav.Link as={Link} to="/">Monitor</Nav.Link>
            <Nav.Link as={Link} to="/rules">Rules</Nav.Link>
            <Nav.Link as={Link} to="/about">About</Nav.Link>
            {/* <Nav.Item>
              <Nav.Link eventKey="monitor">Monitor</Nav.Link>
            </Nav.Item>
            <Nav.Item>
            <Nav.Link eventKey="rules">Rules</Nav.Link>
            </Nav.Item>
            <Nav.Item>
            <Nav.Link eventKey="about">About</Nav.Link>
            </Nav.Item> */}
          </Nav>
          <Navbar.Text>{ props.version }</Navbar.Text>
        </Navbar.Collapse>
      </Container>
    </Navbar>
  )
}

export default NavBar