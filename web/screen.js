'use strict';


export class Screen {


    constructor(width, height) {
        this.screen = document.getElementsByClassName("screen")[0]
        this.screen.setAttribute("width", width)
        this.screen.setAttribute("height", height)

        this.ctx = this.screen.getContext("2d")

        this.ctx.clearRect(0, 0, width, height)
        this.canvasData = this.ctx.getImageData(0, 0, width, height);


        this.drawThreshold = 10
        this.currDraw = 0
    }



    drawPixel(x, y, r, g, b) {
        let index = (x + y * this.canvasData.width) * 4;

        this.canvasData.data[index + 0] = r;
        this.canvasData.data[index + 1] = g;
        this.canvasData.data[index + 2] = b;
        this.canvasData.data[index + 3] = 255;
        this.currDraw += 1

        if (this.currDraw == this.drawThreshold) {
            this.ctx.putImageData(this.canvasData, 0, 0);
            this.currDraw = 0;
        }

    }

}
