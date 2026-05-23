/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sklaokli <sklaokli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 07:54:16 by sklaokli          #+#    #+#             */
/*   Updated: 2026/05/23 09:42:50 by sklaokli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RPN.hpp"
#include <iostream>
#include <sstream>
#include <stack>

/* ************************************************************************** */
/*                                                                            */
/*                                 EXCEPTIONS                                 */
/*                                                                            */
/* ************************************************************************** */

const char* RPN::InsufficientTokensException::what() const throw() {
	return "Error: insufficient tokens";
}

const char* RPN::InvalidTokenException::what() const throw() {
	return "Error: invalid token detected";
}

const char* RPN::DivisionByZeroException::what() const throw() {
	return "Error: division by zero is forbidden";
}

const char* RPN::MalformedExpressionException::what() const throw() {
	return "Error: too many operands left over";
}

const char* RPN::EmptyExpressionException::what() const throw() {
	return "Error: empty expression";
}

/* ************************************************************************** */
/*                                                                            */
/*                          ORTHODOX CANONICAL FORMS                          */
/*                                                                            */
/* ************************************************************************** */

RPN::RPN() {}

RPN::RPN(const RPN& other) {
	(void)other;
}

RPN& RPN::operator=(const RPN& other) {
	(void)other;
	return *this;
}

RPN::~RPN() {}

/* ************************************************************************** */
/*                                                                            */
/*                               PUBLIC METHODS                               */
/*                                                                            */
/* ************************************************************************** */

void RPN::calculate(const std::string& expression) {
	std::string token;
	std::stack<int> stack;
	std::stringstream ss(expression);

	try {
		while (ss >> token) {
			if (token.size() == 1 && std::isdigit(token[0])) {
				int n = token[0] - '0';
				stack.push(n);
			} else if (token.size() == 1 && isOperator(token[0])) {
				if (stack.size() < 2) {
					throw RPN::InsufficientTokensException();
				}
				int b = stack.top();
				stack.pop();
				int a = stack.top();
				stack.pop();
				char op = token[0];
				int result = applyOperator(op, a, b);
				stack.push(result);
			} else {
				throw RPN::InvalidTokenException();
			}
		}
		if (stack.size() > 1) {
			throw RPN::MalformedExpressionException();
		} else if (stack.size() == 0) {
			throw RPN::EmptyExpressionException();
		}
		std::cout << stack.top() << std::endl;
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}

/* ************************************************************************** */
/*                                                                            */
/*                              UTILS & HELPERS                               */
/*                                                                            */
/* ************************************************************************** */

bool RPN::isOperator(char c) {
	return (c == '+' || c == '-' || c == '*' || c == '/');
}

int RPN::applyOperator(char op, int a, int b) {
	switch (op) {
		case '+':
			return a + b;
		case '-':
			return a - b;
		case '*':
			return a * b;
		case '/':
			if (b == 0) {
				throw RPN::DivisionByZeroException();
			}
			return a / b;
		default:
			throw RPN::InvalidTokenException();
	}
}
