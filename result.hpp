#include <utility>
#include <string>
#include <functional>
#include <iostream>


template <typename T>
constexpr T valueOf();
// TODO:

// Logical AND
// success && success
// failure && success
// failure && failure
// success && result
// failure && result
// result  && result

// Logical OR
// success || success
// failure || success
// failure || failure
// success || result
// failure || result
// result  || result

// Conclude/Consume/Finalize
// result(sfunc, ffunc)

// Chaining of computation
// res >> func >> func

#ifndef RESULT_HPP
#define RESULT_HPP

/// Computation that resulted in a failure.
///
/// @tparam F failure type
template <typename F>
struct failure
{
  F _value;
};

/// Convenience.
template <typename F>
failure(F) -> failure<F>;

/// Computation that resulted in a success.
template <typename S>
struct success
{
  S _value;

  template<class Fun, class Ret = decltype(valueOf<Fun>()(_value))>
  auto operator>>(Fun&& fun) {
    std::cout << "doing success>>" << std::endl;
    return fun(_value);
  }
};

/// Convenience.
template <typename S>
success(S) -> success<S>;

/// Parametric type representing a result of a computation, addressing one of
/// the possible outcomes: success or failure.
///
/// @tparam S success type
/// @tparam F failure type
template <typename S, typename F>
class result
{
  private:
    /// Storage of the result in a tagged union.
    union
    {
      success<S> _success;
      failure<F> _failure; 
    };

    /// Tag of the union, where `true` means that the success alternative is
    /// chosen, and `false` translates to failure.
    bool _tag;

  public:
    /// Create a result representing success.
    ///
    /// @param val success value
    result(success<S> const& val) {
      _success = val;
      _tag     = true;
    }

    ///
    result(failure<F> const& val) {
      _failure = val;
      _tag     = false;
    }

    /// 
    result(success<S>&& val) {
      _success = std::move(val);
      _tag     = true;
    }

    ///
    result(failure<F>&& val) {
      _failure = std::move(val);
      _tag     = false;
    }

    ///
    template <typename S2>
    result(success<S2> const& val) {
      _success = success{S(val._value)};
      _tag     = true;
    }

    ///
    template <typename F2>
    result(failure<F2> const& val) {
      _failure = failure{F(val._value)};
      _tag     = false;
    }

    /// Create a result representing success that provides implicit conversion
    /// from an another success type.
    template <typename S2>
    result(success<S2>&& val2) {
      S val(std::move(val2._value));
      _success = success{S(std::move(val._value))};
      _tag     = true;
    }

    /// Create a result representing failure 
    template <typename F2>
    result(failure<F2>&& val) {
      _failure = failure{F(std::move(val._value))};
      _tag     = false;
    }

    /// Create a copy of another result.
    result(result const&) = default;

    /// Create a copy of another result.
    result(result&&) = default;

    /// @brief Combine two results into one. .
    ///
    /// This function adheres to the following table:
    ///   success{x} && success{y} = success{y}
    ///   success{x} && failure{y} = failure{y}
    ///   failure{x} && success{y} = failure{x}
    ///   failure{x} && failure{y} = failure{x}
    ///
    /// @tparam S2    another success type
    /// @param  other another result
    template <typename S2>
    result<S2, F> operator&&(result<S2, F> const& other) {
      if (_tag == true) {
	      return other;
      } else {
	      return failure{_failure};
      }
    }

    /// @brief Combine two results into one.
    ///
    /// This function adheres to the following table:
    ///   success{x} || success{y} = success{x}
    ///   success{x} || failure{y} = success{x}
    ///   failure{x} || success{y} = success{y}
    ///   failure{x} || failure{y} = failure{y}
    ///
    /// @tparam F2    another failure type
    /// @param  other another result
    template <typename F2>
    result<S, F2> operator||(result<S, F2> const& other) {
      if (_tag == true) {
        return other;
      } else {
        return other;
      }
    }

    template<class Fun, class Ret = decltype(valueOf<Fun>()(_success._value))>
    auto operator>>(Fun&& fun) {
      std::cout << "_tag = " << _tag << std::endl;
      return _tag ? fun(_success._value) : _failure;
    }

    template<class Fun, class Ret = decltype(valueOf<Fun>()(_success._value))>
    auto and_then(Fun&& fun) {
      return _tag ? fun(_success._value) : _failure;
    }

    template<class FunS, class FunF,
	     class RetS = decltype(valueOf<FunS>()(_success._value)),
	     class RetF = decltype(valueOf<FunF>()(_failure._value))>
    auto operator()(FunS&& succ_fun, FunF&& fail_fun) {
      return _tag ? succ_fun(_success._value) : fail_fun(_failure._value);
    }

    /// @brief Destructor that invokes the appropriate destructor.
    ~result() {
      if (_tag == true) {
        _success.~success<S>();
      } else {
        _failure.~failure<F>();
      }
    }
};

template <typename S1, typename S2>
success<S2> operator&&(success<S1> const& val1, success<S2> const& val2)
{
  return val2;
}

//template<class Fun, class Ret = decltype(valueOf<Fun>()(_success._value))>
//auto operator>>(auto other& res, Fun&& fun) {
//  std::cout << "_tag = " << _tag << std::endl;
//  return _tag ? fun(_success._value) : _failure;
//}

#endif
