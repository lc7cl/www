#ifndef ACLWRAP_H_
#define ACLWRAP_H_

class AclWrap {
public:
    static void Initialize(v8::Handle<v8::Object> target);
    AclWrap* GetWrapped() { return p_; };

private:
    AclWrap *p_;

};

#endif
