#pragma once
#include <vector>
#include <functional>
#include <optional>

template<typename T>
class SegmentTree2D {
public:
    SegmentTree2D(const std::vector<std::vector<T>>& initialMatrix,
                  std::function<T(T, T)> func) {
        matrix = initialMatrix;
        function = func;
        segmentTree2D.resize(4 * columnsSize(),
                             std::vector<T>(4 * rowsSize()));
        build2D(1, Range(0, static_cast<int64_t>(columnsSize()) - 1));
    }

    SegmentTree2D(const SegmentTree2D& other) {
        function = other.function;
        matrix = other.matrix;
        segmentTree2D = other.segmentTree2D;
    }

    SegmentTree2D(SegmentTree2D&& other) {
        function = std::move(other.function);
        matrix = std::move(other.matrix);
        segmentTree2D = std::move(other.segmentTree2D);
    }

    T query(size_t fromColumn, size_t toColumn,
            size_t fromRow, size_t toRow) const {
        const Range range(0, static_cast<int64_t>(columnsSize()) - 1);
        const Rectangle rectangle = {{fromColumn, fromRow}, {toColumn, toRow}};
        std::optional<T> result = query2D(1, range, rectangle);
        if(!result.has_value()){
            throw std::invalid_argument("invalid indexes");
        }
        return result.value();
    }

    void update(size_t column, size_t row, const T& newValue) {
        const Range range(0, static_cast<int64_t>(columnsSize()) - 1);
        update2D(1, range, {column, row}, newValue);
    }

    size_t columnsSize() const {
        return matrix.size();
    }

    size_t rowsSize() const {
        if (columnsSize() == 0) {
            return 0;
        }
        return matrix.at(0).size();
    }

    void clear() {
        segmentTree2D.clear();
        matrix.clear();
        function = nullptr;
    }

    SegmentTree2D operator=(const SegmentTree2D& other) {
        function = other.function;
        matrix = other.matrix;
        segmentTree2D = other.segmentTree2D;
    }

    SegmentTree2D operator=(SegmentTree2D&& other) {
        function = std::move(other.function);
        matrix = std::move(other.matrix);
        segmentTree2D = std::move(other.segmentTree2D);
    }

private:
    std::vector<std::vector<T>> segmentTree2D;
    std::vector<std::vector<T>> matrix;
    std::function<T(T, T)> function;

    struct Point {
        size_t x;
        size_t y;
    };

    struct Rectangle {
        Point bottomLeft;
        Point topRight;
    };

    class Range {
    public:
        size_t lowerBound;
        size_t upperBound;

        Range(size_t lower, size_t upper) {
            lowerBound = lower;
            upperBound = upper;
        }

        bool checkForBoundsEquality() const {
            return lowerBound == upperBound;
        }

        size_t getMedium() const {
            return (lowerBound + upperBound) / 2;
        }
    };

    // builds segment tree along the first (x) axis.
    void build(std::vector<T>& segmentTree,
               const std::vector<T>& array, size_t index,
               const Range& range) {
        if (range.checkForBoundsEquality()) {
            segmentTree.at(index) = array.at(range.lowerBound);
        } else {
            const size_t medium = range.getMedium();

            build(segmentTree, array, 2 * index,
                  Range(range.lowerBound, medium));
            build(segmentTree, array, 2 * index + 1,
                  Range(medium + 1, range.upperBound));

            segmentTree.at(index) = function(segmentTree.at(2 * index),
                                             segmentTree.at(2 * index + 1));
        }
    }

    // builds final version of segment tree along the second (y) axis.
    // call this function to build segment tree.
    void build2D(size_t index, const Range& range) {
        if (range.checkForBoundsEquality()) {
            build(segmentTree2D.at(index),
                  matrix.at(range.lowerBound), 1,
                  Range(0, static_cast<int64_t>(rowsSize()) - 1));
        } else {
            const size_t medium = range.getMedium();
            build2D(2 * index, Range(range.lowerBound, medium));
            build2D(2 * index + 1, Range(medium + 1, range.upperBound));

            const size_t rowSize =  segmentTree2D.at(index).size();

            for (size_t column = 0; column < rowSize; ++column) {
                segmentTree2D.at(index).at(column) =
                        function(segmentTree2D.at(2 * index).at(column),
                                 segmentTree2D.at(2 * index + 1).at(column));
            }
        }
    }

    // applies function in segment tree along the first (x) axis.
     std::optional<T> query(const std::vector<T>& segmentTree, size_t index,
                            const Range& queryRange,
                            const Range& fixedRange) const {
        if (fixedRange.lowerBound > queryRange.upperBound ||
            fixedRange.upperBound < queryRange.lowerBound) {
            return std::nullopt;
        }

        if (queryRange.lowerBound >= fixedRange.lowerBound &&
            queryRange.upperBound <= fixedRange.upperBound) {
            const T value = segmentTree.at(index);
            return std::optional<T>(value);
        }

        const size_t medium = queryRange.getMedium();

        const std::optional<T> leftQuery =
                query(segmentTree, 2 * index,
                      Range(queryRange.lowerBound, medium), fixedRange);

        const std::optional<T> rightQuery =
                query(segmentTree, 2 * index + 1,
                      Range(medium + 1, queryRange.upperBound), fixedRange);

        const T result = function(leftQuery.value(), rightQuery.value());

        return std::optional<T>(result);
    }

    // applies function in segment tree
    // along the second (y) axis and returns final result.
    // call this function to get query result in the rectangle.
    std::optional<T> query2D(size_t index,
                    const Range& columnsRange,
                    const Rectangle& rect) const {
        if (columnsRange.lowerBound > rect.topRight.x ||
            columnsRange.upperBound < rect.bottomLeft.x) {
            return std::nullopt;
        }

        if (columnsRange.lowerBound >= rect.bottomLeft.x &&
            columnsRange.upperBound <= rect.topRight.x) {
            const std::optional<T> queryResult =
                    query(segmentTree2D[index], 1,
                          Range(0, static_cast<int64_t>(rowsSize()) - 1),
                          Range(rect.bottomLeft.y, rect.topRight.y));
            return queryResult;
        }

        const size_t medium = columnsRange.getMedium();
        const std::optional<T> leftQuery =
                query2D(2 * index,
                        Range(columnsRange.lowerBound, medium), rect);
        const std::optional<T> rightQuery =
                query2D(2 * index + 1,
                        Range(medium + 1, columnsRange.upperBound), rect);

        const T result = function(leftQuery.value(), rightQuery.value());
        return std::optional<T>(result);
    }

    // updates segment tree along the first (x) axis.
    void update(const Range& columnRange,
                const Range& rowRange, size_t indexX,
                size_t indexY,
                const Point& point, const T& value) {
        if (rowRange.checkForBoundsEquality()) {
            if (columnRange.checkForBoundsEquality()) {
                segmentTree2D.at(indexX).at(indexY) = value;
            } else {
                segmentTree2D.at(indexX).at(indexY) =
                        function(segmentTree2D.at(indexX * 2).at(indexY),
                                 segmentTree2D.at(indexX * 2 + 1).at(indexY));
            }
        } else {
            const size_t mediumRow = rowRange.getMedium();
            if (point.y <= mediumRow) {
                update(columnRange,
                       Range(rowRange.lowerBound, mediumRow),
                       indexX, indexY * 2, point, value);
            } else {
                update(columnRange,
                       Range(mediumRow + 1, rowRange.upperBound),
                       indexX, indexY * 2 + 1, point, value);
            }
            segmentTree2D.at(indexX).at(indexY) =
                    function(segmentTree2D.at(indexX).at(indexY * 2),
                             segmentTree2D.at(indexX).at(indexY * 2 + 1));
        }
    }

    // updates segment tree along the second (y) axis.
    // call this function to update value in the tree.
    void update2D(size_t index, const Range& columnRange,
                  const Point& point, const T& value) {
        if (!columnRange.checkForBoundsEquality()) {
            const size_t medium = columnRange.getMedium();
            if (point.x <= medium){
                update2D(index * 2,
                         Range(columnRange.lowerBound, medium), point, value);
            } else {
                update2D(index * 2 + 1,
                         Range(medium + 1, columnRange.upperBound),
                         point, value);
            }
        }

        update(columnRange, Range(0, static_cast<int64_t>(rowsSize()) - 1),
               index, 1, point, value);
    }
};

