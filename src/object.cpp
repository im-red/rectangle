/*********************************************************************************
 * Copyright (C) 2020  Jia Lihong
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 ********************************************************************************/

#include "object.h"

using namespace std;

namespace rectangle {
namespace runtime {

Object::~Object() {}

int Object::intData() const {
  assert(m_category == Category::Int);
  return m_intData;
}

void Object::setIntData(int intData) {
  assert(m_category == Category::Int);
  m_intData = intData;
}

float Object::floatData() const {
  assert(m_category == Category::Float);
  return m_floatData;
}

void Object::setFloatData(float floatData) {
  assert(m_category == Category::Float);
  m_floatData = floatData;
}

std::string Object::stringData() const {
  assert(m_category == Category::String);
  return m_stringData;
}

void Object::setStringData(const std::string &stringData) {
  assert(m_category == Category::String);
  m_stringData = stringData;
}

Object::Category Object::category() const { return m_category; }

void Object::setCategory(const Category &category) { m_category = category; }

std::string Object::toString() const {
  std::string result;
  switch (m_category) {
    case Category::Int: {
      result = "Int(" + to_string(m_intData) + ")";
      break;
    }
    case Category::Float: {
      result = "Float(" + to_string(m_floatData) + ")";
      break;
    }
    case Category::String: {
      result = "String(\"" + m_stringData + "\")";
      break;
    }
    case Category::List: {
      result = "List(";
      if (m_vData.size() == 0) {
        result += ")";
      } else if (m_vData.size() == 1) {
        result += m_vData[0].toString() + ")";
      } else {
        result += m_vData[0].toString();
        for (size_t i = 1; i < m_vData.size(); i++) {
          result += ", " + m_vData[i].toString();
        }
        result += ")";
      }
      break;
    }
    case Category::Struct: {
      result = "Struct(";
      if (m_vData.size() == 0) {
        result += ")";
      } else if (m_vData.size() == 1) {
        result += m_vData[0].toString() + ")";
      } else {
        result += m_vData[0].toString();
        for (size_t i = 1; i < m_vData.size(); i++) {
          result += ", " + m_vData[i].toString();
        }
        result += ")";
      }
      break;
    }
    default: {
      assert(false);
    }
  }
  return result;
}

}  // namespace runtime
}  // namespace rectangle

using namespace rectangle::runtime;

Object operator+(const Object &lhs, const Object &rhs) {
  assert(lhs.category() == rhs.category());

  Object result;
  result.setCategory(lhs.category());
  switch (lhs.category()) {
    case Object::Category::Int: {
      result.setIntData(lhs.intData() + rhs.intData());
      break;
    }
    case Object::Category::Float: {
      result.setFloatData(lhs.floatData() + rhs.floatData());
      break;
    }
    case Object::Category::String: {
      result.setStringData(lhs.stringData() + rhs.stringData());
      break;
    }
    default: {
      assert(false);
    }
  }
  return result;
}

Object operator-(const Object &lhs, const Object &rhs) {
  assert(lhs.category() == rhs.category());

  Object result;
  result.setCategory(lhs.category());
  switch (lhs.category()) {
    case Object::Category::Int: {
      result.setIntData(lhs.intData() - rhs.intData());
      break;
    }
    case Object::Category::Float: {
      result.setFloatData(lhs.floatData() - rhs.floatData());
      break;
    }
    default: {
      assert(false);
    }
  }
  return result;
}

Object operator*(const Object &lhs, const Object &rhs) {
  assert(lhs.category() == rhs.category());

  Object result;
  result.setCategory(lhs.category());
  switch (lhs.category()) {
    case Object::Category::Int: {
      result.setIntData(lhs.intData() * rhs.intData());
      break;
    }
    case Object::Category::Float: {
      result.setFloatData(lhs.floatData() * rhs.floatData());
      break;
    }
    default: {
      assert(false);
    }
  }
  return result;
}

Object operator/(const Object &lhs, const Object &rhs) {
  assert(lhs.category() == rhs.category());

  Object result;
  result.setCategory(lhs.category());
  switch (lhs.category()) {
    case Object::Category::Int: {
      result.setIntData(lhs.intData() / rhs.intData());
      break;
    }
    case Object::Category::Float: {
      result.setFloatData(lhs.floatData() / rhs.floatData());
      break;
    }
    default: {
      assert(false);
    }
  }
  return result;
}

Object operator%(const Object &lhs, const Object &rhs) {
  assert(lhs.category() == rhs.category());

  Object result;
  result.setCategory(lhs.category());
  switch (lhs.category()) {
    case Object::Category::Int: {
      result.setIntData(lhs.intData() % rhs.intData());
      break;
    }
    default: {
      assert(false);
    }
  }
  return result;
}

bool operator==(const Object &lhs, const Object &rhs) {
  if (lhs.category() != rhs.category()) {
    return false;
  }
  switch (lhs.category()) {
    case Object::Category::Int:
      return lhs.intData() == rhs.intData();
    case Object::Category::Float:
      return lhs.floatData() == rhs.floatData();
    case Object::Category::String:
      return lhs.stringData() == rhs.stringData();
    case Object::Category::Struct:
    case Object::Category::List: {
      if (lhs.elementCount() != rhs.elementCount()) {
        return false;
      }
      int len = lhs.elementCount();
      for (int i = 0; i < len; i++) {
        if (lhs.element(i) != rhs.element(i)) {
          return false;
        }
      }
      return true;
    }
    default: {
      return true;
    }
  }
}

bool operator!=(const Object &lhs, const Object &rhs) { return !(lhs == rhs); }

bool operator>(const Object &lhs, const Object &rhs) {
  assert(lhs.category() == rhs.category());

  switch (lhs.category()) {
    case Object::Category::Int: {
      return lhs.intData() > rhs.intData();
    }
    case Object::Category::Float: {
      return lhs.floatData() > rhs.floatData();
    }
    default: {
      assert(false);
    }
  }
  return false;
}

bool operator<(const Object &lhs, const Object &rhs) {
  return !(lhs == rhs || lhs > rhs);
}

bool operator>=(const Object &lhs, const Object &rhs) {
  return lhs > rhs || lhs == rhs;
}

bool operator<=(const Object &lhs, const Object &rhs) { return !(lhs > rhs); }

bool operator&&(const Object &lhs, const Object &rhs) {
  assert(lhs.category() == rhs.category());

  switch (lhs.category()) {
    case Object::Category::Int: {
      return lhs.intData() && rhs.intData();
    }
    default: {
      assert(false);
    }
  }
  return false;
}

bool operator||(const Object &lhs, const Object &rhs) {
  assert(lhs.category() == rhs.category());

  switch (lhs.category()) {
    case Object::Category::Int: {
      return lhs.intData() || rhs.intData();
    }
    default: {
      assert(false);
    }
  }
  return false;
}

ObjectPointer::ObjectPointer(Object *o, bool tmp) : m_object(o), m_tmp(tmp) {}

ObjectPointer::~ObjectPointer() {
  if (m_tmp && m_object) {
    delete m_object;
  }
}

ObjectPointer::ObjectPointer(ObjectPointer &&rhs) {
  m_object = rhs.m_object;
  rhs.m_object = nullptr;
  m_tmp = rhs.m_tmp;
}

ObjectPointer &ObjectPointer::operator=(ObjectPointer &&rhs) {
  m_object = rhs.m_object;
  rhs.m_object = nullptr;
  m_tmp = rhs.m_tmp;
  return *this;
}

Object operator!(const Object &o) {
  assert(o.category() == Object::Category::Int);

  int intData = 0;
  if (o.intData() == 0) {
    intData = 1;
  }

  return Object(intData);
}

Object operator-(const Object &o) {
  Object result;
  if (o.category() == Object::Category::Int) {
    result.setCategory(Object::Category::Int);
    result.setIntData(-o.intData());
  } else if (o.category() == Object::Category::Float) {
    result.setCategory(Object::Category::Float);
    result.setFloatData(-o.floatData());
  } else {
    assert(false);
  }

  return result;
}

Object *ObjectPointer::get() const { return m_object; }

bool ObjectPointer::isTmp() const { return m_tmp; }

ObjectPointer::operator Object() const { return *m_object; }
