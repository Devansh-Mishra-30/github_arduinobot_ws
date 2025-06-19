// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from arduinobot_msgs:srv/AddTwoints.idl
// generated code does not contain a copyright notice

#ifndef ARDUINOBOT_MSGS__SRV__DETAIL__ADD_TWOINTS__STRUCT_H_
#define ARDUINOBOT_MSGS__SRV__DETAIL__ADD_TWOINTS__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in srv/AddTwoints in the package arduinobot_msgs.
typedef struct arduinobot_msgs__srv__AddTwoints_Request
{
  int64_t a;
  int64_t b;
} arduinobot_msgs__srv__AddTwoints_Request;

// Struct for a sequence of arduinobot_msgs__srv__AddTwoints_Request.
typedef struct arduinobot_msgs__srv__AddTwoints_Request__Sequence
{
  arduinobot_msgs__srv__AddTwoints_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} arduinobot_msgs__srv__AddTwoints_Request__Sequence;


// Constants defined in the message

/// Struct defined in srv/AddTwoints in the package arduinobot_msgs.
typedef struct arduinobot_msgs__srv__AddTwoints_Response
{
  int64_t sum;
} arduinobot_msgs__srv__AddTwoints_Response;

// Struct for a sequence of arduinobot_msgs__srv__AddTwoints_Response.
typedef struct arduinobot_msgs__srv__AddTwoints_Response__Sequence
{
  arduinobot_msgs__srv__AddTwoints_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} arduinobot_msgs__srv__AddTwoints_Response__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ARDUINOBOT_MSGS__SRV__DETAIL__ADD_TWOINTS__STRUCT_H_
