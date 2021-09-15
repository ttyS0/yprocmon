import logo from './yprocmon.png'
import './App.css';
import {
  Badge,
  Container,
  Row,
  Col,
  OverlayTrigger,
  Tab,
  Tooltip
} from 'react-bootstrap'
import {
  BrowserRouter as Router,
  Switch,
  Route,
  Link,
  useRouteMatch,
  useParams
} from 'react-router-dom';

import NavBar from './components/NavBar'
import About from './pages/About';
import Rules from './pages/Rules';
import Monitor from './pages/Monitor';

import { AppProvider } from './AppContext'
import { useReducer } from 'react';
import Footer from './components/Footer';

function App() {
  const [state, dispatch] = useReducer((state, action) => {
    switch(action.type) {
      case 'OPERATIONS_APPEND':
        if (!action.payload || action.payload.length == 0) return state;
        const newOperations = action.payload.map((operation) => {
          const { type, data } = operation
          let display = {
            type,
            summary: ''
          }
          let summary
          if (type == 'SPAWN') {
            display.type = (<Badge bg="dark">{type}</Badge>);
            display.summary = (
              <>
                <Badge bg="dark">process: {data.process}</Badge>{' '}
                <Badge bg="dark">pid: {data.pid}</Badge>
              </>
            )
          } else if (type == 'HOOK') {
            display.type = (<Badge bg="primary">{data.name}</Badge>);
            display.summary = data.args.map(arg => [
              <OverlayTrigger
                placement="bottom"
                delay={{ show: 250, hide: 400 }}
                overlay={(props) => <Tooltip {...props}>{arg.value}</Tooltip>}
              >
                <Badge bg="light" text="dark" key={arg.name}>
                  <span>{arg.name}:</span>
                  <Badge pill bg="info">{arg.value.length > 37 ? arg.value.substr(0, 37) + '...' : arg.value}</Badge>
                </Badge>
              </OverlayTrigger>,
              ' '
            ])
          }
          return {
            ...operation,
            display
          }
        })
        return {
          ...state,
          operations: state.operations.concat(newOperations)
        }
        break;
      case 'OPERATIONS_CLEAR':
        return {
          ...state,
          operationClearAt: (new Date).getTime(),
          operations: []
        }
        break;
      case 'FILES_UPDATE':
        return {
          ...state,
          files: action.payload
        }
        break;
    }
  }, {
    operations: [],
    rules: [],
    files: [],
    instances: [],
    operationClearAt: 0
  })
  // const operationTimer = setInterval(async () => {
  //   let newOperations;
  //   if (operations.length != 0) {
  //     newOperations = await api.operations(
  //       operations[operations.length - 1].timestamp
  //     );
  //   } else {
  //     newOperations = await api.operations();
  //   }
  // }, 500);
  return (
    <Router>
      <AppProvider value={{state, dispatch}}>
        <div className="app">
          <header className="app-header">
            <NavBar logo={logo} title="yprocmon" version="v1.0.0" />
          </header>
          <div className="app-main">
            <Switch>
              <Route exact path="/">
                <Monitor />
              </Route>
              <Route path="/about">
                <About />
              </Route>
              <Route path="/rules">
                <Rules />
              </Route>
            </Switch>
            {/* <Tab.Content>
              <Tab.Pane eventKey="monitor">
                <Monitor />
              </Tab.Pane>
              <Tab.Pane eventKey="rules">
                <Rules />
              </Tab.Pane>
              <Tab.Pane eventKey="about">
                <About />
              </Tab.Pane>
            </Tab.Content> */}
          </div>
          <footer>
            <Footer />
          </footer>
        </div>
      </AppProvider>
    </Router>
  );
}

export default App;
