-- Meal Management System Schema
-- Version 1.0

-- This script creates all necessary tables for the Meal Management System.
-- It is recommended to create a dedicated database and user before running this script.
--
-- Example MySQL commands to run first:
-- CREATE DATABASE meal_management;
-- CREATE USER 'meal_user'@'localhost' IDENTIFIED BY 'your_password';
-- GRANT ALL PRIVILEGES ON meal_management.* TO 'meal_user'@'localhost';
-- FLUSH PRIVILEGES;
-- USE meal_management;

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

--
-- Table structure for `users`
--
DROP TABLE IF EXISTS `users`;
CREATE TABLE `users`  (
  `id` int NOT NULL AUTO_INCREMENT,
  `username` varchar(50) NOT NULL,
  `password_hash` varchar(255) NOT NULL,
  `salt` varchar(64) NOT NULL,
  `name` varchar(100) NULL DEFAULT NULL,
  `role` enum('Student','Staff','Admin') NULL DEFAULT 'Student',
  PRIMARY KEY (`id`),
  UNIQUE INDEX `username_unique` (`username`)
) ENGINE = InnoDB;

--
-- Table structure for `meal_periods`
--
DROP TABLE IF EXISTS `meal_periods`;
CREATE TABLE `meal_periods`  (
  `id` int NOT NULL AUTO_INCREMENT,
  `month` varchar(20) NULL DEFAULT NULL,
  `year` varchar(4) NULL DEFAULT NULL,
  `is_active` tinyint(1) NULL DEFAULT 1,
  PRIMARY KEY (`id`),
  UNIQUE INDEX `period_unique`(`month`, `year`)
) ENGINE = InnoDB;

--
-- Table structure for `menu_items`
--
DROP TABLE IF EXISTS `menu_items`;
CREATE TABLE `menu_items`  (
  `id` int NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE INDEX `name_unique` (`name`)
) ENGINE = InnoDB;

--
-- Table structure for `daily_menus`
--
DROP TABLE IF EXISTS `daily_menus`;
CREATE TABLE `daily_menus`  (
  `id` int NOT NULL AUTO_INCREMENT,
  `menu_date` date NOT NULL,
  `meal_type` enum('Breakfast','Lunch','Dinner') NOT NULL,
  `menu_item_id` int NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE INDEX `daily_menu_unique`(`menu_date`, `meal_type`, `menu_item_id`),
  CONSTRAINT `fk_daily_menus_menu_item` FOREIGN KEY (`menu_item_id`) REFERENCES `menu_items` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE = InnoDB;

--
-- Table structure for `expenses`
--
DROP TABLE IF EXISTS `expenses`;
CREATE TABLE `expenses`  (
  `id` int NOT NULL AUTO_INCREMENT,
  `purchase_date` date NULL DEFAULT NULL,
  `item_name` varchar(255) NULL DEFAULT NULL,
  `price` decimal(10, 2) NULL DEFAULT NULL,
  `paid_by_user_id` int NULL DEFAULT NULL,
  `category` varchar(100) NULL DEFAULT NULL,
  PRIMARY KEY (`id`),
  CONSTRAINT `fk_expenses_user` FOREIGN KEY (`paid_by_user_id`) REFERENCES `users` (`id`) ON DELETE SET NULL ON UPDATE CASCADE
) ENGINE = InnoDB;

--
-- Table structure for `meal_attendance`
--
DROP TABLE IF EXISTS `meal_attendance`;
CREATE TABLE `meal_attendance`  (
  `id` int NOT NULL AUTO_INCREMENT,
  `user_id` int NULL DEFAULT NULL,
  `attendance_date` date NULL DEFAULT NULL,
  `meal_type` enum('Breakfast','Lunch','Dinner') NULL DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE INDEX `user_meal_unique`(`user_id`, `attendance_date`, `meal_type`),
  CONSTRAINT `fk_attendance_user` FOREIGN KEY (`user_id`) REFERENCES `users` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE = InnoDB;

--
-- Table structure for `payments`
--
DROP TABLE IF EXISTS `payments`;
CREATE TABLE `payments` (
    `id` INT NOT NULL AUTO_INCREMENT,
    `user_id` INT NULL,
    `amount` DECIMAL(10, 2) NOT NULL,
    `date` DATE NOT NULL,
    PRIMARY KEY (`id`),
    CONSTRAINT `fk_payments_user` FOREIGN KEY (`user_id`) REFERENCES `users` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE = InnoDB;

--
-- Table structure for `settings`
--
DROP TABLE IF EXISTS `settings`;
CREATE TABLE `settings` (
    `id` INT NOT NULL,
    `currency` VARCHAR(10) NOT NULL DEFAULT 'USD',
    PRIMARY KEY (`id`)
) ENGINE = InnoDB;

-- Default settings record
INSERT INTO `settings` (`id`, `currency`) VALUES (1, 'USD');

SET FOREIGN_KEY_CHECKS = 1;