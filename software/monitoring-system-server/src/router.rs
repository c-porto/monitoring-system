use crate::handlers;
use axum::routing::{get, post};
use axum::Router;

pub fn routes() -> Router {
    return Router::new().route("/api", get(handlers::get_api_ref));
}
