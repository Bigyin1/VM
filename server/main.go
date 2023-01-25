package main

import (
	"context"
	"flag"
	"log"
	"net/http"
	"time"

	"github.com/gorilla/websocket"
)

const (
	// Time allowed to write a message to the peer.
	writeWait = 10 * time.Second

	// Time allowed to read the next pong message from the peer.
	pongWait = 60 * time.Second

	// Send pings to peer with this period. Must be less than pongWait.
	pingPeriod = (pongWait * 9) / 10

	// Maximum message size allowed from peer.
	maxMessageSize = 8096
)

var upgrader = websocket.Upgrader{
	ReadBufferSize:  1024,
	WriteBufferSize: 1024,
}

var addr = flag.String("addr", ":8080", "http service address")
var asmPath = flag.String("asm", "./progs/my_asm", "assermbler path")
var vmPath = flag.String("vm", "./progs/my_vm", "vm path")

func serveWs(w http.ResponseWriter, r *http.Request) {

	upgrader.CheckOrigin = func(r *http.Request) bool { return true } // remove
	conn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Println(err)
		return
	}
	respChan := make(chan []byte)
	reqChan := make(chan []byte)

	ctx, cancel := context.WithTimeout(
		context.Background(),
		time.Duration(600*time.Second))

	client := &Client{
		conn:   conn,
		resp:   respChan,
		req:    reqChan,
		ctx:    ctx,
		cancel: cancel,
	}

	runner := &Runner{
		response:   respChan,
		request:    reqChan,
		vmExePath:  *vmPath,
		asmExePath: *asmPath,
		ctx:        ctx,
		cancel:     cancel,
	}

	go client.readConn()
	go client.writeConn()

	runner.Start()
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
