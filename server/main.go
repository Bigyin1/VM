package main

import (
	"context"
	"flag"
	"log"
	"net/http"
	"time"

	"github.com/gorilla/websocket"
)

var upgrader = websocket.Upgrader{
	ReadBufferSize:  1024,
	WriteBufferSize: 1024,
}

func serveWs(w http.ResponseWriter, r *http.Request) {

	conn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Println(err)
		return
	}
	respChan := make(chan []byte)
	reqChan := make(chan []byte)
	binaryRespChan := make(chan []byte)

	ctx, cancel := context.WithTimeout(
		context.Background(),
		time.Duration(600*time.Second))

	client := &Client{
		conn:       conn,
		textResp:   respChan,
		binaryResp: binaryRespChan,
		req:        reqChan,
		ctx:        ctx,
		cancel:     cancel,
	}

	runner := &Runner{
		textResponse:   respChan,
		binaryResponse: binaryRespChan,
		request:        reqChan,
		vmExePath:      *vmPath,
		asmExePath:     *asmPath,
		ldExePath:      *ldPath,
		readobjExePath: *readobjPath,
		ctx:            ctx,
	}

	go client.readConn()
	go client.writeConn()

	go runner.Start()
}

const staticPath = "../web/" // TODO

func serveStatic(w http.ResponseWriter, r *http.Request) {
	log.Println(r.URL.Path)

	if "."+r.URL.Path == "./" {
		http.ServeFile(w, r, staticPath+"index.html")
		return
	}
	http.ServeFile(w, r, staticPath+r.URL.Path)
}

func main() {
	flag.Parse()

	http.HandleFunc("/", serveStatic)
	http.HandleFunc("/ws", serveWs)

	err := http.ListenAndServe(*addr, nil)
	if err != nil {
		log.Fatal("ListenAndServe: ", err)
	}
}
