// see License.txt for copyright and terms of use

#include "cc_type.h"            // SPECIAL: oink_type.h is INCLUDED into cc_type.h
#include "value.h"              // Value

int TypeLike::isActuallyAValue() const {
  switch(identifyByTag()) {
  default:
    xfailure("something is wrong");
    break;

  case Type::T_ATOMIC:
  case Type::T_POINTER:
  case Type::T_REFERENCE:
  case Type::T_FUNCTION:
  case Type::T_ARRAY:
  case Type::T_POINTERTOMEMBER:
    return false;               // is a real type

  case Value::V_ATOMIC:
  case Value::V_POINTER:
  case Value::V_REFERENCE:
  case Value::V_FUNCTION:
  case Value::V_ARRAY:
  case Value::V_POINTERTOMEMBER:
    return true;               // is a value
  }
}
