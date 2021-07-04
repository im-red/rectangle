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

#include "typeinfo.h"

#include <assert.h>

#include <map>

using namespace std;

namespace rectangle {
namespace backend {

TypeInfo::TypeInfo(TypeInfo::Category cat) : m_category(cat) {}

TypeInfo::~TypeInfo() {}

std::string TypeInfo::toString() const {
  static const map<Category, string> MAP = {
      {Category::Int, "int"},     {Category::Void, "void"},
      {Category::Float, "float"}, {Category::String, "string"},
      {Category::List, "list"},   {Category::Custom, "custom"}};

  auto iter = MAP.find(category());
  assert(iter != MAP.end());

  return iter->second;
}

TypeInfo::Category TypeInfo::category() const { return m_category; }

bool TypeInfo::assignCompatible(const std::shared_ptr<TypeInfo> &rhs) const {
  if (operator==(*rhs)) {
    return true;
  }
  if (rhs->m_category == TypeInfo::Category::Void) {
    return true;
  }
  return false;
}

ListTypeInfo::ListTypeInfo(const std::shared_ptr<TypeInfo> &ele)
    : TypeInfo(Category::List), m_elementType(ele) {}

string ListTypeInfo::toString() const {
  const string elementTypeString = m_elementType->toString();
  const string result = "list<" + elementTypeString + ">";
  return result;
}

bool ListTypeInfo::assignCompatible(
    const std::shared_ptr<TypeInfo> &rhs) const {
  if (rhs->category() != Category::List) {
    return false;
  }

  ListTypeInfo *lti = dynamic_cast<ListTypeInfo *>(rhs.get());
  assert(lti != nullptr);
  return elementType()->assignCompatible(lti->elementType());
}

CustomTypeInfo::CustomTypeInfo(const string &name)
    : TypeInfo(Category::Custom), m_name(name) {}

string CustomTypeInfo::toString() const { return m_name; }

}  // namespace backend
}  // namespace rectangle
