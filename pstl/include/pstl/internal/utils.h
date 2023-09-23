// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _PSTL_UTILS_H
#define _PSTL_UTILS_H

#include <new>
#include <iterator>
#include <type_traits>
#include <tuple>
#include <utility>
#include <climits>

_PSTL_HIDE_FROM_ABI_PUSH

namespace __pstl
{
namespace __internal
{

template <typename _Fp>
auto
__except_handler(_Fp __f) -> decltype(__f())
{
    try
    {
        return __f();
    }
    catch (const ::std::bad_alloc&)
    {
        throw; // re-throw bad_alloc according to the standard [algorithms.parallel.exceptions]
    }
    catch (...)
    {
        ::std::terminate(); // Good bye according to the standard [algorithms.parallel.exceptions]
    }
}

template <typename _Op>
struct __invoke_unary_op
{
    mutable _Op __op;

    template <typename _Input, typename _Output>
    void
    operator()(_Input&& __x, _Output&& __y) const
    {
        __y = __op(::std::forward<_Input>(__x));
    }
};

//! Unary operator that returns reference to its argument.
struct __no_op
{
    template <typename _Tp>
    _Tp&&
    operator()(_Tp&& __a) const
    {
        return ::std::forward<_Tp>(__a);
    }
};

//! Logical negation of a predicate
template <typename _Pred>
class __not_pred
{
    _Pred _M_pred;

  public:
    explicit __not_pred(_Pred __pred) : _M_pred(__pred) {}

    template <typename... _Args>
    bool
    operator()(_Args&&... __args) const
    {
        return !_M_pred(::std::forward<_Args>(__args)...);
    }
};

template <typename _Pred>
class __reorder_pred
{
    mutable _Pred _M_pred;

  public:
    explicit __reorder_pred(_Pred __pred) : _M_pred(__pred) {}

    template <typename _FTp, typename _STp>
    bool
    operator()(_FTp&& __a, _STp&& __b) const
    {
        return _M_pred(::std::forward<_STp>(__b), ::std::forward<_FTp>(__a));
    }
};
//! Like a polymorphic lambda for pred(...,value)
template <typename _Tp, typename _Predicate>
class __equal_value_by_pred
{
    const _Tp& _M_value;
    _Predicate _M_pred;

  public:
    __equal_value_by_pred(const _Tp& __value, _Predicate __pred) : _M_value(__value), _M_pred(__pred) {}

    template <typename _Arg>
    bool
    operator()(_Arg&& __arg) const
    {
        return _M_pred(::std::forward<_Arg>(__arg), _M_value);
    }
};

//! Like a polymorphic lambda for ==value
template <typename _Tp>
class __equal_value
{
    const _Tp& _M_value;

  public:
    explicit __equal_value(const _Tp& __value) : _M_value(__value) {}

    template <typename _Arg>
    bool
    operator()(_Arg&& __arg) const
    {
        return ::std::forward<_Arg>(__arg) == _M_value;
    }
};

//! Logical negation of ==value
template <typename _Tp>
class __not_equal_value
{
    const _Tp& _M_value;

  public:
    explicit __not_equal_value(const _Tp& __value) : _M_value(__value) {}

    template <typename _Arg>
    bool
    operator()(_Arg&& __arg) const
    {
        return !(::std::forward<_Arg>(__arg) == _M_value);
    }
};

template <typename _ForwardIterator, typename _Compare, typename _CompareIt>
_ForwardIterator
__cmp_iterators_by_values(_ForwardIterator __a, _ForwardIterator __b, _Compare __comp, _CompareIt __comp_it)
{
    if (__comp_it(__a, __b))
    { // we should return closer iterator
        return __comp(*__b, *__a) ? __b : __a;
    }
    else
    {
        return __comp(*__a, *__b) ? __a : __b;
    }
}

} // namespace __internal
} // namespace __pstl

_PSTL_HIDE_FROM_ABI_POP

#endif /* _PSTL_UTILS_H */
