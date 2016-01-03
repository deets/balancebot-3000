#include <type_traits>

// Convenient user-interface function, deduces `T`.
template<typename T>
Json::Value toJson(const T& x)
{
    // Instantiate a temporary `json_converter` and call `to_json`.
    return json_converter<typename std::decay<T>::type>{}.toJson(x);
}


template<>
struct json_converter<quaternion_t> {
  static Json::Value toJson(const quaternion_t& quat) {
    Json::Value quatJ(Json::arrayValue);
    quatJ.resize(4);
    quatJ[0] = quat.w();
    quatJ[1] = quat.x();
    quatJ[2] = quat.y();
    quatJ[3] = quat.z();
    return quatJ;
  }
};

template<>
struct json_converter<vector3_t> {
  static Json::Value toJson(const vector3_t& vec) {
    Json::Value vecJ(Json::arrayValue);
    vecJ.resize(3);
    vecJ[0] = vec(0);
    vecJ[1] = vec(1);
    vecJ[2] = vec(2);
    return vecJ;
  }
};

template<int Rows, int Cols, typename FloatT>
struct json_converter<Eigen::Matrix<FloatT, Rows, Cols>> {
  using MatrixT = Eigen::Matrix<FloatT, Rows, Cols>;
  static Json::Value toJson(const MatrixT& m) {
    Json::Value res(Json::arrayValue);
    res.resize(Rows);
    for(int i=0; i < Rows; i++) {
      Json::Value row(Json::arrayValue);
      row.resize(Cols);
      for(int j=0; j < Cols; j++) {
	row[j] = m(i, j);
      }
      res[i] = row;
    }
    return res;
  }
};
