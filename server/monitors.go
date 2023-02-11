package main

import (
	"bufio"
	"encoding/json"
	"io"
	"log"
	"sync"
)

func (r *Runner) inputMonitor() {

	defer func() {
		r.vmConsoleWriter.Close()
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
				log.Printf("inputMonitor error: %v", err)
				return
			}

			_, err = r.vmConsoleWriter.Write([]byte(conReq.Data))
			if err != nil {
				log.Printf("inputMonitor error: %v", err)
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
	}()

	const vmMajesticConsoleScreenFrameLen = 7 // TODO config ???

	rb := bufio.NewReader(r.vmGraphicsReader)

	for {

		pointFrameBuf := make([]byte, vmMajesticConsoleScreenFrameLen)

		n, err := rb.Read(pointFrameBuf)
		if err != nil && err != io.EOF {
			log.Printf("monitorGraphicsOut error: %v", err)
			return
		}
		if n == 0 {
			return
		}
		if n != len(pointFrameBuf) {
			log.Printf("monitorGraphicsOut error: got invalid point data of size: %d", n)
			return
		}

		r.responseBinary(pointFrameBuf)

	}

}

func (r *Runner) monitorErrorsOut(wg *sync.WaitGroup) {

	defer func() {
		r.vmErrorsReader.Close()
		wg.Done()
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
