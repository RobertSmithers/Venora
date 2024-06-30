/*
account table holds all data for a user (including auth token)
Note the prepended number is necessary for postgres to create this base table before others
    Postgres runs the files in alphabetical order
*/
-- TODO: Add pgcrypto to salt & hash tokens

-- Also note that to see these tables, you will have to connect (\c in psql) to VenoraDB
DROP TABLE IF EXISTS account;

CREATE TABLE account(
    account_id SERIAL PRIMARY KEY,
    username varchar(40) NOT NULL UNIQUE,
    email varchar(60) UNIQUE,
    token varchar(40) NOT NULL UNIQUE
);