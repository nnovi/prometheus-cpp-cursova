#ifndef COURSE_UTILS_H
#define COURSE_UTILS_H

#include <iostream>
#include <vector>

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& arr) {
    os << "[";
    bool first = true;
    for (auto it : arr) {
        if (!first)
            os << ", ";
        else
            first = false;
        os << it;
    }
    os << "]";
    return os;
}

void skip_spaces(std::istream &is) {
    is.ignore ( std::numeric_limits<std::streamsize>::max(), '\n' );
}

#endif //COURSE_UTILS_H
