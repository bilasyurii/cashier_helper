#ifndef MYEXCEPTION_H
#define MYEXCEPTION_H

#include <exception>

class CMyException : public std::exception {
private:
    const char *m_pcMessage;
public:
    CMyException(const char *pcMessage);

    virtual const char *what() const throw();
};

#endif // MYEXCEPTION_H
