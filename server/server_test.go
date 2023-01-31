package main

import (
	"bytes"
	_ "embed"
	"encoding/json"
	"flag"
	"net/http"
	"net/http/httptest"
	"strings"
	"testing"
	"time"

	"github.com/gorilla/websocket"
)

//go:embed testdata/hello.code
var helloTestCode []byte

func readResponse(t *testing.T, ws *websocket.Conn) (resp textResp, err error) {

	_, data, err := ws.ReadMessage()
	if err != nil {
		if websocket.IsCloseError(err, websocket.CloseGoingAway, websocket.CloseAbnormalClosure) {
			t.Errorf("failed to read response: %v", err)
		}
		return
	}

	if err = json.Unmarshal(data, &resp); err != nil {
		t.Errorf("failed to unmarshal response: %v", err)
		return
	}

	return
}

func readAllTextResponse(t *testing.T, ws *websocket.Conn, dataChan chan<- []byte) {

	defer close(dataChan)
	for {

		resp, err := readResponse(t, ws)
		if err != nil {
			return
		}
		dataChan <- []byte(resp.Message)
	}
}

func getResponseWithTimeout(dataChan <-chan []byte, d time.Duration) string {

	buf := bytes.Buffer{}
	timer := time.NewTimer(d)

L:
	for {
		select {
		case <-timer.C:
			break L
		case data := <-dataChan:
			buf.Write(data)
		}
	}

	return buf.String()
}

func TestWebsocketServer(t *testing.T) {

	flag.Parse()

	s := httptest.NewServer(http.HandlerFunc(serveWs))
	defer s.Close()

	u := "ws" + strings.TrimPrefix(s.URL, "http")

	ws, _, err := websocket.DefaultDialer.Dial(u, nil)
	if err != nil {
		t.Fatalf("%v", err)
	}
	defer ws.Close()

	err = ws.WriteMessage(websocket.TextMessage, helloTestCode)
	if err != nil {
		t.Errorf("failed to send code: %v", err)
		return
	}

	resp, _ := readResponse(t, ws)
	if t.Failed() {
		return
	}
	if resp.MessageType != generalMessageType || resp.Message != compileSuccessMessage {
		t.Errorf("unexpected compile success message: %v", resp)
		return
	}

	resp, _ = readResponse(t, ws)
	if t.Failed() {
		return
	}
	if resp.MessageType != generalMessageType || resp.Message != linkedSuccessMessage {
		t.Errorf("unexpected compile success message: %v", resp)
		return
	}

	dataChan := make(chan []byte)

	go readAllTextResponse(t, ws, dataChan)

	dataSection := "Hello World\n"
	if resp := getResponseWithTimeout(dataChan, 100*time.Millisecond); resp != dataSection {
		t.Errorf("got unexpected data: %s, wanted: %s", resp, dataSection)
		return
	}

	mesReq := "World Hello\n"
	req := consoleWriteReq{Data: mesReq}

	reqBuf, _ := json.Marshal(req)

	err = ws.WriteMessage(websocket.TextMessage, reqBuf)
	if err != nil {
		t.Errorf("failed to send %s: %v", mesReq, err)
		return
	}

	if resp := getResponseWithTimeout(dataChan, 100*time.Millisecond); resp != mesReq {
		t.Errorf("got unexpected data: %s, wanted: %s", resp, dataSection)
		return
	}

}
