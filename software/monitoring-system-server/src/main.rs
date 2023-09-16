use anyhow::Result;
use tokio::net::{TcpListener, TcpStream};
use tokio::task;

#[tokio::main]
async fn main() -> Result<()> {
    println!("Server in progress...");
    for _i in 0..10  {
        let handle = task::spawn(async move {
            println!("Hello from a task");
        });
        handle.await?;
    }
    return Ok(());
}
