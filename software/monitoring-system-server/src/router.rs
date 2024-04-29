use crate::handlers;
use axum::routing::{get, post};
use axum::Router;

pub fn routes() -> Router {
    Router::new()
        .route("/", get(handlers::get_api_ref))
        .route("/api/post_ms", post(handlers::post_measuring_handler))
        .route("/api/ui_update", get(handlers::update_ui_handler))
        .route("/api/get_samples", get(handlers::get_all_data))
}
