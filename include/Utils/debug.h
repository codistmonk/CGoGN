#ifndef DEBUG_H
#define DEBUG_H

#include <sstream>

#ifndef NDEBUG
#define DEBUG_OUT std::cout << "(" __FILE__ << ':' << __LINE__ << ") "
#define DEBUG_GL { GLenum error = glGetError(); while (error) { DEBUG_OUT << gluErrorString(error) << std::endl; error = glGetError(); } }
#else
namespace Debug
{

static std::stringstream ignore;

} // namespace Debug

#define DEBUG_OUT ::Debug::ignore
#define DEBUG_GL
#endif

#endif // DEBUG_H
