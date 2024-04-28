/*
strike_permissions table lists the permissions required for a given strike pack
*/

DROP TABLE IF EXISTS strike_permissions;

/* Not using a value system because there may be accounts that want to test just web-vulnerabilites vs others who want
just iot vulnerabilities or DNS tunneling or something... not a hierarchical approach, but a granular one */
CREATE TABLE strike_permissions(
    permissions_id SERIAL PRIMARY KEY,
    name VARCHAR(100)                           /* Name of the permissions group (silver, gold, platinum, or something) */
);