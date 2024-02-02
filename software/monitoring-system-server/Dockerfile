FROM rust:buster AS base

WORKDIR /server
RUN cargo init
COPY Cargo.toml /server/Cargo.toml
RUN cargo fetch
COPY . /server

FROM base AS builder

RUN cargo build --release

FROM debian:buster 

EXPOSE 42068

COPY --from=builder /server/migrations/ /migrations
COPY --from=builder /server/target/release/monitoring-system-server /server
COPY --from=builder /server/.env /.env

CMD [ "/server" ]
