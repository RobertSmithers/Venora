/*
wperm table is an isolated table that stores only the login info for the admin to make db changes
All changes must be made through db-admin-api microservice
*/

DROP TABLE IF EXISTS wperm;

CREATE TABLE wperm(
    user_id SERIAL PRIMARY KEY,
    username VARCHAR(40),
    salt VARCHAR(16),
    passhash VARCHAR(256) 
);