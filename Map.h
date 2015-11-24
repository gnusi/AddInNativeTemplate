#ifndef __MAP__H__
#define __MAP__H__

#include <vector>
#include <unordered_map>

template<typename T>
class Map {
public:
  typedef long id_type;
  static const id_type INVALID_ID = -1;

  size_t size() const {
    assert(name_to_id_.size() == values_.size());
    return values_.size();
  }

  id_type find_id(const WCHAR_T* name) const {
    auto it = name_to_id_.find(name);
    return name_to_id_.end() == it ? INVALID_ID : it->second;
  }

  const T& at(id_type id) const {
    return values_[id];
  }

  T& insert(const std::wstring& name) {
    values_.reserve(values_.size() + 1);
    name_to_id_.reserve(values_.capacity());

    /* noexcept */
    name_to_id_.emplace(name, id_type(values_.size()));
    values_.emplace_back(name);
    return values_.back();
  }

private:
  std::unordered_map<std::wstring, id_type> name_to_id_;
  std::vector<T> values_;
}; // Map

#endif // __MAP__H__