use anyhow::Result;
use axum::Router;
use std::net::SocketAddr;

mod handlers;
mod router;

#[tokio::main]
async fn main() -> Result<()> {
    let routes_all = Router::new().merge(router::routes());

    let addr = SocketAddr::from(([127, 0, 0, 1], 42068));

    axum::Server::bind(&addr)
        .serve(routes_all.into_make_service())
        .await?;
    return Ok(());
}
