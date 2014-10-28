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

#ifndef EMBRYO_EXCEPTION_H
#define EMBRYO_EXCEPTION_H

#include <string>
#include <exception>



namespace embryo {
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
} // namespace embryo



#endif /* EMBRYO_EXCEPTION_H */
