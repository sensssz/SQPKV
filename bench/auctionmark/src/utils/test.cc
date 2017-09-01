#include "nullable.h"

#include <iostream>
#include <string>

int main() {
    auctionmark::Nullable<int> a = 0;
    auctionmark::Nullable<std::string> b = std::string("jiamin huang");
    std::cout << a.ToString() << std::endl;
    std::cout << b.ToString() << std::endl;
    return 0;
}

