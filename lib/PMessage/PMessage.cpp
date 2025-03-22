#include "PMessage.h"

#include <pb_encode.h>
#include <pb_decode.h>


PMessage::PMessage(const char* name) : PObject(name)
{
}
