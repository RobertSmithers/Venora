/*
account_strike_auth table holds the mapping of which accounts have access to which categories of strike packs
*/

DROP TABLE IF EXISTS account_strike_auth;

CREATE TABLE account_strike_auth(
    account_id INTEGER REFERENCES account(account_id),
    strike_category_id INTEGER REFERENCES categories(category_id),
    PRIMARY KEY (account_id, strike_category_id)
);