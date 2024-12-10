// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#include "UniversalUpdater.hpp"

std::unique_ptr<UU> UU::App = nullptr;


int main(int ARGC, char *ARGV[]) {
	UU::App = std::make_unique<UU>();
	return UU::App->Handler(ARGV);
};