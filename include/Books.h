#ifndef BOOKS_H
#define BOOKS_H

#include <string>

namespace Bookit
{

struct Book
{
    std::string name{};
    std::string author{};
    std::string year{};
    std::string isbn{};
    std::string category{};
};

} // namespace Bookit

#endif // BOOKS_H
