/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sklaokli <sklaokli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 04:16:02 by sklaokli          #+#    #+#             */
/*   Updated: 2026/05/23 09:45:18 by sklaokli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RPN_HPP
#define RPN_HPP

#include <exception>
#include <string>

class RPN {
public:
	class InsufficientTokensException : public std::exception {
	public:
		virtual const char* what() const throw();
	};

	class InvalidTokenException : public std::exception {
	public:
		virtual const char* what() const throw();
	};

	class DivisionByZeroException : public std::exception {
	public:
		virtual const char* what() const throw();
	};

	class MalformedExpressionException : public std::exception {
	public:
		virtual const char* what() const throw();
	};

	class EmptyExpressionException : public std::exception {
	public:
		virtual const char* what() const throw();
	};

	static void calculate(const std::string& expession);
private:
	RPN();
	RPN(const RPN&);
	RPN& operator=(const RPN&);
	~RPN();

	static bool isOperator(char);
	static int applyOperator(char, int, int);
};

#endif
