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

func serveHome(w http.ResponseWriter, r *http.Request) {
	log.Println(r.URL)
	if r.URL.Path != "/" {
		http.Error(w, "Not found", http.StatusNotFound)
		return
	}
	if r.Method != http.MethodGet {
		http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
		return
	}
	http.ServeFile(w, r, "home.html")
}

var addr = flag.String("addr", "127.0.0.1:8080", "http service address")
var asmPath = flag.String("asm", "./my_asm", "assermbler path")
var vmPath = flag.String("vm", "./my_vm", "vm path")

func serveWs(w http.ResponseWriter, r *http.Request) {

	upgrader.CheckOrigin = func(r *http.Request) bool { return true } // remove
	conn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Println(err)
		return
	}

	conn.SetReadLimit(maxMessageSize)
	respChan := make(chan []byte)
	reqChan := make(chan []byte)

	// client := &Client{conn: conn, resp: respChan, req: reqChan}
	ctx, cancel := context.WithTimeout(
		context.Background(),
		time.Duration(600*time.Second))

	runner := &Runner{resp: respChan, req: reqChan, vm: *vmPath,
		asm: *asmPath, ctx: ctx, cancel: cancel, conn: conn}
	runner.Start()
}

func main() {
	flag.Parse()

	// if _, err := os.Open(*asmPath); err != nil {
	// 	log.Fatalf("assembler execuable %s not exist", *asmPath)
	// 	os.Exit(1)
	// }

	// if _, err := os.Open(*vmPath); err != nil {
	// 	log.Fatalf("vm execuable %s not exist", *vmPath)
	// 	os.Exit(1)
	// }

	http.HandleFunc("/", serveHome)
	http.HandleFunc("/ws", serveWs)

	err := http.ListenAndServe(*addr, nil)
	if err != nil {
		log.Fatal("ListenAndServe: ", err)
	}
}
