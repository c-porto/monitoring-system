FROM rust:1.77-alpine as builder

RUN apk add musl-dev openssl-dev openssl-libs-static

WORKDIR /server

COPY . .

RUN cargo build --target x86_64-unknown-linux-musl --release

# Create a new stage with a minimal image
FROM scratch

# Copying the executable from previous stage
COPY --from=builder /server/target/x86_64-unknown-linux-musl/release/monitoring-system-server /server

# Entrypoint
CMD ["/server"]

EXPOSE 42068
