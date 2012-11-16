#ifndef DEBUG_H
#define DEBUG_H

#ifndef NDEBUG
#define DEBUG_OUT std::cout << "(" __FILE__ << ':' << __LINE__ << ") "
#define DEBUG_GL { GLenum error = glGetError(); while (error) { DEBUG_OUT << gluErrorString(error) << std::endl; error = glGetError(); } }
#else
#define DEBUG_OUT //
#define DEBUG_GL
#endif

#endif // DEBUG_H
