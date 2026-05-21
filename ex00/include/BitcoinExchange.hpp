/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sklaokli <sklaokli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 15:32:58 by sklaokli          #+#    #+#             */
/*   Updated: 2026/05/22 02:08:57 by sklaokli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BITCOINEXCHANGE_HPP
#define BITCOINEXCHANGE_HPP

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

class BitcoinExchange {
public:
	class Exception : public std::exception {
	public:
		Exception();
		Exception(const std::string&);
		Exception(const Exception&);
		Exception& operator=(const Exception&);
		virtual ~Exception() throw();
		virtual const char* what() const throw();
	private:
		std::string _msg;
	};

	BitcoinExchange();
	BitcoinExchange(const BitcoinExchange&);
	BitcoinExchange& operator=(const BitcoinExchange&);
	~BitcoinExchange();

	void loadDatabase(const std::string&);
	void processInput(const std::string&);
private:
	std::string trimWhitespace(const std::string& str) const;
	std::vector<std::string> splitTokens(const std::string&, char) const;
	bool isValidHeader(const std::vector<std::string>& tokens,
	    const std::string& match1, const std::string& match2) const;
	int extractDigits(const std::string&) const;
	std::string getPresentDateString() const;
	bool isValidDate(const std::string&) const;
	double parseValue(const std::string&, bool) const;
	double getExchangeAmount(const std::string& date, double value) const;

	std::map<std::string, double> _database;
};

#endif
