use crate::handlers;
use axum::routing::{get, post};
use axum::Router;

pub fn routes() -> Router {
    Router::new()
        .route("/api", get(handlers::get_api_ref))
        .route("/api/post_ms", get(handlers::get_measuring_handler))
        .route("/api/post_ms", post(handlers::post_measuring_handler))
        .route("/api/get_samples", get(handlers::update_ui_handler))
}
