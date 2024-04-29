use axum::response::IntoResponse;
use axum::{Extension, Json};
use serde::{Deserialize, Serialize};
use sqlx::prelude::FromRow;
use sqlx::PgPool;

#[derive(Deserialize, Serialize, Debug)]
pub struct ApiRef {
    version: String,
    name: String,
    project: String,
}

#[derive(Deserialize, Serialize, Debug, FromRow, Clone)]
pub struct MeasurementSample {
    time: String,
    temp: f32,
    humidity: f32,
    light: f32,
    air: f32,
}

#[derive(Deserialize, Serialize, Debug, FromRow, Clone)]
pub struct DataFromSensors {
    temp: f32,
    humidity: f32,
    light: f32,
    air: f32,
}

pub async fn post_measuring_handler(
    Extension(pool): Extension<PgPool>,
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

    let result = sqlx::query("INSERT INTO measurement_samples (time, temp, humidity, light, air) VALUES ($1, $2, $3, $4, $5)")
    .bind(sample.time)
    .bind(sample.temp)
    .bind(sample.humidity)
    .bind(sample.light)
    .bind(sample.air)
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

pub async fn update_ui_handler(Extension(pool): Extension<PgPool>) -> Json<Vec<MeasurementSample>> {
    println!("Get data endpoint hit");

    let db_samples: Result<Vec<MeasurementSample>, sqlx::Error> =
        sqlx::query_as("SELECT * FROM measurement_samples")
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

pub async fn get_all_data(Extension(pool): Extension<PgPool>) -> Json<Vec<MeasurementSample>> {
    println!("Get all data endpoint hit");

    let db_samples: Result<Vec<MeasurementSample>, sqlx::Error> =
        sqlx::query_as("SELECT * FROM measurement_samples")
            .fetch_all(&pool)
            .await;

    match db_samples {
        Ok(samples) => return Json(samples),
        Err(err) => {
            let empty: Vec<MeasurementSample> = vec![];
            println!("Postgresql Error: {}", err);
            println!("Error fetching data in database");
            return Json(empty);
        }
    }
}

pub async fn get_api_ref() -> Json<ApiRef> {
    println!("API_REFERENCE endpoint hit");

    let api_version = ApiRef {
        version: "v0.2.0".into(),
        name: "Reference API".into(),
        project: "monitoring-system".into(),
    };

    Json(api_version)
}
