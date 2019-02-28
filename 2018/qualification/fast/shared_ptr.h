
template<typename T>
class SharedPtr {
public:
  SharedPtr(T* pointer):
      pointer_(pointer) {}
  ~SharedPtr() {
    delete pointer;
  }

private:
  T* pointer_;
};
