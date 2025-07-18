module;

#include "eigen3/Eigen/Eigen"

export module mwc_math_definition;

import mwc_definition;
import mwc_concept;

import std;

export namespace mwc {
  namespace math {
    template <typename tp, size_t tp_count>
      requires concepts::arithmetic_size_c<tp, tp_count>
    using vector_t = Eigen::Vector<tp, tp_count>;

    template <typename tp, size_t tp_row_count, size_t tp_column_count>
      requires concepts::arithmetic_size_c<tp, tp_row_count> and concepts::arithmetic_size_c<tp, tp_column_count>
    using matrix_t = Eigen::Matrix<tp, tp_row_count, tp_column_count>;

    template <typename tp>
      requires std::is_arithmetic_v<tp>
    using quaternion_t = Eigen::Quaternion<tp>;
  }
}