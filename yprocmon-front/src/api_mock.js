class Api
{
    constructor(props) {
        this._prefix = props
    }
    async files() {
        return [
          { name: "a.exe", timestamp: 1631520302, size: 123456 },
          { name: "b.exe", timestamp: 1631520303, size: 123454 },
          { name: "c.exe", timestamp: 1631520304, size: 123453 },
          { name: "d.exe", timestamp: 1631520305, size: 123452 },
          { name: "e.exe", timestamp: 1631520306, size: 123451 },
          { name: "f.exe", timestamp: 1631520307, size: 123450 },
          { name: "g.exe", timestamp: 1631520308, size: 123449 },
          { name: "h.exe", timestamp: 1631520309, size: 123448 }
        ]
    }
    async operations(after) {
        return 
    }
}

return new Api('/api')