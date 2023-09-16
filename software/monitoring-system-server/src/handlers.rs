use anyhow::Result;
use axum::extract::Query;
use axum::response::IntoResponse;
use axum::Json;
use serde::{Deserialize, Serialize};

#[derive(Deserialize, Debug)]
pub struct DataParameters {
    samples: Option<u32>,
    stats: Option<bool>,
}

#[derive(Deserialize, Serialize, Debug)]
pub struct MeasurementSample {
    temp: u16,
    umidity: u16,
    light: u16,
}

#[derive(Serialize, Debug)]
#[serde()]
pub struct MeasurementsData {
    dataset: Vec<MeasurementSample>,
}

pub async fn post_measuring_handler(Json(sample): Json<MeasurementSample>) -> impl IntoResponse {
    todo!();
}

pub async fn get_statistics_handler() -> Result<Json<MeasurementSample>> {
    todo!();
}

pub async fn get_complete_data_handler(
    Query(data_param): Query<DataParameters>,
) -> Result<Json<MeasurementsData>> {
    todo!();
}
