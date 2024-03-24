/*
strike_categories table maps which strike packs fall into which category
*/

DROP TABLE IF EXISTS strike_categories;

CREATE TABLE strike_categories(
    strike_pack_id SERIAL PRIMARY KEY,
    category_id INT REFERENCES categories(category_id),
    description VARCHAR(100)
);