-- Siyi Liu V00951684
-- Churong Yu V00922542

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
SELECT *
FROM Person
WHERE age<18;


/*Q2*/
--version 1
SELECT  Serves.pizzeria, Serves.pizza, Serves.price
FROM Eats NATURAL JOIN Serves
WHERE Eats.name = 'Amy' AND Serves.price < 10;

--version 2
SELECT  pizzeria, pizza, price
FROM Serves
where pizza in (
    select pizza
    from Eats
    where name = 'Amy'
          ) and Serves.price < 10;

/*Q3*/
SELECT  Frequents.pizzeria, Person.name, Person.age
FROM Person NATURAL JOIN Frequents
WHERE Person.age<18;


/*Q4*/
SELECT  Frequents.pizzeria
FROM Person NATURAL JOIN Frequents
WHERE Person.age<18 AND Frequents.pizzeria IN (
    SELECT  Frequents.pizzeria
    FROM Person NATURAL JOIN Frequents
    WHERE  Person.age>30
    );

/*Q5*/
--version1:
Create view Group1 as
    select Frequents.pizzeria, Frequents.name, Person.age
    from Person NATURAL JOIN Frequents
    WHERE Person.age<18;

Create view Group2 as
    select Frequents.pizzeria, Frequents.name, Person.age
    from Person NATURAL JOIN Frequents
    WHERE Person.age>30;
SELECT pizzeria, Group1.name, Group1.age, Group2.name, Group2.age
FROM Group1 join Group2 using (pizzeria);
drop view Group1;
drop view Group2;

--version2:
select pizzeria, Group1.name, Group1.age, Group2.name, Group2.age
FROM (select Frequents.pizzeria, Frequents.name, Person.age
    from Person NATURAL JOIN Frequents
    WHERE Person.age<18) group1 join (
    select Frequents.pizzeria, Frequents.name, Person.age
    from Person NATURAL JOIN Frequents
    WHERE Person.age>30 ) group2 using (pizzeria);

/*Q6*/
select Eats.name, count(Eats.pizza)
from Eats
group by Eats.name
having count(Eats.pizza)>= 2
order by count(Eats.pizza) desc;


/*Q7*/
select pizza, avg(price)
from Serves
group by  pizza
order by avg(price) desc;



DROP table Person;
drop table eats;
drop table Frequents;
drop table serves;