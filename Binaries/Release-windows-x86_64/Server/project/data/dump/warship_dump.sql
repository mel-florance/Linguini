-- MySQL dump 10.13  Distrib 5.7.31, for Linux (x86_64)
--
-- Host: localhost    Database: warship
-- ------------------------------------------------------
-- Server version	5.7.31-0ubuntu0.18.04.1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Current Database: `warship`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `warship` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `warship`;

--
-- Table structure for table `ACCOUNT`
--

DROP TABLE IF EXISTS `ACCOUNT`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ACCOUNT` (
  `ID` int(20) NOT NULL AUTO_INCREMENT,
  `ACCOUNT_ID` int(100) NOT NULL,
  `NAME` varchar(20) NOT NULL,
  `CREATION_DATE` datetime NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `ACCOUNT`
--

LOCK TABLES `ACCOUNT` WRITE;
/*!40000 ALTER TABLE `ACCOUNT` DISABLE KEYS */;
INSERT INTO `ACCOUNT` VALUES (1,503152629,'Rod24','2020-09-29 10:42:07');
/*!40000 ALTER TABLE `ACCOUNT` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `BATTLE`
--

DROP TABLE IF EXISTS `BATTLE`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `BATTLE` (
  `ID` int(20) NOT NULL AUTO_INCREMENT,
  `ACCOUNT_ID` int(20) DEFAULT NULL,
  `SHIP_ID` int(20) DEFAULT NULL,
  `EXP` int(20) NOT NULL,
  `CREATION_DATE` datetime NOT NULL,
  PRIMARY KEY (`ID`),
  KEY `ACCOUNT_ID` (`ACCOUNT_ID`),
  KEY `SHIP_ID` (`SHIP_ID`),
  CONSTRAINT `BATTLE_ibfk_1` FOREIGN KEY (`ACCOUNT_ID`) REFERENCES `ACCOUNT` (`ID`) ON DELETE CASCADE,
  CONSTRAINT `BATTLE_ibfk_2` FOREIGN KEY (`SHIP_ID`) REFERENCES `SHIP` (`ID`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `BATTLE`
--

LOCK TABLES `BATTLE` WRITE;
/*!40000 ALTER TABLE `BATTLE` DISABLE KEYS */;
/*!40000 ALTER TABLE `BATTLE` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `BATTLEHISTORY`
--

DROP TABLE IF EXISTS `BATTLEHISTORY`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `BATTLEHISTORY` (
  `ID` int(20) NOT NULL AUTO_INCREMENT,
  `ACCOUNT_ID` int(20) DEFAULT NULL,
  `BATTLE_ID` int(20) DEFAULT NULL,
  `LOG` text NOT NULL,
  `CREATION_DATE` datetime NOT NULL,
  PRIMARY KEY (`ID`),
  KEY `ACCOUNT_ID` (`ACCOUNT_ID`),
  KEY `BATTLE_ID` (`BATTLE_ID`),
  CONSTRAINT `BATTLEHISTORY_ibfk_1` FOREIGN KEY (`ACCOUNT_ID`) REFERENCES `ACCOUNT` (`ID`) ON DELETE CASCADE,
  CONSTRAINT `BATTLEHISTORY_ibfk_2` FOREIGN KEY (`BATTLE_ID`) REFERENCES `BATTLE` (`ID`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `BATTLEHISTORY`
--

LOCK TABLES `BATTLEHISTORY` WRITE;
/*!40000 ALTER TABLE `BATTLEHISTORY` DISABLE KEYS */;
/*!40000 ALTER TABLE `BATTLEHISTORY` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `RESEARCH`
--

DROP TABLE IF EXISTS `RESEARCH`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `RESEARCH` (
  `ID` int(20) NOT NULL AUTO_INCREMENT,
  `ACCOUNT_ID` int(20) NOT NULL,
  `SHIP_ID` int(20) NOT NULL,
  `EXP` int(20) NOT NULL,
  `CREATION_DATE` datetime NOT NULL,
  PRIMARY KEY (`ID`),
  KEY `ACCOUNT_ID` (`ACCOUNT_ID`),
  KEY `SHIP_ID` (`SHIP_ID`),
  CONSTRAINT `RESEARCH_ibfk_1` FOREIGN KEY (`ACCOUNT_ID`) REFERENCES `ACCOUNT` (`ID`) ON DELETE CASCADE,
  CONSTRAINT `RESEARCH_ibfk_2` FOREIGN KEY (`SHIP_ID`) REFERENCES `SHIP` (`ID`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `RESEARCH`
--

LOCK TABLES `RESEARCH` WRITE;
/*!40000 ALTER TABLE `RESEARCH` DISABLE KEYS */;
/*!40000 ALTER TABLE `RESEARCH` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `SHIP`
--

DROP TABLE IF EXISTS `SHIP`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `SHIP` (
  `ID` int(20) NOT NULL AUTO_INCREMENT,
  `SHIP_ID` int(100) DEFAULT NULL,
  `NAME` varchar(100) NOT NULL,
  `NEXT_SHIP` text,
  `CREATION_DATE` datetime NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `SHIP`
--

LOCK TABLES `SHIP` WRITE;
/*!40000 ALTER TABLE `SHIP` DISABLE KEYS */;
/*!40000 ALTER TABLE `SHIP` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `SequelizeMeta`
--

DROP TABLE IF EXISTS `SequelizeMeta`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `SequelizeMeta` (
  `name` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`name`),
  UNIQUE KEY `name` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `SequelizeMeta`
--

LOCK TABLES `SequelizeMeta` WRITE;
/*!40000 ALTER TABLE `SequelizeMeta` DISABLE KEYS */;
INSERT INTO `SequelizeMeta` VALUES ('20201125142218-create-account.js'),('20201125142232-create-ship.js'),('20201125142302-create-research.js'),('20201125142314-create-battle.js'),('20201125142347-create-battlehistory.js');
/*!40000 ALTER TABLE `SequelizeMeta` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Current Database: `warship`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `warship` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `warship`;

--
-- Table structure for table `ACCOUNT`
--

DROP TABLE IF EXISTS `ACCOUNT`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ACCOUNT` (
  `ID` int(20) NOT NULL AUTO_INCREMENT,
  `ACCOUNT_ID` int(100) NOT NULL,
  `NAME` varchar(20) NOT NULL,
  `CREATION_DATE` datetime NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `ACCOUNT`
--

LOCK TABLES `ACCOUNT` WRITE;
/*!40000 ALTER TABLE `ACCOUNT` DISABLE KEYS */;
INSERT INTO `ACCOUNT` VALUES (1,503152629,'Rod24','2020-09-29 10:42:07');
/*!40000 ALTER TABLE `ACCOUNT` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `BATTLE`
--

DROP TABLE IF EXISTS `BATTLE`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `BATTLE` (
  `ID` int(20) NOT NULL AUTO_INCREMENT,
  `ACCOUNT_ID` int(20) DEFAULT NULL,
  `SHIP_ID` int(20) DEFAULT NULL,
  `EXP` int(20) NOT NULL,
  `CREATION_DATE` datetime NOT NULL,
  PRIMARY KEY (`ID`),
  KEY `ACCOUNT_ID` (`ACCOUNT_ID`),
  KEY `SHIP_ID` (`SHIP_ID`),
  CONSTRAINT `BATTLE_ibfk_1` FOREIGN KEY (`ACCOUNT_ID`) REFERENCES `ACCOUNT` (`ID`) ON DELETE CASCADE,
  CONSTRAINT `BATTLE_ibfk_2` FOREIGN KEY (`SHIP_ID`) REFERENCES `SHIP` (`ID`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `BATTLE`
--

LOCK TABLES `BATTLE` WRITE;
/*!40000 ALTER TABLE `BATTLE` DISABLE KEYS */;
/*!40000 ALTER TABLE `BATTLE` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `BATTLEHISTORY`
--

DROP TABLE IF EXISTS `BATTLEHISTORY`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `BATTLEHISTORY` (
  `ID` int(20) NOT NULL AUTO_INCREMENT,
  `ACCOUNT_ID` int(20) DEFAULT NULL,
  `BATTLE_ID` int(20) DEFAULT NULL,
  `LOG` text NOT NULL,
  `CREATION_DATE` datetime NOT NULL,
  PRIMARY KEY (`ID`),
  KEY `ACCOUNT_ID` (`ACCOUNT_ID`),
  KEY `BATTLE_ID` (`BATTLE_ID`),
  CONSTRAINT `BATTLEHISTORY_ibfk_1` FOREIGN KEY (`ACCOUNT_ID`) REFERENCES `ACCOUNT` (`ID`) ON DELETE CASCADE,
  CONSTRAINT `BATTLEHISTORY_ibfk_2` FOREIGN KEY (`BATTLE_ID`) REFERENCES `BATTLE` (`ID`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `BATTLEHISTORY`
--

LOCK TABLES `BATTLEHISTORY` WRITE;
/*!40000 ALTER TABLE `BATTLEHISTORY` DISABLE KEYS */;
/*!40000 ALTER TABLE `BATTLEHISTORY` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `RESEARCH`
--

DROP TABLE IF EXISTS `RESEARCH`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `RESEARCH` (
  `ID` int(20) NOT NULL AUTO_INCREMENT,
  `ACCOUNT_ID` int(20) NOT NULL,
  `SHIP_ID` int(20) NOT NULL,
  `EXP` int(20) NOT NULL,
  `CREATION_DATE` datetime NOT NULL,
  PRIMARY KEY (`ID`),
  KEY `ACCOUNT_ID` (`ACCOUNT_ID`),
  KEY `SHIP_ID` (`SHIP_ID`),
  CONSTRAINT `RESEARCH_ibfk_1` FOREIGN KEY (`ACCOUNT_ID`) REFERENCES `ACCOUNT` (`ID`) ON DELETE CASCADE,
  CONSTRAINT `RESEARCH_ibfk_2` FOREIGN KEY (`SHIP_ID`) REFERENCES `SHIP` (`ID`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `RESEARCH`
--

LOCK TABLES `RESEARCH` WRITE;
/*!40000 ALTER TABLE `RESEARCH` DISABLE KEYS */;
/*!40000 ALTER TABLE `RESEARCH` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `SHIP`
--

DROP TABLE IF EXISTS `SHIP`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `SHIP` (
  `ID` int(20) NOT NULL AUTO_INCREMENT,
  `SHIP_ID` int(100) DEFAULT NULL,
  `NAME` varchar(100) NOT NULL,
  `NEXT_SHIP` text,
  `CREATION_DATE` datetime NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `SHIP`
--

LOCK TABLES `SHIP` WRITE;
/*!40000 ALTER TABLE `SHIP` DISABLE KEYS */;
/*!40000 ALTER TABLE `SHIP` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `SequelizeMeta`
--

DROP TABLE IF EXISTS `SequelizeMeta`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `SequelizeMeta` (
  `name` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`name`),
  UNIQUE KEY `name` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `SequelizeMeta`
--

LOCK TABLES `SequelizeMeta` WRITE;
/*!40000 ALTER TABLE `SequelizeMeta` DISABLE KEYS */;
INSERT INTO `SequelizeMeta` VALUES ('20201125142218-create-account.js'),('20201125142232-create-ship.js'),('20201125142302-create-research.js'),('20201125142314-create-battle.js'),('20201125142347-create-battlehistory.js');
/*!40000 ALTER TABLE `SequelizeMeta` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2020-12-26 23:51:43
