package main

const errorMessageType = "error"
const generalMessageType = "general"
const consOutMessageType = "console"

type textResp struct {
	MessageType string `json:"type"`
	Message     string `json:"message"`
}

type consoleWriteReq struct {
	Data string `json:"val"`
}
