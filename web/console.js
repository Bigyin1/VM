
'use strict';

export class Console {

    constructor(sendTextCallback) {
        this.prompt = "$> ";
        this.root = document.getElementsByClassName("console")[0]

        this.sendText = sendTextCallback


        let greet = document.createElement("div")
        greet.innerHTML = "Welcome to VM"
        this.root.appendChild(greet)

        this.writePos = 0
        this.currentRow = null

        this.appendRow()

    }

    appendRow() {
        if (this.currentRow != null) {
            this.currentRow.removeAttribute("contenteditable")
            this.currentRow.removeEventListener("keydown", this.onKeyDown.bind(this))
            this.currentRow.onclick = null
            this.currentRow.blur()

        }

        this.writePos = 0

        let row = document.createElement("div")
        row.setAttribute("class", "row")

        this.root.appendChild(row)


        let prompt = document.createElement("span")
        prompt.setAttribute("class", "rowPrompt")
        prompt.innerHTML = this.prompt

        row.appendChild(prompt)


        this.currentRow = document.createElement("div")
        this.currentRow.setAttribute("contenteditable", "true")
        this.currentRow.setAttribute("class", "rowInput")
        this.currentRow.addEventListener("keydown", this.onKeyDown.bind(this))
        this.currentRow.onpaste = (e) => { return false }
        this.currentRow.onclick = (e) => {
            e.preventDefault()
            this.setRowCaretToEnd()
        }

        row.appendChild(this.currentRow)

        this.currentRow.focus()
        this.currentRow.scrollIntoView()
    }

    onKeyDown(event) {
        let key = event.key
        if (key === "Enter") {
            event.preventDefault();

            // TODO: line mode
            // let textToSend = this.currentRow.textContent.substring(
            //     this.writePos,
            //     this.currentRow.textContent.length
            // ) + "\n"

            this.sendText("\n")

            this.appendRow()
            return
        }
        if (key === "ArrowLeft" ||
            key === "ArrowRight" ||
            key === "ArrowUp" ||
            key === "ArrowDown" ||
            key === "Backspace") {
            event.preventDefault();
            return
        }

        if (event.key.length != 1)
            return

        this.sendText(event.key) // send immidiately
        this.writePos += 1
    }


    setRowCaretToEnd() {
        const range = document.createRange();
        const sel = window.getSelection();

        range.selectNodeContents(this.currentRow);
        range.collapse(false);
        sel.removeAllRanges();
        sel.addRange(range);

        this.currentRow.focus();
        range.detach();

        this.currentRow.scrollTop = this.currentRow.scrollHeight;
    }

    putText(text) {
        for (const l of text) {
            if (l === '\n') {
                this.appendRow()
                continue
            }
            this.currentRow.textContent += l
            this.writePos += 1

            this.setRowCaretToEnd()

        }
    }


}


