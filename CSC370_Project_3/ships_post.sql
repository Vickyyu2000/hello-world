/*
This assignment introduces an example concerning World War II capital ships.
It involves the following relations:

Classes(class, type, country, numGuns, bore, displacement)
Ships(name, class, launched)  --launched is for year launched
Battles(name, date_fought)
Outcomes(ship, battle, result)

Ships are built in "classes" from the same design, and the class is usually
named for the first ship of that class.

Relation Classes records the name of the class,
the type (bb for battleship or bc for battlecruiser),
the country that built the ship, the number of main guns,
the bore (diameter of the gun barrel, in inches)
of the main guns, and the displacement (weight, in tons).

Relation Ships records the name of the ship, the name of its class,
and the year in which the ship was launched.

Relation Battles gives the name and date of battles involving these ships.

Relation Outcomes gives the result (sunk, damaged, or ok)
for each ship in each battle.
*/


/*
Exercise 1. (1 point)
1.	Create simple SQL statements to create the above relations
    (no constraints for initial creations).
2.	Insert the following data.

For Classes:
('Bismarck','bb','Germany',8,15,42000);
('Kongo','bc','Japan',8,14,32000);
('North Carolina','bb','USA',9,16,37000);
('Renown','bc','Gt. Britain',6,15,32000);
('Revenge','bb','Gt. Britain',8,15,29000);
('Tennessee','bb','USA',12,14,32000);
('Yamato','bb','Japan',9,18,65000);

For Ships
('California','Tennessee',1921);
('Haruna','Kongo',1915);
('Hiei','Kongo',1914);
('Iowa','Iowa',1943);
('Kirishima','Kongo',1914);
('Kongo','Kongo',1913);
('Missouri','Iowa',1944);
('Musashi','Yamato',1942);
('New Jersey','Iowa',1943);
('North Carolina','North Carolina',1941);
('Ramilles','Revenge',1917);
('Renown','Renown',1916);
('Repulse','Renown',1916);
('Resolution','Revenge',1916);
('Revenge','Revenge',1916);
('Royal Oak','Revenge',1916);
('Royal Sovereign','Revenge',1916);
('Tennessee','Tennessee',1920);
('Washington','North Carolina',1941);
('Wisconsin','Iowa',1944);
('Yamato','Yamato',1941);

For Battles
('North Atlantic','27-May-1941');
('Guadalcanal','15-Nov-1942');
('North Cape','26-Dec-1943');
('Surigao Strait','25-Oct-1944');

For Outcomes
('Bismarck','North Atlantic', 'sunk');
('California','Surigao Strait', 'ok');
('Duke of York','North Cape', 'ok');
('Fuso','Surigao Strait', 'sunk');
('Hood','North Atlantic', 'sunk');
('King George V','North Atlantic', 'ok');
('Kirishima','Guadalcanal', 'sunk');
('Prince of Wales','North Atlantic', 'damaged');
('Rodney','North Atlantic', 'ok');
('Scharnhorst','North Cape', 'sunk');
('South Dakota','Guadalcanal', 'ok');
('West Virginia','Surigao Strait', 'ok');
('Yamashiro','Surigao Strait', 'sunk');
*/

-- Write your sql statements here.
CREATE TABLE Classes (
class varchar(50),
type varchar(2),
country varchar(50),
numGuns int,
bore int,
displacement int
);

CREATE TABLE Ships (
name varchar(50),
class varchar(50),
launched int
);

CREATE TABLE Battles (
name varchar(50),
date_fought date
);

CREATE TABLE Outcomes (
ship varchar(50),
battle varchar(50),
result varchar(10)
);

INSERT INTO Classes (class, type, country, numGuns, bore, displacement)
VALUES ('Bismarck','bb','Germany',8,15,42000),
('Kongo','bc','Japan',8,14,32000),
('North Carolina','bb','USA',9,16,37000),
('Renown','bc','Gt. Britain',6,15,32000),
('Revenge','bb','Gt. Britain',8,15,29000),
('Tennessee','bb','USA',12,14,32000),
('Yamato','bb','Japan',9,18,65000);

INSERT INTO Ships (name, class, launched)
VALUES ('California','Tennessee',1921),
('Haruna','Kongo',1915),
('Hiei','Kongo',1914),
('Iowa','Iowa',1943),
('Kirishima','Kongo',1914),
('Kongo','Kongo',1913),
('Missouri','Iowa',1944),
('Musashi','Yamato',1942),
('New Jersey','Iowa',1943),
('North Carolina','North Carolina',1941),
('Ramilles','Revenge',1917),
('Renown','Renown',1916),
('Repulse','Renown',1916),
('Resolution','Revenge',1916),
('Revenge','Revenge',1916),
('Royal Oak','Revenge',1916),
('Royal Sovereign','Revenge',1916),
('Tennessee','Tennessee',1920),
('Washington','North Carolina',1941),
('Wisconsin','Iowa',1944),
('Yamato','Yamato',1941);

INSERT INTO Battles (name, date_fought)
VALUES ('North Atlantic','1941-05-27'),
('Guadalcanal','1942-11-15'),
('North Cape','1943-12-26'),
('Surigao Strait','1944-10-25');

INSERT INTO Outcomes (ship, battle, result)
VALUES ('Bismarck','North Atlantic', 'sunk'),
('California','Surigao Strait', 'ok'),
('Duke of York','North Cape', 'ok'),
('Fuso','Surigao Strait', 'sunk'),
('Hood','North Atlantic', 'sunk'),
('King George V','North Atlantic', 'ok'),
('Kirishima','Guadalcanal', 'sunk'),
('Prince of Wales','North Atlantic', 'damaged'),
('Rodney','North Atlantic', 'ok'),
('Scharnhorst','North Cape', 'sunk'),
('South Dakota','Guadalcanal', 'ok'),
('West Virginia','Surigao Strait', 'ok'),
('Yamashiro','Surigao Strait', 'sunk');


-- Exercise 2. (6 points)
-- Write SQL queries for the following requirements.

-- 1.	(2 pts) List the name, displacement, and number of guns of the ships engaged in the battle of Guadalcanal.

/*
Expected result:
ship,displacement,numguns
Kirishima,32000,8
South Dakota,NULL,NULL
*/

-- Write your query here.
select outcomes.ship, classes.displacement, classes.numguns
from outcomes left join ships on outcomes.ship = ships.name
    left join classes on ships.class = classes.class
where battle = 'Guadalcanal';

-- 2.	(2 pts) Find the names of the ships whose number of guns was the largest for those ships of the same bore.

-- Write your query here.
SELECT s.name, c.bore, c.numGuns
FROM Ships s
JOIN Classes c ON s.class = c.class
WHERE c.numGuns = (
  SELECT MAX(numGuns)
  FROM Classes
  WHERE bore = c.bore
)
ORDER BY c.bore DESC;

--3. (2 pts) Find for each class with at least three ships the number of ships of that class sunk in battle.
/*
class,sunk_ships
Revenge,0
Kongo,1
Iowa,0
*/

-- Write your query here.
select c2.class, COALESCE(COUNT(o.result), 0) AS ships_sunk
from(select s.class
    from ships s left join classes c on s.class = c.class
    group by s.class
    having count(s.name) >2) c2
    left join ships s2 on c2.class = s2.class
    left join outcomes o on s2.name = o.ship
group by c2.class;


-- Exercise 3. (4 points)

-- Write the following modifications.

-- 1.	(2 points) Two of the three battleships of the Italian Vittorio Veneto class –
-- Vittorio Veneto and Italia – were launched in 1940;
-- the third ship of that class, Roma, was launched in 1942.
-- Each had 15-inch guns and a displacement of 41,000 tons.
-- Insert these facts into the database.

-- Write your sql statements here.
INSERT INTO Classes (class, type, country, bore, displacement)
VALUES ('Vittorio Veneto', 'bb', 'Italy', 15, 41000);

INSERT INTO Ships (name, class, launched)
VALUES ('Vittorio Veneto','Vittorio Veneto',1940),
       ('Italia','Vittorio Veneto',1940),
       ('Roma','Vittorio Veneto',1942);

-- 2.	(1 point) Delete all classes with fewer than three ships.

-- Write your sql statement here.
delete
from classes
where class not in (select c.class
from classes c inner join ships s on c.class = s.class
group by c.class
having count(*) >= 3);

-- 3.	(1 point) Modify the Classes relation so that gun bores are measured in centimeters
-- (one inch = 2.5 cm) and displacements are measured in metric tons (one metric ton = 1.1 ton).

-- Write your sql statement here.
UPDATE classes SET bore = bore * 2.5;

ALTER TABLE Classes
ALTER COLUMN displacement SET DATA TYPE NUMERIC(10, 2);
UPDATE Classes SET displacement = displacement / 1.1;


-- Exercise 4.  (9 points)
-- Add the following constraints using views with check option.

--1. (3 points) No ship can be in battle before it is launched.

-- Write your sql statement here.
CREATE OR REPLACE VIEW OutcomesView AS
SELECT ship, battle, result
FROM Outcomes O
WHERE NOT EXISTS (
SELECT *
FROM Ships S, Battles B
WHERE S.name=O.ship AND O.battle=B.name AND
S.launched > extract(year from B.date_fought)
)
WITH CHECK OPTION;
-- Now we can try some insertion on this view.
INSERT INTO OutcomesView (ship, battle, result)
VALUES('Musashi', 'North Atlantic','ok');
-- This insertion, as expected, should fail since Musashi is launched in 1942,
-- while the North Atlantic battle took place on 27-MAY-41.


-- 2. (3 points) No ship can be launched before
-- the ship that bears the name of the first ship’s class.

-- Write your sql statement here.
CREATE OR REPLACE VIEW ShipsV AS
select name, class, launched
from ships
where not exists(
    select *
    from ships s1, ships s2
    where s1.class = s2.class AND s1.name != s2.name
        AND s1.name = s1.class AND s1.launched < s2.launched
    )
with check option;
-- Now we can try some insertion on this view.
INSERT INTO ShipsV(name, class, launched)
VALUES ('AAA','Kongo',1912);
-- This insertion, as expected, should fail since ship Kongo (first ship of class Kongo) is launched in 1913.


--3. (3 points) No ship fought in a battle that was at a later date than another battle in which that ship was sunk.

-- Write your sql statements here.
CREATE OR REPLACE VIEW OutcomesV AS
SELECT ship, battle, result
FROM Outcomes O
WHERE NOT EXISTS (
    SELECT *
    FROM Outcomes O2, Battles B2
    WHERE O2.ship = O.ship AND O2.battle = B2.name AND B2.date_fought > (
        SELECT MAX(B1.date_fought)
        FROM Battles B1, Outcomes O1
        WHERE O1.ship = O.ship AND O1.battle = B1.name AND O1.result = 'sunk'
    )
)
WITH CHECK OPTION;
-- Now we can try some insertion on this view.
INSERT INTO OutcomesV(ship, battle, result)
VALUES('Bismarck', 'Guadalcanal', 'ok');
-- This insertion, as expected, should fail since 'Bismarck' was sunk in
-- the battle of North Atlantic, in 1941, whereas the battle of Guadalcanal happened in 1942.