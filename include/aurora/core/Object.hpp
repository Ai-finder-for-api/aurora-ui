#pragma once
// Aurora UI - Object tree with parent-owned lifetime (QObject analogue)
#include <algorithm>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "Signal.hpp"

namespace aui {

class Object : public std::enable_shared_from_this<Object> {
public:
    explicit Object(Object* parent = nullptr) { setParent(parent); }
    virtual ~Object() {
        destroyed.emit(this);
        if (parent_) parent_->removeChild(this);
        for (auto* c : children_) c->parent_ = nullptr;
    }
    Object(const Object&) = delete;
    Object& operator=(const Object&) = delete;

    void setParent(Object* p) {
        if (parent_ == p) return;
        if (parent_) parent_->removeChild(this);
        parent_ = p;
        if (p) p->children_.push_back(this);
    }
    Object* parent() const { return parent_; }
    const std::vector<Object*>& children() const { return children_; }

    template <typename T>
    T* findChild(std::string_view name = {}) const {
        for (auto* c : children_) {
            if (auto* t = dynamic_cast<T*>(c))
                if (name.empty() || t->objectName() == name) return t;
            if (auto* t = c->findChild<T>(name)) return t;
        }
        return nullptr;
    }

    void setObjectName(std::string n) { name_ = std::move(n); }
    const std::string& objectName() const { return name_; }

    Signal<Object*> destroyed;

private:
    void removeChild(Object* c) {
        std::erase(children_, c);
    }
    Object* parent_ = nullptr;
    std::vector<Object*> children_;
    std::string name_;
};

} // namespace aui
