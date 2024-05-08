FROM golang:1.19
ENV SRC_DIR=/app/backend
WORKDIR $SRC_DIR


ADD . .


RUN cd server && go build -o serverVM .

WORKDIR $SRC_DIR/server

CMD ["go", "run", "."]
EXPOSE 8080
