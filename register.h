#ifndef __REGISTER__H__
#define __REGISTER__H__ 

#include <unordered_map>

template< typename T >
class singleton {
public:
  static T& instance() {
    static T inst;
    return inst;
  }

protected:
  singleton() = default;

private:
  singleton(const singleton&) = delete;
  singleton& operator=(const singleton&) = delete;
};

/* Generic class representing globally-stored correspondence
 * between object of KeyType and EntryType */
template< typename KeyType, typename EntryType, typename RegisterType >
class generic_register : public singleton < RegisterType > {
 public:
  typedef KeyType key_type;
  typedef EntryType entry_type;

  virtual ~generic_register() { }

  void set(const key_type& key, const entry_type& entry) {
    reg_map_.emplace(key, entry);
  }

  entry_type get(const key_type& key) const {
    const entry_type* entry = lookup(key);
    return entry ? *entry : entry_type();
  }

 protected:
  virtual const entry_type* lookup(const key_type& key) const {
    typename register_map_t::const_iterator it = reg_map_.find(key);
    return reg_map_.end() == it ? nullptr : &it->second;
  }

 private:
  typedef std::unordered_map< key_type, entry_type > register_map_t;

  register_map_t reg_map_;
};

/* Generic registerer
 *
 * The simplest way to register the particular type
 * is to subclass generic_registerer */
template< typename RegisterType >
struct generic_registerer {
  typedef typename RegisterType::key_type key_type;
  typedef typename RegisterType::entry_type entry_type;

  generic_registerer( key_type key, entry_type entry ) {
    RegisterType::instance().set( key, entry );
  }
};

#endif