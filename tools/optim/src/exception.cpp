#include "exception.h"

using namespace cmaes;



Exception::Exception() { }



Exception::Exception(const std::string& inMessage) : mMessage(inMessage) { }



Exception::~Exception() throw() { }



const char*
Exception::what() const throw() {
	return mMessage.c_str();
}
