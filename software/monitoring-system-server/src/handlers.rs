use std::f64;

use anyhow::Result;
use axum::response::{IntoResponse, Response};
use axum::Json;
use dotenv;
use serde::{Deserialize, Serialize};

#[derive(Deserialize, Serialize, Debug)]
pub struct ApiRef {
    version: String,
    name: String,
    project: String,
}

impl ApiRef {
    fn new(version: String, name: String, project: String) -> Self {
        Self {
            version,
            name,
            project,
        }
    }
}

#[derive(Deserialize, Serialize, Debug)]
pub struct MeasurementSample {
    temp: f64,
    humidity: f64,
    light: f64,
    air: f64,
}

impl MeasurementSample {
    fn new(temp: f64, humidity: f64, light: f64, air: f64) -> Self {
        Self {
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

    let env_version: String = match std::env::var("VERSION") {
        Ok(v) => v,
        Err(err) => {
            println!("Missing VERSION variable: {}", err);
            String::from("v0.1.0")
        }
    };

    let env_name: String = match std::env::var("NAME") {
        Ok(n) => n,
        Err(err) => {
            println!("Missing NAME variable: {}", err);
            String::from("API_REFERENCE")
        }
    };

    let env_project: String = match std::env::var("PROJECT") {
        Ok(n) => n,
        Err(err) => {
            println!("Missing PROJECT variable: {}", err);
            String::from("monitoring-system")
        }
    };

    let api_version = ApiRef::new(env_version, env_name, env_project);

    Json(api_version)
}
