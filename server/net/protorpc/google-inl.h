#include <google/protobuf/message.h>

inline void ByteSizeConsistencyError(int byte_size_before_serialization,
                                     int byte_size_after_serialization,
                                     int bytes_produced_by_serialization)
{
    GOOGLE_CHECK_EQ(byte_size_before_serialization, byte_size_after_serialization)
        << "Protocol message was modified concurrently during serialization.";
    GOOGLE_CHECK_EQ(bytes_produced_by_serialization, byte_size_before_serialization)
        << "Byte size calculation and serialization were inconsistent.  This "
           "may indicate a bug in protocol buffers or it may be caused by "
           "concurrent modification of the message.";
    GOOGLE_LOG(FATAL) << "This shouldn't be called if all the sizes are equal.";
}

inline std::string InitializationErrorMessage(const char* action,
                                              const google::protobuf::MessageLite& message)
{

    std::string result;
    result += "Can't ";
    result += action;
    result += " message of type \"";
    result += message.GetTypeName();
    result += "\" because it is missing required fields: ";
    result += message.InitializationErrorString();
    return result;
}