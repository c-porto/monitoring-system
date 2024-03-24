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

#[derive(Deserialize, Serialize, Debug, FromRow, Clone)]
pub struct MeasurementSample {
    time: String,
    temp: f64,
    humidity: f64,
    light: f64,
    air: f64,
}

#[derive(Deserialize, Serialize, Debug, FromRow, Clone)]
pub struct DataFromSensors {
    temp: f64,
    humidity: f64,
    light: f64,
    air: f64,
}

pub async fn post_measuring_handler(
    Extension(pool): Extension<SqlitePool>,
    Json(payload): Json<DataFromSensors>,
) -> impl IntoResponse {
    println!("Post measure endpoint hit");

    let data: DataFromSensors = payload;

    let local = chrono::prelude::Local::now();
    let time = local.format("[%Y/%m/%d - %H:%M:%S]").to_string();
    
    let sample = MeasurementSample {
        time,
        temp: data.temp,
        humidity: data.humidity,
        light: data.light,
        air: data.air,
    };

    println!("Data received: {:?}\n", sample);

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
                "Sucessfully added new sample\nRows affected: {}\n",
                res.rows_affected()
            );
            "Ok"
        }
        Err(_) => {
            println!("Failed to added new sample in database\n");
            "Error"
        }
    }
}

pub async fn update_ui_handler(
    Extension(pool): Extension<SqlitePool>,
) -> Json<Vec<MeasurementSample>> {
    println!("Get data endpoint hit");

    let db_samples = sqlx::query_as!(MeasurementSample, "SELECT * FROM measurement_samples")
        .fetch_all(&pool)
        .await;

    if let Ok(samples) = db_samples {
        let sz = samples.len();
        if sz > 100 {
            let l = sz - 100;
            let new_samples = samples[l..sz].iter().cloned().collect();
            return Json(new_samples);
        }
        return Json(samples);
    } else {
        let empty: Vec<MeasurementSample> = vec![];
        println!("Error fetching data in database");
        return Json(empty);
    }
}

pub async fn get_all_data(
    Extension(pool): Extension<SqlitePool>,
) -> Json<Vec<MeasurementSample>> {
    println!("Get all data endpoint hit");

    let db_samples = sqlx::query_as!(MeasurementSample, "SELECT * FROM measurement_samples")
        .fetch_all(&pool)
        .await;

    if let Ok(samples) = db_samples {
        return Json(samples);
    } else {
        let empty: Vec<MeasurementSample> = vec![];
        println!("Error fetching data in database");
        return Json(empty);
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
