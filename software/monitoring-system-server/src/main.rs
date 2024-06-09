use anyhow::Result;
use axum::{extract::Extension, Router};
use sqlx::postgres::{PgConnectOptions, PgPoolOptions};
use std::net::SocketAddr;

mod handlers;
mod router;

#[tokio::main]
async fn main() -> Result<()> {
    let port = std::env::var("DB_PORT").unwrap();
    let pswr = std::env::var("DB_PASS").unwrap();
    let db_name = std::env::var("DB_NAME").unwrap();
    let host = std::env::var("INSTANCE_HOST").unwrap();
    let user = std::env::var("DB_USER").unwrap();

    let conn_opt = PgConnectOptions::new()
        .host(host.as_str())
        .port(port.parse::<u16>().unwrap())
        .password(pswr.as_str())
        .database(db_name.as_str())
        .username(user.as_str());

    let pool = PgPoolOptions::new()
        .max_connections(10)
        .connect_with(conn_opt)
        .await?;

    sqlx::migrate!("./migrations").run(&pool).await?;

    let routes_all = Router::new().merge(router::routes()).layer(Extension(pool));
    let addr: SocketAddr = "0.0.0.0:42068".parse().unwrap();

    // Used to log a simple time reference
    //tokio::task::spawn(async {
    //loop {
    //   let local = chrono::prelude::Local::now();
    //    let date = local.format("[%Y/%m/%d - %H:%M:%S]").to_string();
    //     println!("Server time: {}", date);
    //      tokio::time::sleep(tokio::time::Duration::from_secs(10)).await;
    //   }
    //});

    axum::Server::bind(&addr)
        .serve(routes_all.into_make_service())
        .await?;

    Ok(())
}
