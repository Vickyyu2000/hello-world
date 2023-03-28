/* Create the schema for our tables */
create table Person(name VARCHAR(30), age int, gender VARCHAR(10));
create table Frequents(name VARCHAR(30), pizzeria VARCHAR(30));
create table Eats(name VARCHAR(30), pizza VARCHAR(30));
create table Serves(pizzeria VARCHAR(30), pizza VARCHAR(30), price float);

/* Populate the tables with our data */
insert into Person values('Amy', 16, 'female');
insert into Person values('Ben', 21, 'male');
insert into Person values('Cal', 33, 'male');
insert into Person values('Dan', 13, 'male');
insert into Person values('Eli', 45, 'male');
insert into Person values('Fay', 21, 'female');
insert into Person values('Gus', 24, 'male');
insert into Person values('Hil', 30, 'female');
insert into Person values('Ian', 18, 'male');

insert into Frequents values('Amy', 'Pizza Hut');
insert into Frequents values('Ben', 'Pizza Hut');
insert into Frequents values('Ben', 'Chicago Pizza');
insert into Frequents values('Cal', 'Straw Hat');
insert into Frequents values('Cal', 'New York Pizza');
insert into Frequents values('Dan', 'Straw Hat');
insert into Frequents values('Dan', 'New York Pizza');
insert into Frequents values('Eli', 'Straw Hat');
insert into Frequents values('Eli', 'Chicago Pizza');
insert into Frequents values('Fay', 'Dominos');
insert into Frequents values('Fay', 'Little Caesars');
insert into Frequents values('Gus', 'Chicago Pizza');
insert into Frequents values('Gus', 'Pizza Hut');
insert into Frequents values('Hil', 'Dominos');
insert into Frequents values('Hil', 'Straw Hat');
insert into Frequents values('Hil', 'Pizza Hut');
insert into Frequents values('Ian', 'New York Pizza');
insert into Frequents values('Ian', 'Straw Hat');
insert into Frequents values('Ian', 'Dominos');

insert into Eats values('Amy', 'pepperoni');
insert into Eats values('Amy', 'mushroom');
insert into Eats values('Ben', 'pepperoni');
insert into Eats values('Ben', 'cheese');
insert into Eats values('Cal', 'supreme');
insert into Eats values('Dan', 'pepperoni');
insert into Eats values('Dan', 'cheese');
insert into Eats values('Dan', 'sausage');
insert into Eats values('Dan', 'supreme');
insert into Eats values('Dan', 'mushroom');
insert into Eats values('Eli', 'supreme');
insert into Eats values('Eli', 'cheese');
insert into Eats values('Fay', 'mushroom');
insert into Eats values('Gus', 'mushroom');
insert into Eats values('Gus', 'supreme');
insert into Eats values('Gus', 'cheese');
insert into Eats values('Hil', 'supreme');
insert into Eats values('Hil', 'cheese');
insert into Eats values('Ian', 'supreme');
insert into Eats values('Ian', 'pepperoni');

insert into Serves values('Pizza Hut', 'pepperoni', 12);
insert into Serves values('Pizza Hut', 'sausage', 12);
insert into Serves values('Pizza Hut', 'cheese', 9);
insert into Serves values('Pizza Hut', 'supreme', 12);
insert into Serves values('Little Caesars', 'pepperoni', 9.75);
insert into Serves values('Little Caesars', 'sausage', 9.5);
insert into Serves values('Little Caesars', 'cheese', 7);
insert into Serves values('Little Caesars', 'mushroom', 9.25);
insert into Serves values('Dominos', 'cheese', 9.75);
insert into Serves values('Dominos', 'mushroom', 11);
insert into Serves values('Straw Hat', 'pepperoni', 8);
insert into Serves values('Straw Hat', 'cheese', 9.25);
insert into Serves values('Straw Hat', 'sausage', 9.75);
insert into Serves values('New York Pizza', 'pepperoni', 8);
insert into Serves values('New York Pizza', 'cheese', 7);
insert into Serves values('New York Pizza', 'supreme', 8.5);
insert into Serves values('Chicago Pizza', 'cheese', 7.75);
insert into Serves values('Chicago Pizza', 'supreme', 8.5);

/*Q1*/
SELECT name
FROM Person
WHERE age<18;

/* π name (σ age < 18 (Person)) */

/*Q2*/
SELECT serves.pizzeria, serves.pizza, serves.price
from serves natural join eats
where eats.name = 'Amy' AND serves.price < 10;

/* Q3 */
SELECT frequents.pizzeria, frequents.name, person.age
from frequents natural join person
where person.age<18;

/* Q4 */
select distinct f1.pizzeria
from person p1 natural join frequents f1, person p2 natural join frequents f2
where f1.pizzeria = f2.pizzeria and p1.age < 18 and p2.age > 30;

/* Q5 */
SELECT F1.pizzeria, P1.name, P1.age, P2.name, P2.age
FROM Frequents F1
JOIN Frequents F2 ON F1.pizzeria = F2.pizzeria AND F1.name <> F2.name
JOIN Person P1 ON F1.name = P1.name
JOIN Person P2 ON F2.name = P2.name
WHERE P1.age < 18 AND P2.age > 30;

select distinct f1.pizzeria, p1.name, p1.age, p2.name, p2.age
from person p1 natural join frequents f1, person p2 natural join frequents f2
where f1.pizzeria = f2.pizzeria and p1.age < 18 and p2.age > 30;


/* Q6 */
SELECT Eats.name, COUNT(DISTINCT Eats.pizza) AS num_pizzas
FROM Eats
WHERE Eats.name IN (SELECT Eats.name FROM Eats GROUP BY Eats.name HAVING COUNT(DISTINCT Eats.pizza) >= 2)
GROUP BY Eats.name
ORDER BY num_pizzas DESC;

select eats.name, count(eats.pizza) as count
from eats
group by eats.name
having count(eats.pizza) >= 2
order by count(eats.pizza) desc;


/* Q7 */

select serves.pizza, avg(serves.price)
from serves
group by serves.pizza
order by avg(price) desc;

select person.name, person.age,eats.pizza
from person left join eats using(name)
where age<18;