use anyhow::Result;
use rand::Rng;
use reqwest::Client;
use serde::{Deserialize, Serialize};
use tokio;

#[derive(Deserialize, Serialize, Debug)]
struct TestData {
    temp: f64,
    humidity: f64,
    light: f64,
    air: f64,
}

#[tokio::main]
async fn main() -> Result<()> {
    let mut rng = rand::thread_rng();
    for _i in 0..50 {

        let payload = TestData {
            light: rng.gen(),
            air: rng.gen(),
            temp: rng.gen(),
            humidity: rng.gen(),
        };

        // Serialize the payload to JSON
        let json_payload = serde_json::to_string(&payload)?;

        // Specify the URL for the POST request
        let url = "http://localhost:42068/api/post_ms";

        // Create a reqwest client
        let client = Client::new();

        // Make the POST request
        let response = client
            .post(url)
            .header("Content-Type", "application/json")
            .body(json_payload)
            .send()
            .await?;

        // Check if the request was successful (status code 2xx)
        if response.status().is_success() {
            let response_body = response.text().await?;
            println!("Request was successful! Response: {:?}", response_body);
        } else {
            println!("Request failed with status code: {}", response.status());
        }
    }

    Ok(())
}
