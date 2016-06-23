// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Config.hpp>
#include "MultiNameUtils.hpp"
#include <iostream>
#include <cstdlib>

int main(void)
{
    std::cout << "test toIndexedName()..." << std::endl;
    if (toIndexedName("test", 123) != "test[123]") return EXIT_FAILURE;

    std::cout << "test isIndexedName()..." << std::endl;
    if (isIndexedName("test")) return EXIT_FAILURE;
    if (isIndexedName("test 123")) return EXIT_FAILURE;
    if (not isIndexedName("test[123]")) return EXIT_FAILURE;

    std::cout << "test splitIndexedName()..." << std::endl;
    size_t index;
    if (splitIndexedName("test[123]", index) != "test") return EXIT_FAILURE;
    if (index != 123) return EXIT_FAILURE;
    try
    {
        splitIndexedName("test 123", index); //should throw
        return EXIT_FAILURE;
    }
    catch (const std::exception &ex){}

    std::cout << "test csvSplit()..." << std::endl;
    const auto split = csvSplit("foo1, bar2, baz3");
    if (split.size() != 3) return EXIT_FAILURE;
    if (split.at(0) != "foo1") return EXIT_FAILURE;
    if (split.at(1) != "bar2") return EXIT_FAILURE;
    if (split.at(2) != "baz3") return EXIT_FAILURE;

    std::cout << "test csvJoin()..." << std::endl;
    if (csvJoin(split) != "foo1, bar2, baz3") return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
