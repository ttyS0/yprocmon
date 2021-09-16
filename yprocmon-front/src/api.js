import axios from 'axios'

class Api
{
  constructor(props) {
    this._client = axios.create({
      baseURL: props
    })
  }
  async files() {
    const res = await this._client.get('files')
    return res.data
  }
  async operations(after) {
    const res = await this._client.get(after ? `operations?after=${after}` : 'operations')
    return res.data
  }
  async instances() {
    const res = await this._client.get('instances')
    return res.data
  }
  async upload(file) {
    const formData = new FormData();
    formData.append("file", file);
    const res = await this._client.post('upload', formData, {
      headers: {
        "Content-Type": "multipart/form-data",
      },
    })
    return res.data;
  }
  async run(name, command) {
    const params = new URLSearchParams();
    params.append("name", name)
    params.append("command", command)
    const res = await this._client.post(`run`, params);
    return res.data;
  }
}

export default new Api('/api')