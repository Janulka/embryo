/*
 *  libembryo
 *  Copyright (C) 2008 by Alexandre Devert
 *
 *           DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *                  Version 2, December 2004
 *
 *  Copyright (C) 2004 Sam Hocevar
 *  14 rue de Plaisance, 75014 Paris, France
 *  Everyone is permitted to copy and distribute verbatim or modified
 *  copies of this license document, and changing it is allowed as long
 *  as the name is changed.
 *
 *            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *  TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
 *
 *  0. You just DO WHAT THE FUCK YOU WANT TO.
 */

#ifndef EMBRYO_HANDLE_T_H
#define EMBRYO_HANDLE_T_H

#include <functional>



namespace embryo {
  /*
   * Intrusive smart-pointer, taken form the Boost library
   *    - inc_ref_count(T) should be defined : increases the reference counter
   *    - dec_ref_count(T) should be defined : decreases the reference counter
   */

  template <class T>
  class HandleT {
  private:
    T* mPtr;

  public:
    inline HandleT() : mPtr(0) { }

    inline HandleT(T* inPtr, bool inAddRef = true) : mPtr(inPtr) {
      if (mPtr != 0 && inAddRef)
        inc_ref_count(mPtr);
    }

    inline HandleT(HandleT<T> const & inHandleT) : mPtr(inHandleT.mPtr) {
      if (mPtr != 0)
        inc_ref_count(mPtr);
    }

    inline ~HandleT() {
      if (mPtr != 0)
        dec_ref_count(mPtr);
    }

    HandleT& operator = (HandleT const & inHandleT) {
      HandleT(inHandleT).swap(*this);
      return *this;
    }

    HandleT& operator = (T* inPtr) {
      HandleT(inPtr).swap(*this);
      return *this;
    }

    inline T& operator * () const {
      return *mPtr;
    }

    inline T* operator -> ( ) const {
      return mPtr;
    }

    inline T* get() const {
      return mPtr;
    }

    inline bool operator ! () const {
      return !mPtr;
    }

    inline bool operator != (const T* inPtr) const {
      return mPtr != inPtr;
    }

    inline bool operator == (const T* inPtr) const {
      return mPtr == inPtr;
    }

    inline bool operator < (const T* inPtr) const {
      return std::less<T*>()(mPtr, inPtr);
    }

    inline void swap(HandleT<T>& inHandleT) {
      T* lTmp = mPtr;
      mPtr = inHandleT.mPtr;
      inHandleT.mPtr = lTmp;
    }
  }
  ; // class HandleT



  template<class T, class U>
  inline bool operator == (HandleT<T> const & a, HandleT<U> const & b) {
    return a.get() == b.get();
  }

  template<class T, class U>
  inline bool operator != (HandleT<T> const & a, HandleT<U> const & b) {
    return a.get() != b.get();
  }

  template<class T, class U>
  inline bool operator == (HandleT<T> const & a, U* b) {
    return a.get() == b;
  }

  template<class T, class U>
  inline bool operator != (HandleT<T> const & a, U* b) {
    return a.get() != b;
  }

  template<class T, class U>
  inline bool operator == (T* a, HandleT<U> const & b) {
    return a == b.get();
  }

  template<class T, class U>
  inline bool operator != (T* a, HandleT<U> const & b) {
    return a != b.get();
  }

  template<class T>
  inline bool operator < (HandleT<T> const & a, HandleT<T> const & b) {
    return std::less<T*>()(a.get(), b.get());
  }

  template<class T>
  void swap(HandleT<T>& a, HandleT<T> & b) {
    a.swap(b);
  }

  template<class T>
  T* get_pointer(HandleT<T> const & a) {
    return a.get();
  }

  template<class T, class U>
  HandleT<T> static_pointer_cast(HandleT<U> const & p) {
    return static_cast<T*>(p.get());
  }

  template<class T, class U>
  HandleT<T> const_pointer_cast(HandleT<U> const & p) {
    return const_cast<T*>(p.get());
  }

  template<class T, class U>
  HandleT<T> dynamic_pointer_cast(HandleT<U> const & p) {
    return dynamic_cast<T*>(p.get());
  }
} // namespace embryo



#endif /* EMBRYO_HANDLE_T_H */
