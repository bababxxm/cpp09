/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sklaokli <sklaokli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 04:15:32 by sklaokli          #+#    #+#             */
/*   Updated: 2026/05/23 09:50:50 by sklaokli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdlib>
#include <iostream>
#include <string>
#include "RPN.hpp"

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cerr << "Usage: ./RPN [expression]" << std::endl;
		return EXIT_FAILURE;
	}

	try {
		RPN::calculate(argv[1]);
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
