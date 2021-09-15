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
}

export default new Api('/api')