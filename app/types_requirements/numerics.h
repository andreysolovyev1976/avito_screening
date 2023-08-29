//
// Created by Andrey Solovyev on 29/01/2023.
//

#pragma once

#include <type_traits>
#ifdef __cpp_concepts
#include <concepts>
#endif
#include <ratio>

#ifndef TYPE_REQUIREMENTS_NUMERICS_H
#define TYPE_REQUIREMENTS_NUMERICS_H


namespace culib::requirements {

#ifndef __cpp_concepts

  template <typename Number>
  using IsFloatinPoint = std::enable_if_t<std::is_floating_point_v<Number>, bool>;
  template <typename Number>
  using IsNotFloatinPoint = std::enable_if_t<not std::is_floating_point_v<Number>, bool>;

  template <typename Number>
  using IsIntegral = std::enable_if_t<std::is_integral_v<Number>, bool>;

  template <typename Number>
  using IsNotIntegral = std::enable_if_t<not std::is_integral_v<Number>, bool>;

  template <typename Number>
  using IsArithmetic = std::enable_if_t<std::is_arithmetic_v<Number>, bool>;;

  template <typename Number>
  using IsNotArithmetic = std::enable_if_t<!std::is_arithmetic_v<Number>, bool>;;



  namespace details {
	template<typename I, typename = void>
	struct MaybeIncrementable : std::false_type { };
	template<typename I>
	struct MaybeIncrementable<I, std::void_t<
			decltype(std::declval<I>()++),
			decltype(++std::declval<I>())>> : std::true_type {};
  }//!namespace

  template <typename I>
  inline constexpr bool is_incrementable_v { details::MaybeIncrementable<I>::value };

  template <typename Iter>
  using IsIncrementable = std::enable_if_t<is_incrementable_v<Iter>, bool>;

  namespace details {
	template <typename I, typename = void>
	struct MaybeDecrementable : std::false_type {} ;
	template <typename I>
	struct MaybeDecrementable<I, std::void_t<
			decltype(std::declval<I>()--),
			decltype(--std::declval<I>())>> : std::true_type {};
  }//!namespace

  template <typename I>
  inline constexpr bool is_decrementable_v { details::MaybeDecrementable<I>::value };

  template <typename Iter>
  using IsDecrementable = std::enable_if_t<is_decrementable_v<Iter>, bool>;




  namespace details {
	template <typename R, typename = void>
	struct MaybeStdRatio : std::false_type {} ;
	template <typename R>
	struct MaybeStdRatio<R, std::void_t<
			decltype(R::num),
			decltype(R::den),
			decltype(std::declval<std::ratio_equal<R, R>>())>
	> : std::true_type {};
  }//!namespace

  template <typename R>
  inline constexpr bool is_ratio_v { details::MaybeStdRatio<R>::value };

  template <typename R>
  using IsStdRatio = std::enable_if_t<is_ratio_v<R>, bool>;



#else

  /**
   * @details
   * IsIntegral and IsFloating are the std:: \n
   * concepts, there is no need to define those\n
   * in C++20.\n
   * */


  template <typename Number>
  concept IsArithmetic = std::integral<Number> || std::floating_point<Number>;

  template <typename Number>
  concept IsNotArithmetic = !IsArithmetic<Number>;





  template <typename I>
  concept Incrementable = requires(I i) {
	  ++i;
	  i++;
  };
  template <typename I>
  concept Decrementable = requires(I i) {
	  --i;
	  i--;
  };



  template <typename Ratio>
  concept IsStdRatio = requires () {
	  Ratio::num;
	  Ratio::den;
	  std::ratio_equal<Ratio, Ratio>{};
  };
  template <typename R>
  inline constexpr bool is_ratio_v { IsStdRatio<R> ? true : false };


#endif


}//!namespace
#endif //TYPE_REQUIREMENTS_NUMERICS_H

