#pragma once

#include <ostream>

namespace matrix
{
    template <typename TYPE, int ROW, int COL>
    class Matrix
    {
    public:
        using type = TYPE;
        static constexpr int rows = ROW;
        static constexpr int cols = COL;

        Matrix();

        Matrix<TYPE, ROW, COL>
        operator+(const Matrix<TYPE, ROW, COL> mat) const;

        Matrix<TYPE, ROW, COL>
        operator-(const Matrix<TYPE, ROW, COL> mat) const;

        template <int OUT_COL>
        Matrix<TYPE, ROW, OUT_COL>
        operator*(const Matrix<TYPE, COL, OUT_COL> mat) const;

        const TYPE* operator[](int idx) const;
        TYPE* operator[](int idx);

        // friend std::ostream &operator<<(std::ostream &os,
        //                                 const Matrix<TYPE, ROW, COL>);

    private:
        TYPE mat_[rows][cols];
    };

    template <typename TYPE, int ROW>
    using Vector = Matrix<TYPE, ROW, 1>;

    using Matrix4F = Matrix<float, 4, 4>;
    using Vector4F = Vector<float, 4>;
} // namespace matrix

#include <matrix/matrix.hxx>
