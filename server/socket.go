package main

import (
	"context"
	"log"
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
	maxMessageSize = 32000
)

type Client struct {
	conn       *websocket.Conn
	textResp   <-chan []byte
	binaryResp <-chan []byte
	req        chan<- []byte

	cancel context.CancelFunc
	ctx    context.Context
}

func (c *Client) readConn() {
	defer func() {
		c.cancel()
		close(c.req)
		c.conn.Close()
	}()

	c.conn.SetReadLimit(maxMessageSize)
	c.conn.SetReadDeadline(time.Now().Add(pongWait))
	c.conn.SetPongHandler(func(string) error { c.conn.SetReadDeadline(time.Now().Add(pongWait)); return nil })

	for {
		_, buf, err := c.conn.ReadMessage()
		if err != nil {
			if websocket.IsCloseError(err, websocket.CloseGoingAway, websocket.CloseAbnormalClosure) {
				log.Printf("websocket error: %v", err)
			}
			return
		}

		select {
		case c.req <- buf:
		case <-c.ctx.Done():
			return
		}
	}
}

func (c *Client) sendResponseWithType(message []byte, mesType int) error {

	w, err := c.conn.NextWriter(mesType)
	if err != nil {
		return err
	}
	w.Write(message)

	if err := w.Close(); err != nil {
		return err
	}

	return nil
}

func (c *Client) writeConn() {
	ticker := time.NewTicker(pingPeriod)
	defer func() {
		c.cancel()
		c.conn.Close()
		ticker.Stop()
	}()

	for {
		select {
		case message, ok := <-c.textResp:
			c.conn.SetWriteDeadline(time.Now().Add(writeWait))
			if !ok {
				c.conn.WriteMessage(websocket.CloseMessage, []byte{})
				return
			}

			if err := c.sendResponseWithType(message, websocket.TextMessage); err != nil {
				return
			}

		case message, ok := <-c.binaryResp:
			c.conn.SetWriteDeadline(time.Now().Add(writeWait))
			if !ok {
				c.conn.WriteMessage(websocket.CloseMessage, []byte{})
				return
			}

			if err := c.sendResponseWithType(message, websocket.BinaryMessage); err != nil {
				return
			}

		case <-ticker.C:
			c.conn.SetWriteDeadline(time.Now().Add(writeWait))
			if err := c.conn.WriteMessage(websocket.PingMessage, nil); err != nil {
				return
			}

		case <-c.ctx.Done():
			return

		}
	}
}
