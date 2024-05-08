FROM danger89/cmake AS builder
ENV SRC_DIR=/app/backend
WORKDIR $SRC_DIR

ADD . .

RUN cmake -B ./build -DCMAKE_BUILD_TYPE=Release
RUN cmake --build ./build --config Debug --target install
RUN cmake --build ./build --config Debug --target BuildServerStdLib


FROM golang:1.19
ENV SRC_DIR=/app/backend
WORKDIR $SRC_DIR

COPY --from=builder $SRC_DIR/ ./

WORKDIR $SRC_DIR/server

CMD ["go", "run", "."]
EXPOSE 8080
