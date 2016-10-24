#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

// Conversion from Euler angles (in radians) to Quaternion
vec3 EulerAngles(90, 45, 0);
MyQuaternion = quat(EulerAngles);

// Conversion from axis-angle
// In GLM the angle must be in degrees here, so convert it.
MyQuaternion = gtx::quaternion::angleAxis(degrees(RotationAngle), RotationAxis);

mat4 RotationMatrix = quaternion::toMat4(quaternion);
mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScaleMatrix;
// You can now use ModelMatrix to build the MVP matrix

glm::mat4 QuatRotate(RotationAngle,RotationAxis)
{
  MyQuaternion = gtx::quaternion::angleAxis(degrees(RotationAngle), RotationAxis);
  return quaternion::toMat4(MyQuaternion);
}
