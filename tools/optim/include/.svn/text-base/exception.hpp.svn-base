#ifndef CMAES_EXCEPTION_H
#define CMAES_EXCEPTION_H

#include <string>
#include <exception>



namespace cmaes {
  class Exception : public std::exception {
  protected:
    std::string mMessage;

  public:
    Exception();

    Exception(const std::string& inMessage);

    virtual ~Exception() throw();

    virtual const char* what() const throw();

    inline std::string getMessage() const throw() {
      return mMessage;
    }

    inline void setMessage(const std::string& inMessage) {
      mMessage = inMessage;
    }
  }
  ; // class Exception
} // namespace cmaes



#endif /* CMAES_EXCEPTION_H */
