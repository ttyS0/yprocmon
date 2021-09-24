import axios from 'axios'

const host = '10.0.10.104'

class Api {
  websocketURL = `ws://${host}:8052`
  constructor(props) {
    this._client = axios.create({
      // baseURL: 'http://10.0.10.104:8051/api'
      // baseURL: '/api',
      baseURL: `http://${host}:8051/api`
    })
  }
  changeServer(server) {
    this._client = axios.create({
      baseURL: `${server}/api`,
    });
  }
  async files() {
    const res = await this._client.get('files')
    return res.data
  }
  async messages(after) {
    const res = await this._client.get(after ? `messages?after=${after}` : 'messages')
    return res.data
  }
  async instances() {
    const res = await this._client.get('instances')
    return res.data
  }
  async upload(file) {
    const formData = new FormData();
    formData.append('file', file);
    const res = await this._client.post('upload', formData, {
      headers: {
        'Content-Type': 'multipart/form-data',
      },
    })
    return res.data;
  }
  async run(name, command) {
    const params = new URLSearchParams();
    params.append('name', name)
    params.append('command', command)
    const res = await this._client.post(`run`, params);
    return res.data;
  }
}

export default new Api('/api')