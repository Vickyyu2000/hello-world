-- Create the tables for the S&P 500 database
-- by importing the data from the csv files provided. 
-- They contain information about the companies 
-- in the S&P 500 stock market index
-- during some interval of time in 2014-2015.
-- https://en.wikipedia.org/wiki/S%26P_500 

-- Familiarize yourself with the tables.
select * from history;
select * from sp500;
select distinct "State" from sp500;
select symbol from history;

-- Change the type of the day column to DATE. 
ALTER TABLE history ALTER COLUMN day TYPE DATE
USING to_date(day, 'YYYY-MM-DD');


-- Exercise 1 (3 pts)

-- 1. (1 pts) Find the number of companies for each state, sort descending by the number.

-- Write your query here.
SELECT "State", COUNT(*) AS num_companies
FROM sp500
GROUP BY "State"
ORDER BY num_companies DESC;


-- 2. (1 pts) Find the number of companies for each sector, sort descending by the number.

-- Write your query here. 
SELECT "Sector", COUNT(*) AS num_companies
FROM sp500
GROUP BY "Sector"
ORDER BY num_companies DESC;

-- 3. (1 pts) Order the days of the week by their average volatility.
-- Sort descending by the average volatility. 
-- Use 100*abs(high-low)/low to measure daily volatility.

-- Write your query here.

SELECT
    CASE
        WHEN EXTRACT(DOW FROM day) = 0 THEN 'Sunday'
        WHEN EXTRACT(DOW FROM day) = 1 THEN 'Monday'
        WHEN EXTRACT(DOW FROM day) = 2 THEN 'Tuesday'
        WHEN EXTRACT(DOW FROM day) = 3 THEN 'Wednesday'
        WHEN EXTRACT(DOW FROM day) = 4 THEN 'Thursday'
        WHEN EXTRACT(DOW FROM day) = 5 THEN 'Friday'
        ELSE 'Saturday'
    END AS weekday,
    AVG(100 * ABS(high - low) / low) AS avg_volatility
FROM
    history
GROUP BY
    EXTRACT(DOW FROM day)
ORDER BY
    avg_volatility DESC;

-- Exercise 2 (4 pts)

-- 1. (2 pts) Find for each symbol and day the pct change from the previous business day.
-- Order descending by pct change. Use adjclose.


-- Write your query here. 

SELECT *
FROM (
    SELECT
        symbol,
        day,
        100 * (adjclose - LAG(adjclose, 1) OVER (PARTITION BY symbol ORDER BY day)) / LAG(adjclose, 1) OVER (PARTITION BY symbol ORDER BY day) AS pct_change
    FROM
        history
) subq
WHERE
    pct_change IS NOT NULL
ORDER BY
    pct_change DESC;

-- 2. (2 pts)
-- Many traders believe in buying stocks in uptrend
-- in order to maximize their chance of profit. 
-- Let us check this strategy.
-- Find for each symbol on Oct 1, 2015 
-- the pct change 20 trading days earlier and 20 trading days later.
-- Order descending by pct change with respect to 20 trading days earlier.
-- Use adjclose.

-- Expected result
--symbol,pct_change,pct_change2
--TE,26.0661102331371252,3.0406725557250169
--TAP,24.6107784431137725,5.1057184046131667
--CVC,24.4688922610015175,-0.67052727826882048156
--...

-- Write your query here.

select symbol,
       100*(adjclose-adjclose_prev)/adjclose_prev AS pct_change,
       100*(adjclose_post-adjclose)/adjclose AS pct_change2
from
    (select symbol,
            adjclose,
            day,
            LAG(adjclose,20) OVER (PARTITION BY symbol ORDER BY day) AS adjclose_prev,
            LEAD(adjclose,20) OVER (PARTITION BY symbol ORDER BY day) AS adjclose_post
     from history) h1
where day = '2015-10-01'
order by pct_change desc;



-- Exercise 3 (3 pts)
-- Find the top 10 symbols with respect to their average money volume AVG(volume*adjclose).
-- Use round(..., -8) on the average money volume.
-- Give three versions of your query, using ROW_NUMBER(), RANK(), and DENSE_RANK().

-- Write your query here.
/*
 row_number() version
 */
SELECT symbol, avg_money_volume, rank
FROM
    (select symbol,
            ROUND(AVG(volume*adjclose), -8) AS avg_money_volume,
            ROW_NUMBER() OVER (ORDER BY AVG(volume*adjclose) DESC) AS rank
     from history
     group by symbol) h1
WHERE rank <= 10
ORDER BY rank;

/*
 rank() version
 */
WITH AvgMoneyVolume AS (SELECT symbol, round(AVG(volume * adjclose), -8) AS moneyvolume
                        FROM history
                        GROUP BY symbol)
SELECT *
FROM (
    SELECT symbol, moneyvolume AS mvolume,
           RANK() OVER (ORDER BY moneyvolume DESC) AS rank
    FROM AvgMoneyVolume) X
WHERE rank<=10
ORDER BY rank;

/*
 dense_rank() version
 */
WITH AvgMoneyVolume AS (SELECT symbol, round(AVG(volume * adjclose), -8) AS AvgMoneyVolume
                        FROM history
                        GROUP BY symbol)
SELECT *
FROM (
    SELECT symbol, AvgMoneyVolume AS AvgMonVol,
           DENSE_RANK() OVER (ORDER BY AvgMoneyVolume DESC) AS rank
    FROM AvgMoneyVolume) X
WHERE rank<=10
ORDER BY rank;
