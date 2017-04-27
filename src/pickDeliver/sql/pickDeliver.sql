/*PGR-GNU*****************************************************************
File: pickDeliver.sql

Generated with Template by:
Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Function's developer: 
Copyright (c) 2017 Celia Virginia Vergara Castillo
Mail: 

------

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

********************************************************************PGR-GNU*/

-- for the sake of Reginas book I am keeping this signature

CREATE OR REPLACE FUNCTION _pgr_pickDeliver(
    customers_sql TEXT,
    max_vehicles INTEGER,
    capacity FLOAT,
    speed FLOAT DEFAULT 1, 
    max_cycles INTEGER DEFAULT 10, 

    OUT seq INTEGER,
    OUT vehicle_id INTEGER,
    OUT vehicle_seq INTEGER,
    OUT stop_id BIGINT,
    OUT travel_time FLOAT,
    OUT arrival_time FLOAT,
    OUT wait_time FLOAT,
    OUT service_time FLOAT,
    OUT departure_time FLOAT
)
RETURNS SETOF RECORD AS
$BODY$
DECLARE
    first_sql TEXT;
    second_sql TEXT;
    final_sql TEXT;
BEGIN
    first_sql = $$WITH
        customer_data AS ($$ || customers_sql || $$ ),
        pickups AS (
            SELECT id, demand, x as pick_x, y as pick_y, opentime as pick_open, closetime as pick_close, servicetime as pick_service
            FROM  customer_data WHERE pindex = 0 AND id != 0
        ),
        deliveries AS (
            SELECT pindex AS id, x as deliver_x, y as deliver_y, opentime as deliver_open, closetime as deliver_close, servicetime as deliver_service
            FROM  customer_data WHERE dindex = 0 AND id != 0
        )
        SELECT * FROM pickups JOIN deliveries USING(id) ORDER BY pickups.id
    $$;

    second_sql = $$WITH
        customer_data AS ($$ || customers_sql || $$ )
        SELECT id, x AS start_x, y AS start_y,
            opentime AS start_open, closetime AS start_close, $$ ||
            capacity || $$ AS capacity, $$ || max_vehicles || $$ AS number 
            FROM customer_data WHERE id = 0 LIMIT 1
        $$;

    final_sql = $$ WITH
        customer_data AS ($$ || customers_sql || $$ ),
        pick_deliver AS (SELECT * FROM _pgr_pickDeliver('$$ || first_sql || $$',  '$$ || second_sql || $$',  $$ || max_cycles || $$)),
        picks AS (SELECT pick_deliver.*, pindex, dindex, id AS the_id FROM pick_deliver JOIN customer_data ON (id = order_id AND stop_type = 1)),
        delivers AS (SELECT pick_deliver.*, pindex, dindex, dindex AS the_id FROM pick_deliver JOIN customer_data ON (id = order_id AND stop_type = 2)),
        depots AS (SELECT pick_deliver.*, pindex, dindex, dindex AS the_id FROM pick_deliver JOIN customer_data ON (id = order_id AND order_id = 0)),
        the_union AS (SELECT * FROM picks UNION SELECT * FROM delivers UNION SELECT * from depots)

        SELECT a.seq, vehicle_number, a.vehicle_seq, the_id::BIGINT, a.travel_time, a.arrival_time, a.wait_time, a.service_time, a.departure_time
        FROM (SELECT * FROM the_union) AS a ORDER BY a.seq
        $$;
    RETURN QUERY EXECUTE final_sql;
END;
$BODY$
LANGUAGE plpgsql VOLATILE STRICT;


-- LATEST signature
CREATE OR REPLACE FUNCTION _pgr_pickDeliver(
    orders_sql TEXT,
    vehicles_sql TEXT,
    max_cycles INTEGER DEFAULT 10, 

    OUT seq INTEGER,
    OUT vehicle_number INTEGER,
    OUT vehicle_id BIGINT,
    OUT vehicle_seq INTEGER,
    OUT order_id BIGINT,
    OUT stop_type INT,
    OUT cargo FLOAT,
    OUT travel_time FLOAT,
    OUT arrival_time FLOAT,
    OUT wait_time FLOAT,
    OUT service_time FLOAT,
    OUT departure_time FLOAT
)

RETURNS SETOF RECORD AS
'$libdir/${PGROUTING_LIBRARY_NAME}', 'pickDeliver'
LANGUAGE c VOLATILE STRICT;
