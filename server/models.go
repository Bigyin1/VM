package main

const errorMessageType = "error"
const generalMessageType = "general"
const consOutMessageType = "console"

type textResp struct {
	MessageType string `json:"type"`
	Message     string `json:"message"`
}

type point struct {
	X     uint16 `json:"x"`
	Y     uint16 `json:"y"`
	Color byte   `json:"color"`
}

type consoleWriteReq struct {
	Data string `json:"val"`
}
