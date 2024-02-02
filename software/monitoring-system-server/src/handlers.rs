use axum::response::IntoResponse;
use axum::{Extension, Json};
use serde::{Deserialize, Serialize};
use sqlx::prelude::FromRow;
use sqlx::SqlitePool;
use std::f64;

#[derive(Deserialize, Serialize, Debug)]
pub struct ApiRef {
    version: String,
    name: String,
    project: String,
}

#[derive(Deserialize, Serialize, Debug, FromRow)]
pub struct MeasurementSample {
    time: String,
    temp: f64,
    humidity: f64,
    light: f64,
    air: f64,
}

pub async fn post_measuring_handler(
    Extension(pool): Extension<SqlitePool>,
    Json(payload): Json<MeasurementSample>,
) -> impl IntoResponse {
    println!("Post measure endpoint hit");

    let sample: MeasurementSample = payload;

    println!("Data received: {:?}", sample);

    let result = sqlx::query!(
        r#"
        INSERT INTO measurement_samples (time, temp, humidity, light, air)
        VALUES (?, ?, ?, ?, ?)
        "#,
        sample.time,
        sample.temp,
        sample.humidity,
        sample.light,
        sample.air
    )
    .execute(&pool)
    .await;

    match result {
        Ok(res) => {
            println!(
                "Sucessfully added new sample\nRows affected: {}",
                res.rows_affected()
            );
            "Ok"
        }
        Err(_) => {
            println!("Failed to added new sample in database");
            "Error"
        }
    }
}

pub async fn update_ui_handler(
    Extension(pool): Extension<SqlitePool>,
) -> Json<Vec<MeasurementSample>> {
    let db_samples = sqlx::query_as!(MeasurementSample, "SELECT * FROM measurement_samples")
        .fetch_all(&pool)
        .await;

    if let Ok(samples) = db_samples {
        Json(samples)
    } else {
        let empty: Vec<MeasurementSample> = vec![];
        println!("Error fetching data in database");
        Json(empty)
    }
}

pub async fn get_api_ref() -> Json<ApiRef> {
    println!("API_REFERENCE endpoint hit");

    let api_version = ApiRef {
        version: "v0.1.0".to_string(),
        name: "Reference API".to_string(),
        project: "monitoring-system".to_string(),
    };

    Json(api_version)
}

pub async fn get_measuring_handler() -> &'static str {
    println!("Get measure endpoint hit");

    "Please use this endpoint to post measuments"
}
