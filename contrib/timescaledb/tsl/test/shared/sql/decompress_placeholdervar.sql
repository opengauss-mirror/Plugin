-- This file and its contents are licensed under the Timescale License.
-- Please see the included NOTICE for copyright information and
-- LICENSE-TIMESCALE for a copy of the license.

-- test handling of PlaceHolderVar in DecompressChunk
-- https://github.com/timescale/timescaledb/issues/1981

CREATE TABLE decompress_phv_device (
    device_id integer NOT NULL,
    decompress_phv_device_name varchar(128) NOT NULL,
    network_id integer
);

CREATE TABLE decompress_phv_ping (
    id integer NOT NULL,
    device_id integer NOT NULL,
    pkt_rx integer NOT NULL,
    rtt_avg float,
    insert_ts timestamp DEFAULT CURRENT_TIMESTAMP NOT NULL
);

SELECT create_hypertable ('decompress_phv_ping', 'insert_ts', chunk_time_interval => interval '1 day');

INSERT INTO decompress_phv_device
    VALUES (41040, 'MB10010013', 240), (65101, 'BR100103ca', 240), (65103, 'BR100103cb', 240), (65105, 'BR100103cc', 240), (65107, 'BR100103cd', 240), (41041, 'MB11000000', 241), (65201, 'BR11000014', 241), (65203, 'BR11000015', 241), (65205, 'BR11000016', 241), (65207, 'BR11000017', 241);

INSERT INTO decompress_phv_ping
    VALUES (1, 41040, 3, 243.333333333333, '2019-03-21 02:30:48.007'), (1, 41040, 3, 300.333333333333, '2019-03-21 02:00:19.957'), (1, 41040, 3, 265.333333333333, '2019-03-21 01:28:50.669'), (1, 41040, 3, 174, '2019-03-21 00:57:09.733'), (1, 41040, 3, 552.666666666667, '2019-03-21 00:21:02.305'), (1, 41041, 3, 330.333333333333, '2019-03-21 02:30:44.984'), (1, 41041, 3, 753, '2019-03-21 02:00:38.062'), (1, 41041, 3, 260, '2019-03-21 01:30:10.21'), (1, 41041, 3, 357.333333333333, '2019-03-21 00:59:10.768'), (1, 41041, 3, 474.666666666667, '2019-03-21 00:20:54.272'), (1, 65101, 3, 17.5200004577637, '2019-03-21 02:36:50.606'), (1, 65101, 3, 17.5200004577637, '2019-03-21 02:06:43.631'), (1, 65101, 3, 31.8933359781901, '2019-03-21 01:34:56.56'), (1, 65101, 3, 17.5200004577637, '2019-03-21 01:03:10.913'), (1, 65101, 3, 17.5200004577637, '2019-03-21 00:33:53.707'), (1, 65201, 3, 17.5200004577637, '2019-03-21 02:31:58.444'), (1, 65201, 3, 17.5200004577637, '2019-03-21 02:01:53.565'), (1, 65201, 3, 17.5200004577637, '2019-03-21 01:30:16.261'), (1, 65201, 2, 17.5200004577637, '2019-03-21 00:59:34.953'), (1, 65201, 3, 17.5200004577637, '2019-03-21 00:27:19.228'), (1, 41040, 3, 31.6666666666667, '2019-03-22 03:29:45.404'), (1, 41040, 3, 128, '2019-03-22 03:01:36.323'), (1, 41040, 3, 50, '2019-03-22 02:31:20.107'), (1, 41040, 3, 149.666666666667, '2019-03-22 02:01:07.201'), (1, 41040, 3, 502, '2019-03-22 01:28:27.129'), (1, 41041, 3, 355, '2019-03-22 03:30:50.844'), (1, 41041, 3, 53, '2019-03-22 03:01:40.348'), (1, 41041, 3, 121.333333333333, '2019-03-22 02:32:04.392'), (1, 41041, 3, 325.333333333333, '2019-03-22 02:02:06.968'), (1, 41041, 3, 576.666666666667, '2019-03-22 01:30:04.821'), (1, 65101, 3, 17.5200004577637, '2019-03-22 03:08:33.227'), (1, 65101, 3, 17.5200004577637, '2019-03-22 02:37:29.96'), (1, 65101, 3, 17.5200004577637, '2019-03-22 02:07:14.805'), (1, 65101, 3, 17.5200004577637, '2019-03-22 01:34:25.055'), (1, 65101, 3, 17.5200004577637, '2019-03-22 01:03:40.405'), (1, 65201, 3, 17.5200004577637, '2019-03-22 03:30:56.885'), (1, 65201, 3, 17.5200004577637, '2019-03-22 03:03:40.243'), (1, 65201, 3, 17.5200004577637, '2019-03-22 02:32:38.625'), (1, 65201, 3, 17.5200004577637, '2019-03-22 02:02:20.05'), (1, 65201, 3, 17.5200004577637, '2019-03-22 01:30:09.863');

ALTER TABLE decompress_phv_ping SET (timescaledb.compress, timescaledb.compress_orderby = 'device_id, insert_ts DESC');

SELECT count(compress_chunk(chunk_name)) AS compressed
FROM timescaledb_information.compressed_chunk_stats temprow
WHERE hypertable_name = 'decompress_phv_ping'::regclass
    AND compression_status = 'Uncompressed';

SELECT from_ts, insert_ts, network_id, pkt_loss, rtt_avg
FROM generate_series('2019-03-21 01:00:00'::timestamp, '2019-03-23 13:00:00'::timestamp, '12h'::interval) ts_bucket (from_ts)
    LEFT JOIN (
        SELECT d.network_id,
            CASE WHEN p.pkt_rx = 0 THEN
                1
            ELSE
                0
            END AS pkt_loss,
            p.rtt_avg,
            p.insert_ts
        FROM decompress_phv_device d
            INNER JOIN decompress_phv_ping p ON p.device_id = d.device_id
        WHERE d.network_id = 240) data ON data.insert_ts BETWEEN from_ts
    AND from_ts + '12h'::interval
ORDER BY 1,2;

DROP TABLE decompress_phv_ping;
DROP TABLE decompress_phv_device;

