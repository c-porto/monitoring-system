use anyhow::Result;
use sqlx::{migrate::MigrateDatabase, Sqlite};

pub async fn db_init(db_url: &str) -> Result<()> {
    println!("Trying to create sqlite database");

    let db_exists = Sqlite::database_exists(db_url).await.unwrap_or(false);

    if db_exists {
        println!("Database already exists");
        return Ok(());
    }

    Sqlite::create_database(db_url).await?;

    println!("Database created sucessfully");

    Ok(())
}
