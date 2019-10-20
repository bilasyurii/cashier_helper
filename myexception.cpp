#include "myexception.h"

CMyException::CMyException(const char *pcMessage) : m_pcMessage(pcMessage) {}

const char *CMyException::what() const throw() {
    return m_pcMessage;
}
