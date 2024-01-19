use anyhow::Result;
use axum::Router;
use std::net::SocketAddr;

mod handlers;
mod router;

#[tokio::main]
async fn main() -> Result<()> {
    if let Ok(_) = dotenv::dotenv() {
        println!("Environment variables loaded");
    } else {
        println!("Couldn't load .env file");
    }

    let routes_all = Router::new().merge(router::routes());

    let addr: SocketAddr = "0.0.0.0:42068".parse().unwrap();

    println!("Listening to port 42068");
    axum::Server::bind(&addr)
        .serve(routes_all.into_make_service())
        .await?;

    return Ok(());
}
