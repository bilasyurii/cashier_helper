#ifndef CODENUMBER_H
#define CODENUMBER_H


class CCodeNumber {
public:
    enum TYPE {
        L, R, G, WRONG
    };

    CCodeNumber();
    CCodeNumber(unsigned n, TYPE type = L);
    CCodeNumber(bool *bits);

    TYPE getType() const;
    operator unsigned() const;
private:
    TYPE m_type;
    unsigned m_uNum;
};

#endif // CODENUMBER_H
