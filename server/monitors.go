package main

import (
	"bufio"
	"encoding/binary"
	"encoding/json"
	"io"
	"log"
	"sync"
	"unsafe"
)

func (r *Runner) readMonitor() {

	defer func() {
		r.vmConsoleWriter.Close()
		log.Printf("readMonitor done")
	}()

	for {

		select {
		case buf, ok := <-r.request:
			if !ok {
				return
			}

			conReq := consoleWriteReq{}

			err := json.Unmarshal(buf, &conReq)
			if err != nil {
				log.Printf("readMonitor error: %v", err)
				return
			}

			_, err = r.vmConsoleWriter.Write(append([]byte(conReq.Data), '\n'))
			if err != nil {
				log.Printf("readMonitor error: %v", err)
				return
			}

		case <-r.ctx.Done():
			return

		}

	}

}

func (r *Runner) monitorConsoleOut(wg *sync.WaitGroup) {

	defer func() {
		r.vmConsoleReader.Close()
		wg.Done()
		log.Printf("monitorConsoleOut done")
	}()

	consBuf := make([]byte, 128)
	for {

		n, err := r.vmConsoleReader.Read(consBuf)
		if err != nil && err != io.EOF {
			log.Printf("monitorConsoleOut error: %v", err)
			return
		}
		if n == 0 {
			return
		}

		consData := textResp{MessageType: consOutMessageType,
			Message: string(consBuf[0:n])}

		r.responseJson(consData) // async ??

	}

}

func (r *Runner) monitorGraphicsOut(wg *sync.WaitGroup) {

	defer func() {
		r.vmGraphicsReader.Close()
		wg.Done()
		log.Printf("monitorGraphicsOut done")

	}()

	currPoint := point{}
	sz := unsafe.Sizeof(currPoint.X) + unsafe.Sizeof(currPoint.Y) + unsafe.Sizeof(currPoint.Color)

	graphicsBuf := make([]byte, sz)
	for {

		n, err := r.vmGraphicsReader.Read(graphicsBuf)
		if err != nil && err != io.EOF {
			log.Printf("monitorGraphicsOut error: %v", err)
			return
		}
		if n == 0 {
			return
		}

		if n != len(graphicsBuf) {
			log.Printf("monitorGraphicsOut error: got invalid point data of size: %d", n)
			return
		}

		currPoint.X = binary.LittleEndian.Uint16(graphicsBuf)
		currPoint.Y = binary.LittleEndian.Uint16(graphicsBuf[2:])
		currPoint.Color = graphicsBuf[4]

		r.responseJson(currPoint)

	}

}

func (r *Runner) monitorErrorsOut(wg *sync.WaitGroup) {

	defer func() {
		r.vmErrorsReader.Close()
		wg.Done()
		log.Printf("monitorErrorsOut done")
	}()

	errScanner := bufio.NewScanner(r.vmErrorsReader)

	for errScanner.Scan() {

		errBuf := errScanner.Bytes()

		errMes := textResp{
			MessageType: errorMessageType,
			Message:     string(errBuf),
		}
		r.responseJson(errMes)
	}

	if errScanner.Err() != nil {
		log.Printf("monitorErrorsOut error: %v", errScanner.Err())
	}

}
