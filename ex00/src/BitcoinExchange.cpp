/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sklaokli <sklaokli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 14:57:16 by sklaokli          #+#    #+#             */
/*   Updated: 2026/05/22 02:10:19 by sklaokli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BitcoinExchange.hpp"

// --- Exception --- //

BitcoinExchange::Exception::Exception() : _msg("Error: BitcoinExchange") {}

BitcoinExchange::Exception::Exception(const std::string& msg)
    : _msg("Error: " + msg) {}

BitcoinExchange::Exception::Exception(const Exception& rhs) : _msg(rhs._msg) {}

BitcoinExchange::Exception& BitcoinExchange::Exception::operator=(
    const Exception& rhs) {
	if (this != &rhs) _msg = rhs._msg;
	return *this;
}

BitcoinExchange::Exception::~Exception() throw() {}

const char* BitcoinExchange::Exception::what() const throw() {
	return _msg.c_str();
}

// --- BitcoinExchange --- //

BitcoinExchange::BitcoinExchange() {}

BitcoinExchange::BitcoinExchange(const BitcoinExchange& rhs)
    : _database(rhs._database) {}

BitcoinExchange& BitcoinExchange::operator=(const BitcoinExchange& rhs) {
	if (this != &rhs) {
		_database = rhs._database;
	}
	return *this;
}

BitcoinExchange::~BitcoinExchange() {}

std::string BitcoinExchange::trimWhitespace(const std::string& str) const {
	std::string::size_type first = str.find_first_not_of(" \t\r\n");
	if (first == std::string::npos) {
		return "";
	}
	std::string::size_type last = str.find_last_not_of(" \t\r\n");
	return str.substr(first, (last - first + 1));
}

std::vector<std::string> BitcoinExchange::splitTokens(
    const std::string& str, char delimiter) const {
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

std::string BitcoinExchange::getPresentDateString() const {
	std::time_t rawTime = std::time(NULL);
	std::tm* timeInfo = std::localtime(&rawTime);

	char buffer[11];
	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeInfo);

	return std::string(buffer);
}

int BitcoinExchange::extractDigits(const std::string& str) const {
	for (std::string::size_type i = 0; i < str.length(); ++i) {
		if (!std::isdigit(str[i])) return -1;
	}
	return std::atoi(str.c_str());
}

/**
 * @brief Validates if a date string is chronologically and contextually valid.
 *
 * Implements strict calendar validations under C++98 compliance:
 *
 * 1. isInvalidFormat: Enforces structural checks (Must be YYYY-MM-DD).
 *
 * 2. isOutOfBoundsDate: Rejects inputs predating 2009-01-02 or postdating
 *    the dynamic present time of the local host system clock.
 *
 * 3. containsNonDigits: Rejects alpha characters caught by extractDigits().
 *
 * 4. isLeapYear: Dynamically updates February's max capacity to 29 days.
 *
 * 5. impossibleDate: Enforces month boundaries and check if the day is
 *    possible.
 *
 * @param date The raw date string to validate.
 * @return true if the date is fully valid, false otherwise.
 */
bool BitcoinExchange::isValidDate(const std::string& date) const {
	bool isInvalidFormat =
	    (date.length() != 10 || date[4] != '-' || date[7] != '-');

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

double BitcoinExchange::parseValue(
    const std::string& str, bool isDataBase) const {
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

bool BitcoinExchange::isValidHeader(const std::vector<std::string>& tokens,
    const std::string& match1, const std::string& match2) const {
	std::string firstRowHeader = tokens[0];
	std::string secondRowHeader = tokens[1];
	return firstRowHeader == match1 && secondRowHeader == match2;
}

void BitcoinExchange::loadDatabase(const std::string& path) {
	std::ifstream dbFile(path.c_str());
	if (!dbFile.is_open()) {
		throw Exception(std::string(strerror(errno)) + " => " + path);
	}

	double exchangeRate;
	std::string date;

	std::string line;
	std::vector<std::string> tokens;
	bool is_header = true;

	while (std::getline(dbFile, line)) {
		if (line.empty()) {
			continue;
		}
		tokens = splitTokens(line, ',');
		if (tokens.size() != 2) {
			std::cerr << "Error: bad input => " << line << std::endl;
			continue;
		}
		if (is_header) {
			if (!isValidHeader(tokens, "date", "exchange_rate")) {
				throw Exception("invalid database file header structure.");
			}
			is_header = false;
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
}

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

void BitcoinExchange::processInput(const std::string& path) {
	std::ifstream inputFile(path.c_str());
	if (!inputFile.is_open()) {
		throw Exception(std::string(strerror(errno)) + " => " + path);
	}

	double value;
	std::string date;
	double amount;

	std::string line;
	std::vector<std::string> tokens;
	bool is_header = true;

	while (std::getline(inputFile, line)) {
		if (line.empty()) {
			continue;
		}
		tokens = splitTokens(line, '|');
		if (tokens.size() != 2) {
			std::cerr << "Error: bad input => " << line << std::endl;
			continue;
		}
		if (is_header) {
			if (!isValidHeader(tokens, "date", "value")) {
				throw Exception("invalid input file header structure.");
			}
			is_header = false;
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
}
