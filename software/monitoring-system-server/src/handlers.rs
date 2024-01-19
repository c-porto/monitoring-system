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
    pub fn new(version: String, name: String, project: String) -> Self {
        Self {
            version,
            name,
            project,
        }
    }
}

#[derive(Deserialize, Serialize, Debug)]
pub struct MeasurementSample {
    temp: u16,
    umidity: u16,
    light: u16,
}

#[derive(Serialize, Debug)]
#[serde(transparent)]
pub struct MeasurementsData {
    dataset: Vec<MeasurementSample>,
}

pub async fn post_measuring_handler(Json(sample): Json<MeasurementSample>) -> impl IntoResponse {
    todo!();
}

pub async fn get_statistics_handler() -> Result<Json<MeasurementSample>> {
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
