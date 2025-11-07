module;

#include <eigen3/Eigen/Eigen>
#include <eigen3/Eigen/Geometry>

export module mwc_math_definition;

import mwc_definition;
import mwc_concept;

import std;

export namespace mwc {
  namespace math {
    enum class dense_storage_order_et : int32_t {
      e_column_major = Eigen::ColMajor,
      e_row_major = Eigen::RowMajor,
    };
    enum class dense_storage_type_et : int32_t {
      e_isometry = Eigen::Isometry,
      e_affine = Eigen::Affine,
      e_affine_compact = Eigen::AffineCompact,
      e_projective = Eigen::Projective,
    };
    template <typename tp, size_t tp_count>
      requires concepts::arithmetic_size_c<tp, tp_count>
    using vector_t = Eigen::Vector<tp, tp_count>;

    template <typename tp, size_t tp_row_count, size_t tp_column_count,
              dense_storage_order_et tp_storage_order = dense_storage_order_et::e_column_major>
      requires concepts::arithmetic_size_c<tp, tp_row_count> and concepts::arithmetic_size_c<tp, tp_column_count>
    using matrix_t = Eigen::Matrix<tp, tp_row_count, tp_column_count, std::to_underlying(tp_storage_order)>;

    template <std::floating_point tp>
    using quaternion_t = Eigen::Quaternion<tp>;

    template <std::floating_point tp, size_t tp_count, dense_storage_type_et tp_storage_type,
              dense_storage_order_et tp_storage_order = dense_storage_order_et::e_column_major>
    using transformation_t
      = Eigen::Transform<tp, tp_count, std::to_underlying(tp_storage_type), std::to_underlying(tp_storage_order)>;

    template <std::floating_point tp>
    using angle_axis_t = Eigen::AngleAxis<tp>;
  }
}