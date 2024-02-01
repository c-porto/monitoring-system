use std::f64;

use anyhow::Result;
use axum::response::{IntoResponse, Response};
use axum::Json;
use serde::{Deserialize, Serialize};
use sqlx::prelude::FromRow;

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

impl MeasurementSample {
    fn new(time: String, temp: f64, humidity: f64, light: f64, air: f64) -> Self {
        Self {
            time,
            temp,
            humidity,
            light,
            air,
        }
    }
}

#[derive(Serialize, Debug)]
#[serde(transparent)]
pub struct MeasurementsData {
    dataset: Vec<MeasurementSample>,
}

pub async fn post_measuring_handler(Json(payload): Json<MeasurementSample>) -> impl IntoResponse {
    let data = payload;
    todo!();
}

pub async fn update_ui_handler() -> Result<Json<MeasurementSample>> {
    todo!();
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
