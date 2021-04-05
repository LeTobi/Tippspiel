#include "msgID.h"

MsgID::MsgID(MsgType t, unsigned int a0)
{
    type = t;
    arg0 = a0;
}

MsgID::MsgID(MsgType t)
{
    type = t;
}

bool MsgID::operator==(const MsgID& other) const {
    return type==other.type && arg0==other.arg0;
}

bool MsgID::operator!=(const MsgID& other) const {
    return !(*this==other);
}

bool MsgID::operator<(const MsgID& other) const {
    if (type==other.type)
        return arg0<other.arg0;
    return type<other.type;
}