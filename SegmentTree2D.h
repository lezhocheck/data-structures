/* Author: Oleh Toporkov */

/* Usage:
    const int INFINITY = 1'000'000'000;

    template<typename T>
    T max(const T& a, const T& b) {
        return std::max(a, b);
    }

    template<typename T>
    T sum(const T& a, const T& b) {
        return a + b;
    }

    vector<vector<int>> matrix = {
            {1, 2, 3},
            {4, 5, 6},
            {7, 8, 9}
    };

 1) Case for maximum

    SegmentTree2D<int> seg(matrix, max, -INFINITY);
    seg.update(0, 0, 12);
    int a = seg.query(1, 2, 1, 2);

2) Case for sum

    SegmentTree2D<int> seg(matrix, sum, 0);
    seg.update(0, 0, 12);
    int a = seg.query(1, 2, 1, 2);

 */


template<typename T>
class SegmentTree2D {
public:
    // 'id' is a default element of applied operation. i.e. for integers: '0' for sum or '-INFINITY' for maximum
    explicit SegmentTree2D(const std::vector<std::vector<T>>& initialMatrix,
                           T function(const T&, const T&), const T& identityElement) {
        matrix = initialMatrix;
        func = function;
        id = identityElement;
        segmentTree2D.resize(4 * columnsSize(),
                             std::vector<T>(4 * rowsSize(), 0));
        build2D(1, Range(0, columnsSize() - 1));
    }

    // get query on range [fromColumn, toColumn], [fromRow, toRow]
    T query(const size_t fromColumn, const size_t toColumn,
                  const size_t fromRow, const size_t toRow) const {

        const Range range(0, columnsSize() - 1);
        const Rectangle rectangle = {Point(fromColumn, fromRow),
                                     Point(toColumn, toRow)};
        const T result = query2D(1, range, rectangle);

        return result;
    }

    void update(const size_t column,
                const size_t row, const T& newValue) {

        const Range range(0, columnsSize() - 1);
        const Point point(column, row);
        update2D(1, range, point, newValue);
    }

    size_t columnsSize() const {
        const size_t columns = matrix.size();
        return columns;
    }

    size_t rowsSize() const {
        if (columnsSize() == 0) {
            return 0;
        }
        const size_t rows = matrix.at(0).size();
        return rows;
    }

private:
    std::vector<std::vector<T>> segmentTree2D;
    std::vector<std::vector<T>> matrix;
    T (*func)(const T&, const T&);
    T id;

    class Point {
    public:
        size_t x;
        size_t y;

        Point(const size_t xValue,
              const size_t yValue) {
            x = xValue;
            y = yValue;
        }
    };

    struct Rectangle {
        Point bottomLeft;
        Point topRight;
    };

    class Range {
    public:
        size_t lowerBound;
        size_t upperBound;

        Range(const size_t lower, const size_t upper) {
            lowerBound = lower;
            upperBound = upper;
        }

        bool checkForBoundsEquality() const {
            const bool result = lowerBound == upperBound;
            return result;
        }

        size_t getMedium() const {
            const size_t result = (lowerBound + upperBound) / 2;
            return result;
        }
    };

    // builds segment tree along the first (x) axis.
    void build(std::vector<T>* segmentTree,
               const std::vector<T>& array,
               const size_t index, const Range& range) {

        if (range.checkForBoundsEquality()) {
            segmentTree->at(index) = array.at(range.lowerBound);
        } else {
            const size_t medium = range.getMedium();

            build(segmentTree, array, 2 * index,
                  Range(range.lowerBound, medium));
            build(segmentTree, array, 2 * index + 1,
                  Range(medium + 1, range.upperBound));

            segmentTree->at(index) = func(segmentTree->at(2 * index),
                                              segmentTree->at(2 * index + 1));
        }
    }

    // builds final version of segment tree along the second (y) axis.
    // call this function to build segment tree.
    void build2D(const size_t index, const Range& range) {
        if (range.checkForBoundsEquality()) {
            build(&segmentTree2D.at(index),
                  matrix.at(range.lowerBound), 1,
                  Range(0, rowsSize() - 1));
        } else {
            const size_t medium = range.getMedium();
            build2D(2 * index, Range(range.lowerBound, medium));
            build2D(2 * index + 1, Range(medium + 1, range.upperBound));

            const size_t rowSize = segmentTree2D.at(index).size();

            for (size_t column = 0; column < rowSize; ++column) {
                segmentTree2D.at(index).at(column) =
                        func(segmentTree2D.at(2 * index).at(column),
                                 segmentTree2D.at(2 * index + 1).at(column));
            }
        }
    }

    // finds maximum value in segment tree along the first (x) axis.
    T query(const std::vector<T>& segmentTree, const size_t index,
                  const Range& queryRange, const Range& fixedRange) const {
        if (fixedRange.lowerBound > queryRange.upperBound ||
            fixedRange.upperBound < queryRange.lowerBound) {
            return id;
        }

        if (queryRange.lowerBound >= fixedRange.lowerBound &&
            queryRange.upperBound <= fixedRange.upperBound) {
            return segmentTree.at(index);
        }

        const size_t medium = queryRange.getMedium();
        const T leftQuery = query(segmentTree, 2 * index,
                                        Range(queryRange.lowerBound, medium), fixedRange);
        const T rightQuery = query(segmentTree, 2 * index + 1,
                                         Range(medium + 1, queryRange.upperBound), fixedRange);

        const T result = func(leftQuery, rightQuery);
        return result;
    }

    // finds maximum value in segment tree
    // along the second (y) axis and returns final result.
    // call this function to get maximum value in the rectangle.
    T query2D(const size_t index,
                    const Range& columnsRange,
                    const Rectangle& rect) const {
        if (columnsRange.lowerBound > rect.topRight.x ||
            columnsRange.upperBound < rect.bottomLeft.x) {
            return id;
        }

        if (columnsRange.lowerBound >= rect.bottomLeft.x &&
            columnsRange.upperBound <= rect.topRight.x) {
            const T queryResult = query(segmentTree2D[index], 1,
                                              Range(0, rowsSize() - 1),
                                              Range(rect.bottomLeft.y, rect.topRight.y));
            return queryResult;
        }

        const size_t medium = columnsRange.getMedium();
        const T leftQuery = query2D(2 * index,
                                          Range(columnsRange.lowerBound, medium), rect);
        const T rightQuery = query2D(2 * index + 1,
                                           Range(medium + 1, columnsRange.upperBound), rect);
        const T result = func(leftQuery, rightQuery);

        return result;
    }

    // updates segment tree along the first (x) axis.
    void update(const Range& columnRange,
                const Range& rowRange, const size_t indexX,
                const size_t indexY,
                const Point& point, const T& value) {

        if (rowRange.checkForBoundsEquality()) {
            if (columnRange.checkForBoundsEquality()) {
                segmentTree2D.at(indexX).at(indexY) = value;
            } else {
                segmentTree2D.at(indexX).at(indexY) =
                        func(segmentTree2D.at(indexX * 2).at(indexY),
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
                    func(segmentTree2D.at(indexX).at(indexY * 2),
                             segmentTree2D.at(indexX).at(indexY * 2 + 1));
        }
    }

    // updates segment tree along the second (y) axis.
    // call this function to update value in the tree.
    void update2D(const size_t index, const Range& columnRange,
                  const Point& point, const T& value) {
        if (!columnRange.checkForBoundsEquality()) {
            const size_t medium = columnRange.getMedium();
            if (point.x <= medium){
                update2D(index * 2,
                         Range(columnRange.lowerBound, medium), point, value);
            } else {
                update2D(index * 2 + 1,
                         Range(medium + 1, columnRange.upperBound), point, value);
            }
        }

        update(columnRange, Range(0, rowsSize() - 1),
               index, 1, point, value);
    }
};
