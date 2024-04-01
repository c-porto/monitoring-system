use anyhow::Result;
use axum::{extract::Extension, Router};
use sqlx::postgres::PgPoolOptions;
use std::net::SocketAddr;

mod handlers;
mod router;

#[tokio::main]
async fn main() -> Result<()> {
    if let Ok(_) = dotenv::dotenv() {
        println!("Environment variables loaded");
    } else {
        eprintln!("Couldn't load .env file");
        std::process::exit(1);
    }

    let db_url = std::env::var("DATABASE_URL")?;

    let pool = PgPoolOptions::new()
        .max_connections(10)
        .connect(db_url.as_str())
        .await?;

    sqlx::migrate!("./migrations").run(&pool).await?;

    let routes_all = Router::new().merge(router::routes()).layer(Extension(pool));
    let addr: SocketAddr = "0.0.0.0:42068".parse().unwrap();

    // Used to log a simple time reference
    tokio::task::spawn(async {
        loop {
            let local = chrono::prelude::Local::now();
            let date = local.format("[%Y/%m/%d - %H:%M:%S]").to_string();
            println!("Server time: {}", date);
            tokio::time::sleep(tokio::time::Duration::from_secs(10)).await;
        }
    });

    println!("Listening to port 42068");

    axum::Server::bind(&addr)
        .serve(routes_all.into_make_service())
        .await?;

    Ok(())
}
