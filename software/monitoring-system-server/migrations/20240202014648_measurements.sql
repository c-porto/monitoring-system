-- Add migration script here

CREATE TABLE IF NOT EXISTS measurement_samples (
    time TEXT NOT NULL,
    temp REAL NOT NULL,
    humidity REAL NOT NULL,
    light REAL NOT NULL,
    air REAL NOT NULL
);
