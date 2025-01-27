#pragma once

#include <string>
#include <map>

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include "system_properties/include/api/_system_properties.h"

struct prop_cb {
    virtual void exec(const char *name, const char *value) = 0;
};

static inline bool str_starts(std::string_view s, std::string_view ss) {
    return s.size() >= ss.size() && s.compare(0, ss.size(), ss) == 0;
}


using prop_list = std::map<std::string, std::string>;

struct prop_collector : prop_cb {
    explicit prop_collector(prop_list &list) : list(list) {}
    void exec(const char *name, const char *value) override {
        list.insert({name, value});
    }
private:
    prop_list &list;
};

// System properties
std::string get_prop(const char *name, bool persist = false);
int delete_prop(const char *name, bool persist = false);
int set_prop(const char *name, const char *value, bool skip_svc = false);
//void load_prop_file(const char *filename, bool skip_svc = false);

//void persist_get_prop(const char *name, prop_cb *prop_cb);
//void persist_get_props(prop_cb *prop_cb);
//bool persist_delete_prop(const char *name);
//bool persist_set_prop(const char *name, const char *value);
