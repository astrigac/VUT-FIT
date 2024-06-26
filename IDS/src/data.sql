--+--------------------------------------------------------------------------------------------
--|  IDS (Database systems): Part Four
--|  Authors: Romana Ďuráčiová <xdurac01>, Aurel Strigáč <xstrig00>
--+--------------------------------------------------------------------------------------------

DROP TABLE contains;
DROP TABLE review;
DROP TABLE productOrder;
DROP TABLE product;
DROP TABLE category;
DROP TABLE employee;
DROP TABLE customer;
DROP TABLE visitor;

DROP MATERIALIZED VIEW customers_shipped_via_Packeta;

CREATE TABLE visitor 
( 
    visitorID       INT GENERATED BY DEFAULT AS IDENTITY,
    name            VARCHAR2(255) NOT NULL,
    lastname        VARCHAR2(255) NOT NULL,
    phoneNumber     CHAR(13) NOT NULL,
    email           VARCHAR2(255) NOT NULL,
    address         VARCHAR2(255) NOT NULL,

    CHECK (REGEXP_LIKE(phoneNumber, '^\+420\d{9}$')) 
);

CREATE TABLE employee 
( 
    visitorID           INT NOT NULL,
    employeeID          INT GENERATED BY DEFAULT AS IDENTITY,
    entryDate           DATE NOT NULL,
    contractExpiryDate  DATE,
    nationalIdNumber    INT NOT NULL,           --Birth number
    citizenCardNumber   VARCHAR2(8) NOT NULL,   --Citizen pass number
    insurance           VARCHAR2(255) NOT NULL,
    position            VARCHAR2(255) NOT NULL,
    salary              NUMBER(7) NOT NULL,
    accountNumber       VARCHAR2(18) NOT NULL,

    -- Birth numbers after 1954 (including) must be divisible by 11
    CHECK(MOD(nationalIdNumber, 11) = 0)
);

CREATE TABLE customer
(
    visitorID       INT NOT NULL,
    customerID      INT GENERATED BY DEFAULT AS IDENTITY,
    loginName       VARCHAR2(255) NOT NULL,
    password        VARCHAR2(255) NOT NULL
);

CREATE TABLE productOrder
(
    orderID         INT GENERATED BY DEFAULT AS IDENTITY,
    orderDate       DATE NOT NULL,
    priceSum        DECIMAL(9, 2) NOT NULL,
    shippingAddress VARCHAR2(255) NOT NULL,
    deliveryMethod  VARCHAR2(255) NOT NULL,
    paymentMethod   VARCHAR2(255) NOT NULL,
    productID       INT NOT NULL,
    employeeID      INT NOT NULL,
    customerID      INT NOT NULL
);

CREATE TABLE product
(
    productID       INT GENERATED BY DEFAULT AS IDENTITY,
    productName     VARCHAR2(255) NOT NULL,
    price           DECIMAL(8, 2) NOT NULL,
    stock           NUMBER(6) NOT NULL,
    description     VARCHAR2(255) NOT NULL,
    categoryID      INT NOT NULL
);

CREATE TABLE review
(
    reviewID    INT GENERATED BY DEFAULT AS IDENTITY,
    stars       INT NOT NULL,
    description VARCHAR(255) NOT NULL,
    orderID     INT NOT NULL,
    customerID  INT NOT NULL,

    CHECK(stars < 6)
);

CREATE TABLE category 
(
    categoryID      INT GENERATED BY DEFAULT AS IDENTITY, 
    categoryName    VARCHAR2(255) NOT NULL
);

CREATE TABLE contains
(
    quantity    NUMBER NOT NULL,
    productID   NUMBER NOT NULL,
    orderID     NUMBER NOT NULL
);

----------------------------------------- PRIMARY KEYS ----------------------------------------

ALTER TABLE visitor ADD CONSTRAINT PK_visitor PRIMARY KEY(visitorID);
ALTER TABLE employee ADD CONSTRAINT PK_employee PRIMARY KEY(employeeID);
ALTER TABLE customer ADD CONSTRAINT PK_customer PRIMARY KEY(customerID);
ALTER TABLE review ADD CONSTRAINT PK_review PRIMARY KEY(reviewID);
ALTER TABLE productOrder ADD CONSTRAINT PK_productOrder PRIMARY KEY(orderID);
ALTER TABLE product ADD CONSTRAINT PK_product PRIMARY KEY(productID);
ALTER TABLE category ADD CONSTRAINT PK_category PRIMARY KEY(categoryID);
ALTER TABLE contains ADD CONSTRAINT PK_contains PRIMARY KEY(productID, orderID);

---------------------------------------- FOREIGN KEYS -----------------------------------------

ALTER TABLE customer ADD CONSTRAINT FK_customer_visitor FOREIGN KEY(visitorID) REFERENCES visitor;

ALTER TABLE employee ADD CONSTRAINT FK_employee_visitor FOREIGN KEY(visitorID) REFERENCES visitor;

ALTER TABLE review ADD CONSTRAINT FK_review_customer FOREIGN KEY(customerID) REFERENCES customer;
ALTER TABLE review ADD CONSTRAINT FK_review_productOrder FOREIGN KEY(orderID) REFERENCES productOrder;

ALTER TABLE productOrder ADD CONSTRAINT FK_productOrder_customer FOREIGN KEY(customerID) REFERENCES customer;
ALTER TABLE productOrder ADD CONSTRAINT FK_productOrder_employee FOREIGN KEY(employeeID) REFERENCES employee;
ALTER TABLE productOrder ADD CONSTRAINT FK_productOrder_product FOREIGN KEY(productID) REFERENCES product;

ALTER TABLE product ADD CONSTRAINT FK_product_category FOREIGN KEY(categoryID) REFERENCES category;

ALTER TABLE contains ADD CONSTRAINT FK_contains_product FOREIGN KEY(productID) REFERENCES product;
ALTER TABLE contains ADD CONSTRAINT FK_contains_productOrder FOREIGN KEY(orderID) REFERENCES productOrder;

--------------------------------------- DATA INSERTION ----------------------------------------

INSERT INTO visitor (name, lastname, phoneNumber, email, address) VALUES ('Michal', 'Stastny', '+420915438000', 'misko@gmail.com', 'Armadna 6, 911 01 Trencin');
INSERT INTO visitor (name, lastname, phoneNumber, email, address) VALUES ('Kristina', 'Nosalova', '+420915438001', 'nosal.kris@seznam.cz', 'Halalovka 28, 911 08 Trencin');
INSERT INTO visitor (name, lastname, phoneNumber, email, address) VALUES ('Alex', 'Kutny', '+420739254380', 'alex.kutny@gmail.com', 'Stavbarska 1, 821 07 Bratislava');
INSERT INTO visitor (name, lastname, phoneNumber, email, address) VALUES ('Robo', 'Mikla', '+420123456789', 'robert.m@centrum.cz', 'Katovska 48B, 908 51 Holic');
INSERT INTO visitor (name, lastname, phoneNumber, email, address) VALUES ('Jan', 'Kolenik', '+420918929343', 'jan.kolenik@gmail.com', 'Obchodna 4, 900 28 Ivanka pri Dunaji');
INSERT INTO visitor (name, lastname, phoneNumber, email, address) VALUES ('Mikulas', 'Cernak', '+420234567890', 'miki@gmail.com', 'Mierove namestie 1, 019 01 Ilava');
INSERT INTO visitor (name, lastname, phoneNumber, email, address) VALUES ('Veronika', 'Kovacova', '+420987654321', 'm.kovac@seznam.cz', 'Namestie republiky 32, 984 01 Lucenec');
INSERT INTO visitor (name, lastname, phoneNumber, email, address) VALUES ('Imrich', 'Debnar', '+420741932055', 'imro@gmail.com', 'Clementisova 6, 911 01 Trencin');
INSERT INTO visitor (name, lastname, phoneNumber, email, address) VALUES ('Monika', 'Kovacova', '+420549932044', 'monik@centrum.sk', 'Kosicka 1, 911 01 Trencin');
INSERT INTO visitor (name, lastname, phoneNumber, email, address) VALUES ('Peter', 'Sykora', '+420915438002', 'sykorka.peter@gmail.com', 'Trencianska Turna 458, 911 01 Trencianska Turna');
INSERT INTO visitor (name, lastname, phoneNumber, email, address) VALUES ('Jozef', 'Kovac', '+420915438003', 'kov.joz@email.cz', 'Kotlarska 46, 602 00 Brno');
INSERT INTO visitor (name, lastname, phoneNumber, email, address) VALUES ('Juraj', 'Fuzik', '+420945738118', 'fuzoj@gmail.com', 'Palackeho trida 64, 612 00 Brno-Kralovo Pole');
INSERT INTO visitor (name, lastname, phoneNumber, email, address) VALUES ('Alexandra', 'Orechovska', '+420912438369', 'sasa.orechovska@seznam.cz', 'Vodova 77, 612 00 Brno-Kralovo Pole');


INSERT INTO customer (visitorID, loginName, password) VALUES (1, 'michal32', 'password');
INSERT INTO customer (visitorID, loginName, password) VALUES (2, 'xklani03', 'sadljsh12-4<');
INSERT INTO customer (visitorID, loginName, password) VALUES (3, 'monika33', '345-23jhkjh-22');
INSERT INTO customer (visitorID, loginName, password) VALUES (4, 'xsykor33', 'heslo? heslo : heslo');
INSERT INTO customer (visitorID, loginName, password) VALUES (5, 'sasaorech', 'heslokreslo');
INSERT INTO customer (visitorID, loginName, password) VALUES (6, 'imrodebnar', '02052008');
INSERT INTO customer (visitorID, loginName, password) VALUES (7, 'kovacova2', 'verike544');
INSERT INTO customer (visitorID, loginName, password) VALUES (8, 'XkovacX', 'jozo123');

INSERT INTO employee (visitorID, entryDate, contractExpiryDate, nationalIdNumber, citizenCardNumber, insurance, position, salary, accountNumber) VALUES
    (5, TO_DATE('2021-10-16', 'YYYY-MM-DD'), null, 6808051899, 'HW4567XY', 'Kooperativa', 'Skladnik', 29000, '304560232/0100');
INSERT INTO employee (visitorID, entryDate, contractExpiryDate, nationalIdNumber, citizenCardNumber, insurance, position, salary, accountNumber) VALUES
    (9, TO_DATE('2019-01-01', 'YYYY-MM-DD'), null, 6365636596, 'HH46Y85A', 'Union', 'Skladnik', 65308, '652235032/3800');
INSERT INTO employee (visitorID, entryDate, contractExpiryDate, nationalIdNumber, citizenCardNumber, insurance, position, salary, accountNumber) VALUES
    (6, TO_DATE('2020-05-06', 'YYYY-MM-DD'), TO_DATE('2025-05-06', 'YYYY-MM-DD'), 7707303989, 'KE3962IU', 'Uniqa', 'Orderpicker', 32000, '123450232/0800');
INSERT INTO employee (visitorID, entryDate, contractExpiryDate, nationalIdNumber, citizenCardNumber, insurance, position, salary, accountNumber) VALUES
    (7, TO_DATE('2022-09-06', 'YYYY-MM-DD'), null, 9854089927, 'DE2345PO', 'VZP', 'SBS', 30000, '300204323/0300');
INSERT INTO employee (visitorID, entryDate, contractExpiryDate, nationalIdNumber, citizenCardNumber, insurance, position, salary, accountNumber) VALUES
    (8, TO_DATE('2014-04-04', 'YYYY-MM-DD'), null, 9311765232, 'HH46Y85A', 'Union', 'Manazer', 87008, '304460232/0100');

INSERT INTO category (categoryName) VALUES ('Umyvanie');
INSERT INTO category (categoryName) VALUES ('Ochrana laku');
INSERT INTO category (categoryName) VALUES ('Cistenie okien');
INSERT INTO category (categoryName) VALUES ('Cistenie interieru');

INSERT INTO product (productName, price, stock, description, categoryID) VALUES
    ('Koch Chemie Autoshampoo', 200.29, 20, 'Mierne alkalicky, neposkodzuje vopred aplikovanu ochranu automobilu. 1l', 1);
INSERT INTO product (productName, price, stock, description, categoryID) VALUES
    ('Soft99 Kiwami Light', 480.70, 1, 'Tuhy hybridny vosk pre auta so svetlym lakom. 200g', 2);
INSERT INTO product (productName, price, stock, description, categoryID) VALUES
    ('Ewocar Hydro36', 1950.99, 5, 'Keramicky povlak s vydrzou az 36 mesiacov. 50ml', 2);
INSERT INTO product (productName, price, stock, description, categoryID) VALUES
    ('Ewocar Neutral Foam Shampoo', 249.99, 300, 'pH neutrálna aktívna pena / šampón. 1l', 1);
INSERT INTO product (productName, price, stock, description, categoryID) VALUES
    ('Aqua Car Cosmetics Glass Cleaner', 279.99, 50, 'Pripravok na cistenie okien, zrkadiel, chromovych prvkov a sklenenych povrchov reflektorov. 500ml', 3);
INSERT INTO product (productName, price, stock, description, categoryID) VALUES
    ('Aqua Car Cosmetics Interior Cleaner', 220.55, 44, 'Pripravok na cistenie interieru. 500ml', 4);
INSERT INTO product (productName, price, stock, description, categoryID) VALUES
    ('Aqua Car Cosmetics Leather Cleaner', 329.49, 35, 'Pripravok na cistenie kozenych sedadiel. 500ml', 4);
INSERT INTO product (productName, price, stock, description, categoryID) VALUES
    ('Aqua Car Cosmetics Leather Conditioner', 259.39, 23, 'Pripravok na impregnaciu kozenych sedadiel. 300ml', 4);
INSERT INTO product (productName, price, stock, description, categoryID) VALUES
    ('Aqua Car Cosmetics Dashboard Cleaner', 125.53, 70, 'Pripravok na cistenie plastovych a kovovych casti interieru. 500ml', 4);
INSERT INTO product (productName, price, stock, description, categoryID) VALUES
    ('Aqua Car Cosmetics Dashboard Protector', 179.99, 78, 'Pripravok na ochranu plastovych a kovovych casti interieru. 500ml', 4);
INSERT INTO product (productName, price, stock, description, categoryID) VALUES
    ('Aqua Car Cosmetics Dashboard Tissues', 162.49, 250, 'Vlhcene utierky na cistenie palubnej dosky. 30ks', 4);


INSERT INTO productOrder (orderDate, priceSum, shippingAddress, deliveryMethod, paymentMethod, productID, employeeID, customerID) VALUES 
    (TO_DATE('2021-12-12', 'YYYY-MM-DD'), 2500.29, 'Stavbarska 1, 821 07 Bratislava', 'Packeta', 'ApplePay', 4, 1, 3);
INSERT INTO productOrder (orderDate, priceSum, shippingAddress, deliveryMethod, paymentMethod, productID, employeeID, customerID) VALUES 
    (TO_DATE('2022-05-07', 'YYYY-MM-DD'), 250.00, 'Halalovka 28, 911 08 Trencin', 'Packeta', 'Kartou', 1, 3, 2);
INSERT INTO productOrder (orderDate, priceSum, shippingAddress, deliveryMethod, paymentMethod, productID, employeeID, customerID) VALUES 
    (TO_DATE('2022-05-06', 'YYYY-MM-DD'), 6000.00, 'Halalovka 28, 911 08 Trencin', 'DPD', 'Pri prevzati', 3, 1, 2);
INSERT INTO productOrder (orderDate, priceSum, shippingAddress, deliveryMethod, paymentMethod, productID, employeeID, customerID) VALUES 
    (TO_DATE('2023-02-24', 'YYYY-MM-DD'), 99.99, 'Kotlarska 46, 602 00 Brno', 'Packeta', 'Kartou', 8, 2, 6);
INSERT INTO productOrder (orderDate, priceSum, shippingAddress, deliveryMethod, paymentMethod, productID, employeeID, customerID) VALUES 
    (TO_DATE('2023-02-28', 'YYYY-MM-DD'), 37.47, 'Vodova 77, 612 00 Brno-Kralovo Pole', 'Packeta', 'ApplePay', 11, 2, 8);
INSERT INTO productOrder (orderDate, priceSum, shippingAddress, deliveryMethod, paymentMethod, productID, employeeID, customerID) VALUES
    (TO_DATE('2023-03-05', 'YYYY-MM-DD'), 54.99, 'Namestie republiky 32, 984 01 Lucenec', 'DPD', 'Kartou', 9, 2, 7);
INSERT INTO productOrder (orderDate, priceSum, shippingAddress, deliveryMethod, paymentMethod, productID, employeeID, customerID) VALUES
    (TO_DATE('2023-03-17', 'YYYY-MM-DD'), 225.31, 'Mierove namestie 1, 019 01 Ilava', 'Packeta', 'Pri prevzati', 9, 2, 6);

INSERT INTO contains (quantity, productID, orderID) VALUES (1, 4, 1);
INSERT INTO contains (quantity, productID, orderID) VALUES (2, 1, 2);
INSERT INTO contains (quantity, productID, orderID) VALUES (3, 3, 3);
INSERT INTO contains (quantity, productID, orderID) VALUES (1, 8, 4);
INSERT INTO contains (quantity, productID, orderID) VALUES (3, 11, 5);
INSERT INTO contains (quantity, productID, orderID) VALUES (1, 9, 6);
INSERT INTO contains (quantity, productID, orderID) VALUES (1, 5, 7);
INSERT INTO contains (quantity, productID, orderID) VALUES (3, 7, 7);
INSERT INTO contains (quantity, productID, orderID) VALUES (3, 8, 7);

INSERT INTO review (stars, description, orderID, customerID) VALUES (5, 'Rychle dodanie, super vyber. Odporucam!', 1, 3);
INSERT INTO review (stars, description, orderID, customerID) VALUES (3, 'Som spokojny.', 2, 2);
INSERT INTO review (stars, description, orderID, customerID) VALUES (5, 'Nemal som ziadny problem', 3, 2);
INSERT INTO review (stars, description, orderID, customerID) VALUES (5, 'Odporucam.', 4, 6);
-------------------------------------- SELECT STATEMENTS --------------------------------------

--1.) lists name, lastname, account number and salary of each employee
SELECT name, lastname, accountNumber, salary FROM employee NATURAL JOIN visitor;
--2.) lists logins of users that got their order shipped by Packeta
SELECT loginName FROM customer NATURAL JOIN productOrder p WHERE p.deliveryMethod = 'Packeta';
--3.) lists names and emails of users who rated eshop with less then 3 stars
SELECT name, email FROM visitor NATURAL JOIN customer NATURAL JOIN review r WHERE r.stars < 3;
--4.) lists amount of products shipped via each delivery method, sorted from highest to lowest
SELECT deliveryMethod, SUM(quantity) products_shipped FROM productOrder NATURAL JOIN contains GROUP BY deliveryMethod ORDER BY SUM(quantity) DESC; 
--5.) lists positions and total number of employees working that position
SELECT position, count(*) employee_count FROM employee GROUP BY position;
--6.) lists names and lastnames of employees that have not dispatched any orders yet
SELECT name, lastname FROM visitor NATURAL JOIN employee WHERE NOT EXISTS (SELECT * FROM productOrder WHERE productOrder.employeeID = employee.employeeID);
--7.) lists the name and email of customers whose password are shorter than 14 characters
SELECT name, email FROM visitor NATURAL JOIN customer WHERE customerID IN (SELECT customerID FROM customer WHERE LENGTH(password) < 14);

SET serveroutput on;

--------------------------------------- TRIGGERS ---------------------------------------
-- Database trigger that calculates the new average rating for the e-shop

CREATE OR REPLACE TRIGGER review_trigger
BEFORE INSERT ON review
FOR EACH ROW
DECLARE
    avg_rating DECIMAL(4,2);
    new_avg_rating DECIMAL(4,2);
    review_count INTEGER;
    cust_username VARCHAR2(255);
    order_date DATE;
BEGIN
    SELECT COUNT(*) INTO review_count FROM review;
    SELECT AVG(stars) INTO new_avg_rating FROM review;
    new_avg_rating := ((new_avg_rating * review_count + :NEW.stars)) / (review_count + 1);

    SELECT AVG(stars) INTO avg_rating
    FROM review
    WHERE reviewID <> :NEW.reviewID;

    SELECT c.loginName, o.orderDate INTO cust_username, order_date
    FROM customer c
    JOIN productOrder o ON o.orderID = :NEW.orderID
    WHERE c.customerID = :NEW.customerID;

    dbms_output.put_line('NEW RATING');
    dbms_output.put_line('Rating: ' || :NEW.stars || ' stars');
    dbms_output.put_line('Customer username: ' || cust_username);
    dbms_output.put_line('Order date: ' || order_date);
    dbms_output.put_line('Customer wrote: ' || :NEW.description);
    dbms_output.put_line('Average rating shift: ' || avg_rating || ' -> ' || new_avg_rating);
END;
/

INSERT INTO review (stars, description, orderID, customerID) VALUES (0, 'Uz druhy krat ma oklamali.', 5, 8);
INSERT INTO review (stars, description, orderID, customerID) VALUES (5, 'Nikdy som s tymto obchodom nemala problem.', 6, 7);

-----------------------------------------------------------------------------------------
-- Database trigger designed for checking the strength of passwords for customer records

CREATE OR REPLACE TRIGGER password_strength_trigger
BEFORE INSERT OR UPDATE ON customer
FOR EACH ROW
DECLARE
    password_strength VARCHAR2(10);
BEGIN
    IF LENGTH(:NEW.password) < 7 THEN
        password_strength := 'WEAK';
    ELSIF LENGTH(:NEW.password) < 12 THEN
        password_strength := 'MEDIUM';
    ELSE
        password_strength := 'STRONG';
    END IF;

    IF REGEXP_LIKE(:NEW.password, '[.\=/\%+-]') THEN
        IF password_strength = 'WEAK' THEN
            password_strength := 'MEDIUM';
        ELSE
            password_strength := 'STRONG';
        END IF;
    END IF;

    IF REGEXP_LIKE(:NEW.password, '[0-9]') THEN
        IF password_strength = 'WEAK' THEN
            password_strength := 'MEDIUM';
        ELSE
            password_strength := 'STRONG';
        END IF;
    END IF;

    dbms_output.put_line('PASSWORD CHANGE');
    dbms_output.put_line('User: ' || :NEW.loginName);
    IF :OLD.password IS NOT NULL THEN
        dbms_output.put_line('Old password: ' || :OLD.password);
    ELSE
        dbms_output.put_line('New user');
    END IF;
    dbms_output.put_line('New password: ' || :NEW.password);
    dbms_output.put_line('Password strength: ' || password_strength);
END;
/

INSERT INTO customer (visitorID, loginName, password) VALUES (13, 'saska22', 'password');
UPDATE customer
    SET password = 'Alt+F4'
WHERE visitorID = 13;



-------------------------------------- PROCEDURES --------------------------------------
-- Procedure that updates the stock of a product after a new order is made.

CREATE OR REPLACE PROCEDURE update_product_stock (
    product_id IN product.productID%type
) IS
    total_orders INT;
    product_stock INT;
    CURSOR cursor_orders IS SELECT quantity FROM contains WHERE productID = product_id;
BEGIN
    total_orders := 0;

    FOR order_row IN cursor_orders LOOP
        total_orders := total_orders + order_row.quantity;
    END LOOP;

    DBMS_OUTPUT.put_line('Total orders of the product: ' || total_orders);
    
    SELECT stock INTO product_stock FROM product WHERE productID = product_id;
    
    DBMS_OUTPUT.put_line('Stock before orders: ' || product_stock);
    product_stock := product_stock - total_orders;
    DBMS_OUTPUT.put_line('Stock after orders: ' || product_stock);
    
    IF product_stock < 0 THEN
        DBMS_OUTPUT.put_line('ERROR: Invalid stock of product.');
    ELSE
        UPDATE product SET stock = product_stock WHERE productID = product_id;
        COMMIT;
    END IF;

EXCEPTION
    WHEN NO_DATA_FOUND THEN
        dbms_output.put_line('ERROR: Invalid product ID.');
    WHEN OTHERS THEN
        ROLLBACK;
        raise;
END;
/

BEGIN
    DBMS_OUTPUT.put_line('PRODUCT 11: Aqua Car Cosmetics Dashboard Tissues');
    update_product_stock(11);
    DBMS_OUTPUT.put_line('PRODUCT 8: Aqua Car Cosmetics Leather Conditioner');
    update_product_stock(8);
END;
/

-----------------------------------------------------------------------------------------
-- Procedure that calculates the total price of all orders made by a specific customer

CREATE OR REPLACE PROCEDURE total_price (
    par_customerID IN customer.customerID%TYPE
) IS
    total_price NUMBER := 0;

BEGIN
    FOR order_record IN (SELECT * FROM productOrder WHERE customerID = par_customerID) LOOP
        total_price := total_price + order_record.priceSum;
    END LOOP;

    DBMS_OUTPUT.put_line('Total price paid for orders: ' || total_price || ' CZK');

EXCEPTION WHEN OTHERS THEN
        total_price := NULL;
        DBMS_OUTPUT.PUT_LINE('Error: ' || SQLERRM);
END;
/

BEGIN
    DBMS_OUTPUT.put_line('Customer ID: 2');
    total_price(2);
    DBMS_OUTPUT.put_line('Customer ID: 6');
    total_price(6);
END;
/

------------------------------------- EXPLAIN PLAN--------------------------------------
-- Explain plan for the following queries and create indexes to improve their performance.

EXPLAIN PLAN FOR
    SELECT c.categoryName, AVG(p.price) AS avgPrice
    FROM product p
    JOIN category c ON p.categoryID = c.categoryID
    GROUP BY c.categoryName;
-- print the plan
SELECT * FROM TABLE(DBMS_XPLAN.DISPLAY);

CREATE INDEX categoryPrice_idx ON product(categoryID, price);

EXPLAIN PLAN FOR
    SELECT c.categoryName, AVG(p.price) AS avgPrice
    FROM product p
    JOIN category c ON p.categoryID = c.categoryID
    GROUP BY c.categoryName;
-- print the plan
SELECT * FROM TABLE(DBMS_XPLAN.DISPLAY);
-------------------------------------- PRIVILEGES --------------------------------------
-- Grant privileges to the other member of the team.
GRANT ALL ON contains TO xstrig00;
GRANT ALL ON review TO xstrig00;
GRANT ALL ON productOrder TO xstrig00;
GRANT ALL ON product TO xstrig00;
GRANT ALL ON category TO xstrig00;
GRANT ALL ON employee TO xstrig00;
GRANT ALL ON customer TO xstrig00;
GRANT ALL ON visitor TO xstrig00;

GRANT EXECUTE ON update_product_stock TO xstrig00;
GRANT EXECUTE ON total_price TO xstrig00;

---------------------------------- MATERIALIZED VIEW -----------------------------------
-- VIEW on usernames of customers that got their order shipped via Packeta from 'customer' and 'productOrder' tables.
-- is performed by the second member of the team (xstrig00).
CREATE MATERIALIZED VIEW customers_shipped_via_Packeta AS
    SELECT loginName,
            orderDate
    FROM xdurac01.customer NATURAL JOIN xdurac01.productOrder p WHERE p.deliveryMethod = 'Packeta'
    ORDER BY orderDate DESC;
    
SELECT * FROM customers_shipped_via_Packeta;

GRANT ALL ON customers_shipped_via_Packeta TO xstrig00;

----------------------------------- SELECT STATEMENT -----------------------------------
-- SELECT statement lists the order ID, date, and total price of each order, and categorizes the order total

WITH order_total AS (
  SELECT
    productOrder.orderID,
    productOrder.priceSum AS total
  FROM productOrder
  JOIN product ON product.productID = productOrder.productID
  GROUP BY productOrder.orderID, productOrder.priceSum
)
SELECT
  productOrder.orderID,
  productOrder.orderDate,
  order_total.total AS orderTotal,
  CASE
    WHEN order_total.total > 3000 THEN 'High Total'
    WHEN order_total.total > 1000 THEN 'Medium Total'
    ELSE 'Low Total'
  END AS totalCategory
FROM productOrder
JOIN order_total ON order_total.orderID = productOrder.orderID
GROUP BY productOrder.orderDate, order_total.total, productOrder.orderID
ORDER BY productOrder.orderDate DESC;