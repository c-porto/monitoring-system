use anyhow::Result;
use axum::{extract::Extension, Router};
use sqlx::postgres::PgPoolOptions;
use std::net::SocketAddr;

mod handlers;
mod router;

#[tokio::main]
async fn main() -> Result<()> {
    let env_db = std::env::var("DATABASE_URL");

    let db_url: String;

    match env_db {
        Ok(url) => db_url = url,
        Err(_err) => {
            println!("DB URL was not set in enviroment variables");
            std::process::exit(1);
        }
    }

    let std_pool = PgPoolOptions::new()
        .max_connections(1)
        .connect(db_url.as_str())
        .await?;

    let db_exists: bool = sqlx::query_scalar(
        "SELECT EXISTS (
    SELECT 1 
    FROM pg_database 
    WHERE datname = $1
)",
    )
    .bind("ms")
    .fetch_one(&std_pool)
    .await?;

    if !db_exists {
        sqlx::query("CREATE DATABASE ms")
            .execute(&std_pool)
            .await?;
    }

    std_pool.close().await;

    let new_db_url = db_url + "/ms";

    let pool = PgPoolOptions::new()
        .max_connections(10)
        .connect(new_db_url.as_str())
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

    println!("Listening to port 42068");

    axum::Server::bind(&addr)
        .serve(routes_all.into_make_service())
        .await?;

    Ok(())
}
