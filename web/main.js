import { Console } from "./console.js"
import { CodeInput } from "./codeInput.js";


class VMClient {

    constructor(addr) {
        this.addr = addr
        this.console = new Console(this.sendConsoleText.bind(this))

        this.codeInput = new CodeInput()

        this.socket = null

        this.codeInput.button.addEventListener("click", (event) => {

            if (this.socket != null) {
                this.socket.close(1000, "incoming new code")
            }

            this.socket = new WebSocket(this.addr)
            this.socket.onopen = (e) => {
                console.log("connection opened")
                console.log("sending code: ", this.codeInput.getText())
                this.socket.send(this.codeInput.getText())
            }

            this.socket.onerror = (err) => {
                console.log("websocket error")
            }

            this.socket.onclose = (e) => {
                console.log("websocket closed")
            }

            this.socket.onmessage = (e) => {
                let data = JSON.parse(e.data)
                console.log("incoming data: ", data)
                this.processMessage(data)
            }


        })

    }

    processMessage(message) {
        if (message.type === "console") {
            this.console.putText(message.message)
            return
        }
    }


    sendConsoleText(text) {
        if (this.socket == null) return
        if (this.socket.readyState != WebSocket.OPEN) return


        let toSend = { val: text }
        console.log("sending data: ", toSend)

        let data = JSON.stringify(toSend)
        this.socket.send(data)
    }


}


let cmClient = new VMClient("ws://127.0.0.1:8080/ws")
