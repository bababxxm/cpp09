/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sklaokli <sklaokli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 14:57:16 by sklaokli          #+#    #+#             */
/*   Updated: 2026/05/23 09:47:47 by sklaokli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BitcoinExchange.hpp"
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

/* ************************************************************************** */
/*                                                                            */
/*                               EXCEPTIONS                                   */
/*                                                                            */
/* ************************************************************************** */

const char* BitcoinExchange::FileOpenException::what() const throw() {
	return "Error: could not open file.";
}

const char* BitcoinExchange::DatabaseHeaderException::what() const throw() {
	return "Error: bad database header.";
}

const char* BitcoinExchange::InputHeaderException::what() const throw() {
	return "Error: bad input header.";
}

/* ************************************************************************** */
/*                                                                            */
/*                          ORTHODOX CANONICAL FORMS                          */
/*                                                                            */
/* ************************************************************************** */

BitcoinExchange::BitcoinExchange() {}

BitcoinExchange::BitcoinExchange(const BitcoinExchange& other)
    : _database(other._database) {}

BitcoinExchange& BitcoinExchange::operator=(const BitcoinExchange& other) {
	if (this != &other) {
		_database = other._database;
	}
	return *this;
}

BitcoinExchange::~BitcoinExchange() {}

/* ************************************************************************** */
/*                                                                            */
/*                             PUBLIC METHODS                                 */
/*                                                                            */
/* ************************************************************************** */

/**
 * @brief Parses the internal reference CSV database to populate exchange rates.
 *
 * Opens the target database file stream and validates its structural format.
 * Skips invalid calendar dates and bad numerical input rows. Populates the
 * internal lookup map with valid date-to-rate pairs.
 *
 * @param path System path to the historical CSV database file.
 * @throws Exception if the file cannot be accessed or headers are malformed.
 */
void BitcoinExchange::loadDatabase(const std::string& path) {
	std::ifstream dbFile(path.c_str());
	if (!dbFile.is_open()) {
		std::cerr << strerror(errno) << " => " << path << std::endl;
		throw BitcoinExchange::FileOpenException();
	}

	std::string date;
	double exchangeRate;
	std::string line;
	std::vector<std::string> tokens;
	bool is_header = true;

	while (std::getline(dbFile, line)) {
		if (line.empty()) {
			continue;
		}
		if (is_header) {
			if (line != "date,exchange_rate") {
				throw BitcoinExchange::DatabaseHeaderException();
			}
			is_header = false;
			continue;
		}
		tokens = splitTokens(line, ',');
		if (tokens.size() != 2) {
			std::cerr << "Error: bad input => " << line << std::endl;
			continue;
		}
		date = tokens[0];
		if (!isValidDate(date)) {
			continue;
		}
		exchangeRate = parseValue(tokens[1], true);
		if (exchangeRate == -1.0) {
			continue;
		}
		_database[date] = exchangeRate;
	}
	dbFile.close();
}

/**
 * @brief Main execution loop for parsing and evaluating client query lists.
 *
 * Streams the user input file line-by-line. Coordinates string tokenization,
 * structural validation, date consistency checks, and exchange amount lookups
 * before printing the final transaction evaluation directly to standard output.
 *
 * @param path System path to the client input file to process.
 * @throws Exception if the input file cannot be opened.
 */
void BitcoinExchange::processInput(const std::string& path) {
	std::ifstream inputFile(path.c_str());
	if (!inputFile.is_open()) {
		std::cerr << strerror(errno) << " => " << path << std::endl;
		throw BitcoinExchange::FileOpenException();
	}

	std::string date;
	double value;
	double amount;
	std::string line;
	std::vector<std::string> tokens;
	bool is_header = true;

	while (std::getline(inputFile, line)) {
		if (line.empty()) {
			continue;
		}
		if (is_header) {
			if (line != "date | value") {
				throw BitcoinExchange::InputHeaderException();
			}
			is_header = false;
			continue;
		}
		tokens = splitTokens(line, '|');
		if (tokens.size() != 2) {
			std::cerr << "Error: bad input => " << line << std::endl;
			continue;
		}
		date = tokens[0];
		if (!isValidDate(date)) {
			continue;
		}
		value = parseValue(tokens[1], false);
		if (value == -1.0) {
			continue;
		}
		amount = getExchangeAmount(date, value);
		if (amount == -1.0) {
			continue;
		}
		std::cout << date << " => " << value << " = " << amount << std::endl;
	}
	inputFile.close();
}

/* ************************************************************************** */
/*                                                                            */
/*                             PRIVATE METHODS                                */
/*                                                                            */
/* ************************************************************************** */

/**
 * @brief Resolves the target currency amount using closest historical records.
 *
 * Employs a lower_bound binary search strategy on the database map. Returns a
 * constant iterator pointing to the first element equal to or greater than the
 * key. If the exact date does not exist, it decrements the iterator to fall
 * back to the closest past chronological record.
 *
 * @param date The target date string (YYYY-MM-DD) to locate.
 * @param value The multiplier transaction currency volume amount.
 * @return Total calculated value, or -1.0 if the date predates the database.
 */
double BitcoinExchange::getExchangeAmount(
    const std::string& date, double value) const {
	std::map<std::string, double>::const_iterator it =
	    _database.lower_bound(date);
	if (it != _database.end() && it->first != date) {
		if (it == _database.begin()) {
			std::cerr << "Error: no exchange data available before "
			          << it->first << std::endl;
			return -1.0;
		}
		--it;
	} else if (it == _database.end()) {
		if (!_database.empty()) {
			it = --_database.end();
		} else {
			std::cerr << "Error: exchange database is empty." << std::endl;
			return -1.0;
		}
	}
	return value * it->second;
}

/* ************************************************************************** */
/*                                                                            */
/*                              UTILS & HELPERS                               */
/*                                                                            */
/* ************************************************************************** */

bool BitcoinExchange::isValidDate(const std::string& date) {
	bool isInvalidFormat =
	    (date.size() != 10 || date[4] != '-' || date[7] != '-');

	if (isInvalidFormat) {
		std::cerr << "Error: bad input => " << date << std::endl;
		return false;
	}

	int year = extractDigits(date.substr(0, 4));
	int month = extractDigits(date.substr(5, 2));
	int day = extractDigits(date.substr(8, 2));

	bool containsNonDigits = (year == -1 || month == -1 || day == -1);

	if (containsNonDigits) {
		std::cerr << "Error: date contains non-digit characters => " << date
		          << std::endl;
		return false;
	}

	std::string presentDate = getPresentDateString();
	bool isOutOfBoundsDate = (date < "2009-01-02" || date > presentDate);

	if (isOutOfBoundsDate) {
		std::cerr << "Error: bad input => " << date << std::endl;
		return false;
	}

	int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);

	if (isLeapYear) {
		daysInMonth[2] = 29;
	}

	bool impossibleDate =
	    (month < 1 || month > 12 || day < 1 || day > daysInMonth[month]);

	if (impossibleDate) {
		std::cerr << "Error: impossible calendar date => " << date << std::endl;
		return false;
	}

	return true;
}

/**
 * @brief Validates and converts a string into a double precision value.
 *
 * Parses raw strings into float representations using robust pointer trailing
 * validations. Enforces structural rules and strictly limits values to a
 * maximum threshold of 1000.0 for evaluation queries.
 *
 * @param str The raw string to extract the value from.
 * @param isDataBase Boolean flag to bypass the 1000.0 maximum ceiling rule.
 * @return The parsed double value, or -1.0 if syntax or bounds checks fail.
 */
double BitcoinExchange::parseValue(const std::string& str, bool isDataBase) {
	if (str.empty()) {
		std::cerr << "Error: empty value." << std::endl;
		return -1.0;
	}

	std::string::size_type index = 0;
	if (str[0] == '+') {
		index++;
	}

	char* endPtr;
	double value = std::strtod(str.c_str() + index, &endPtr);

	bool isBadInput = (endPtr == str.c_str() + index || *endPtr != '\0');

	if (isBadInput) {
		std::cerr << "Error: bad input => " << str << std::endl;
		return -1.0;
	}

	if (value < 0.0) {
		std::cerr << "Error: not a positive number." << std::endl;
		return -1.0;
	}

	if (!isDataBase && value > 1000.0) {
		std::cerr << "Error: too large a number." << std::endl;
		return -1.0;
	}

	return value;
}

std::string BitcoinExchange::trimWhitespace(const std::string& str) {
	const std::string whiteSpace = " \t\r\n";
	std::string::size_type first = str.find_first_not_of(whiteSpace);
	if (first == std::string::npos) {
		return "";
	}
	std::string::size_type last = str.find_last_not_of(whiteSpace);
	return str.substr(first, (last - first + 1));
}

std::vector<std::string> BitcoinExchange::splitTokens(
    const std::string& str, char delimiter) {
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;

	while (std::getline(ss, token, delimiter)) {
		tokens.push_back(token);
	}
	for (std::vector<std::string>::iterator it = tokens.begin();
	     it != tokens.end(); ++it) {
		*it = trimWhitespace(*it);
	}
	return tokens;
}

std::string BitcoinExchange::getPresentDateString() {
	std::time_t rawTime = std::time(NULL);
	std::tm* timeInfo = std::localtime(&rawTime);

	char buffer[11];
	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeInfo);

	return std::string(buffer);
}

int BitcoinExchange::extractDigits(const std::string& str) {
	for (std::string::size_type i = 0; i < str.size(); ++i) {
		if (!std::isdigit(str[i])) return -1;
	}
	return std::atoi(str.c_str());
}
