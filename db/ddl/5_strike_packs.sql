/*
strike_categories table maps which strike packs fall into which category
*/

DROP TABLE IF EXISTS strike_packs;

CREATE TABLE strike_packs(
    strike_pack_id SERIAL PRIMARY KEY,
    category_id INT REFERENCES strike_categories(category_id), /* The strike category (2024-CVEs, zero-days, etc.) */
    permissions_group_id INT REFERENCES strike_permissions(permissions_id), /* The required permissions to view/use this strike pack (top 100 CVEs)*/
    strike_name VARCHAR(40),                            /* Display name for the strike pack */
    description VARCHAR(100)                           /* A description of the strike pack */
    /* target_type INT REFERENCES target_systems(system_id) */ /* This will need to be a separate table */
);