#pragma once

#include <vector>
#include <iostream>
#include <cmath>

template<typename T>
class Matrix2D {
public:
    Matrix2D() { }
    Matrix2D(size_t x, size_t y) :
        matrix(std::vector<std::vector<T>>(x, std::vector<T>(y, 0))) { }

    explicit Matrix2D(size_t n) {
        matrix.resize(n, std::vector<T>(n, 0));
        for(size_t i = 0; i < n; i++) {
            matrix[i][i] = 1;
        }
    }

    explicit Matrix2D(const std::vector<std::vector<T>>& vector) {
        if(vector.size() == 0 || vector.at(0).size() == 0){
            throw std::invalid_argument("invalid parameters");
        }
        matrix = vector;
    }

    Matrix2D(const Matrix2D& other) : matrix(other.matrix) { }
    Matrix2D(Matrix2D&& other) : matrix(std::move(other.matrix)) { }

    const T& at(size_t x, size_t y) const {
        return matrix.at(x).at(y);
    }

    T& at(size_t x, size_t y) {
        return matrix.at(x).at(y);
    }

    void clear() {
        matrix.clear();
    }

    void resize(size_t x, size_t y) {
        matrix.resize(x, std::vector<T>(y));
    }

    std::pair<size_t, size_t> size() const {
        size_t y = matrix.size() == 0 ? 0 : matrix.at(0).size();
        return {matrix.size(), y};
    }

    Matrix2D operator*(const Matrix2D& other) const {
        Size thisSize = (Size)size();
        Size otherSize = (Size)other.size();
        if (thisSize.y != otherSize.x) {
            throw std::logic_error("sizes mismatch");
        }
        Matrix2D result(thisSize.x, otherSize.y);
        for (size_t i = 0; i < thisSize.x; i++) {
            for (size_t j = 0; j < otherSize.y; j++) {
                for (size_t k = 0; k < otherSize.x; k++) {
                    result.at(i, j) += (at(i, k) * other.at(k, j));
                }
            }
        }
        return result;
    }

    Matrix2D operator+(const Matrix2D& other) const {
        Size thisSize = (Size)size();
        Size otherSize = (Size)other.size();
        if (thisSize != otherSize) {
            throw std::logic_error("sizes mismatch");
        }
        Matrix2D result(thisSize.x, otherSize.y);
        for (size_t i = 0; i < thisSize.x; i++) {
            for (size_t j = 0; j < otherSize.y; j++) {
                result.at(i, j) = at(i, j) + other.at(i, j);
            }
        }
        return result;
    }

    Matrix2D operator-(const Matrix2D& other) const {
        Size thisSize = (Size)size();
        Size otherSize = (Size)other.size();
        if (thisSize != otherSize) {
            throw std::logic_error("sizes mismatch");
        }
        Matrix2D result(thisSize.x, otherSize.y);
        for (size_t i = 0; i < thisSize.x; i++) {
            for (size_t j = 0; j < otherSize.y; j++) {
                result.at(i, j) = at(i, j) - other.at(i, j);
            }
        }
        return result;
    }

    template<typename U>
    friend Matrix2D<U> operator*(const Matrix2D<U>& lhs, const U& rhs);
    template<typename U>
    friend Matrix2D<U> operator*(const U& lhs, const Matrix2D<U>& rhs);

    Matrix2D& operator=(const Matrix2D& other) {
        matrix = other.matrix;
        return *this;
    }

    Matrix2D& operator=(Matrix2D&& other) {
        matrix = move(other.matrix);
        return *this;
    }

    Matrix2D pow(size_t n) const {
        Size thisSize = (Size)size();
        if(thisSize.x != thisSize.y) {
            throw std::logic_error("sizes mismatch");
        }
        Matrix2D result(thisSize.x);
        Matrix2D temp(*this);
        while(n) {
            if (n & 1)
                result = result * temp;
            temp = temp * temp;
            n >>= 1;
        }
        return result;
    }

private:
    std::vector<std::vector<T>> matrix;

    class Size {
    public:
        size_t x, y;
        explicit Size(const std::pair<size_t, size_t>& pair) {
            x = pair.first;
            y = pair.second;
        }
        Size(size_t first, size_t second) {
            x = first;
            y = second;
        }

        inline bool operator==(const Size& other) const {
            return x == other.x && y == other.y;
        }

        inline bool operator!=(const Size& other) const {
            return !(*this == other);
        }
    };

    Matrix2D multiplyByValue(const T& other) const {
        Size thisSize = (Size)size();
        if (thisSize.x == 0 || thisSize.y == 0) {
            throw std::logic_error("cannot multiply zero matrix");
        }
        Matrix2D result(thisSize.x, thisSize.y);
        for (size_t i = 0; i < thisSize.x; i++) {
            for (size_t j = 0; j < thisSize.y; j++) {
                result.at(i, j) = at(i, j) * other;
            }
        }
        return result;
    }
};

template <typename U>
Matrix2D<U> operator*(const Matrix2D<U> &lhs, const U& rhs) {
    return lhs.multiplyByValue(rhs);
}

template<typename U>
Matrix2D<U> operator*(const U& lhs, const Matrix2D<U> &rhs) {
    return rhs.multiplyByValue(lhs);
}


template<typename T>
std::ostream& operator<<(std::ostream& out, const Matrix2D<T>& m){
    out << '[';
    for(int i = 0; i < m.size().first; i++){
        if(i != 0){
            out << '\n';
        }
        out << '[';
        for(int j = 0; j < m.size().second; j++){
            if(j != 0){
                out << ", ";
            }
            out << m.at(i, j);
        }
        out << ']';
    }
    out << "]\n";
    return out;
}

