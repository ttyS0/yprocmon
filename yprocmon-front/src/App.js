import logo from "./yprocmon.png";
import "./App.css";
import {
  Badge,
  Container,
  Row,
  Col,
  OverlayTrigger,
  Tab,
  Tooltip,
} from "react-bootstrap";
import {
  BrowserRouter as Router,
  Switch,
  Route,
  Link,
  useRouteMatch,
  useParams,
} from "react-router-dom";
import Ws from 'react-websocket';

import NavBar from "./components/NavBar";
import About from "./pages/About";
import Rules from "./pages/Rules";
import Monitor from "./pages/Monitor";

import { AppProvider, initalState } from "./AppContext";
import { useReducer } from "react";
import Footer from "./components/Footer";
import api from "./api";

function App() {
  const [state, dispatch] = useReducer((state, action) => {
    switch (action.type) {
      case "MESSAGES_APPEND":
        if (!action.payload || action.payload.length == 0) return state;
        const newMessages = action.payload.map((m) => {
          const { type, data } = m;
          let display = {
            type,
            summary: "",
          };
          let summary;
          let severe = null;
          if (type == "SPAWN") {
            display.type = 'spawn'
            display.summary = [
              {
                key: 'process',
                value: data.process
              },
              {
                key: 'pid',
                value: data.pid
              }
            ];
          } else if (type == "HOOK") {
            display.type = data.name;
            display.summary = data.args.map((arg) => ({
              key: arg.name,
              value:
                arg.value.length > 37
                  ? arg.value.substr(0, 37) + "..."
                  : arg.value,
              tooltip: arg.value
            }));
            const getArg = (a) => data.args.filter(x => x.name == a)[0].value
            if (state.sensitive[data.name]) {
              severe = 'Sensitive operations.'
            }
            else if (data.name == 'send' && getArg('data').startsWith('HTTP/1.1'))
            {
              severe = 'HTTP contents detected.'
            }
          }
          return {
            ...m,
            display,
            severe
            // expert,
            // severity
          };
        });
        return {
          ...state,
          messages: state.messages.concat(newMessages),
        };
        break;
      case "MESSAGES_CLEAR":
        return {
          ...state,
          messagesClearAt: new Date().getTime(),
          messages: [],
        };
        break;
      case "INSTANCES_UPDATE":
        return {
          ...state,
          instances: action.payload,
          instancesMap: Object.fromEntries(action.payload.map(instance => [instance.pid, instance.name]))
        };
        break;
      case "FILES_UPDATE":
        return {
          ...state,
          files: action.payload,
        };
        break;
    }
  }, initalState);
  // const operationTimer = setInterval(async () => {
  //   let newOperations;
  //   if (messages.length != 0) {
  //     newOperations = await api.messages(
  //       messages[messages.length - 1].timestamp
  //     );
  //   } else {
  //     newOperations = await api.messages();
  //   }
  // }, 500);
  return (
    <Router>
      <AppProvider value={{ state, dispatch }}>
        <div className="app">
          <Ws url={api.websocketURL} onMessage={async (m) => {
            // try {JSON.parse(await m.text())} catch(e) { console.log(e, m) }
            let messages = JSON.parse(await m.text());
            if (!Array.isArray(messages)) { messages = [messages] }
            dispatch({
              type: 'MESSAGES_APPEND',
              payload: messages
            });
          }}></Ws>
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
