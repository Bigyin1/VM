import { Console } from "./console.js"
import { CodeInput } from "./codeInput.js";
import { Screen } from "./screen.js";


class VMClient {

    constructor(addr) {
        this.addr = addr

        this.console = new Console(document.getElementsByClassName("console")[0],
            "$> ", this.sendConsoleText.bind(this), "Welcome to VM")

        this.infoConsole = new Console(document.getElementsByClassName("infoConsole")[0],
            "  ", null, "Log")

        this.codeInput = new CodeInput()

        this.screen = new Screen(this.codeInput.root.offsetWidth, this.codeInput.root.offsetHeight)


        this.socket = null

        this.codeInput.button.addEventListener("click", (event) => {

            this.infoConsole.clear()
            this.screen.clear()

            if (this.socket != null) {
                this.socket.close(1000, "incoming new code")
            }

            this.socket = new WebSocket(this.addr)
            this.socket.binaryType = "arraybuffer"

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

                if (e.data instanceof ArrayBuffer) {
                    this.processBinaryMessage(e.data)
                    return
                }

                let data = JSON.parse(e.data)
                console.log("incoming data: ", data)
                this.processJSONMessage(data)
            }

        })

        document.getElementsByClassName("clearButton")[0].addEventListener("click", (e) => {
            this.console.clear()
        })

    }

    processJSONMessage(message) {
        if (message.type === "console") {
            this.console.putText(message.message, false)
            return
        }

        if (message.type === "general") {
            this.infoConsole.putText(message.message, false)
            return
        }

        if (message.type === "codedump") {
            this.infoConsole.putText("\n----------------\n", false)
            this.infoConsole.putText(message.message, false)
            this.infoConsole.putText("\n----------------\n", false)
            return
        }

        if (message.type === "error") {
            this.infoConsole.putText(message.message, true)
            return
        }

    }

    processBinaryMessage(buf) {
        let arr = new Uint8Array(buf)
        let x = arr[0] + 256 * arr[1]
        let y = arr[2] + 256 * arr[3]
        let r = arr[4]
        let g = arr[5]
        let b = arr[6]

        this.screen.drawPixel(x, y, r, g, b)
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


let cmClient = new VMClient(`ws://${window.location.host}/ws`)
