/*
categories table enumerates all the types of strike_pack categories
Note the prepended number is necessary for postgres to create this base table before others
    Postgres runs the files in alphabetical order
*/

DROP TABLE IF EXISTS categories;

CREATE TABLE categories (
    category_id SERIAL PRIMARY KEY,
    category_name VARCHAR(100) UNIQUE
);