'use strict';


export class CodeInput {


    constructor() {
        this.root = document.getElementsByClassName("codeInput")[0]
        // this.root.textContent = ""

        this.button = document.getElementsByClassName("sendButton")[0]
    }




    getText() {
        return this.root.value
    }
}
