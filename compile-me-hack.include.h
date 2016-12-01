/*     THIS FILE SHOULD NOT BE INCLUDED AS PART OF A RELEASE OR SUBMISSION.
       IF USED, IT WILL CAUSE PROBLEMS WHEN THE NEEDED CODE IS AVAILABLE.     */

/* -- What does it do?
 *   Allows this program to compile and run in order to help us understand it.
 *   After following the instructions below, calls to Output() will either
 *    -- fail to compile with an inscrutable error message, or
 *    -- print a string to the log streams passed to it, along with a tag
 *       identifying why the output showed up.
 *
 * -- What is the signature:  void(string, output stream, log stream).
 *
 * -- How do you use this?
 * Add the text, verbatim:
      -include compile-me-hack.include.h
 * at the end of the very first line of 'makefile', such that it looks like
      GPP = g++ -O3 -Wall -std=c++11 -include compile-me-hack.include.h
 * then run make; the code should compile, with plenty of warnings.
 */

# if ! defined CSCE_240_PROJECT_7_HEADER_COMPILE_ME_HACK_H
# define CSCE_240_PROJECT_7_HEADER_COMPILE_ME_HACK_H
////////////////////////////////////////////////////////////////////////////////
// TODO:  FIXME:
// A nasty and error-prone hack to get this program to compile, silently, w/o
// the definition of Utils::Output.
int __attribute__ ((weak)) nil() { return (int)0; }
int __attribute__ ((weak)) extern ignore;
/* Because the macro is is intended to be substituted as only part of an entire
   statement, it's not possible to resolve the name or suppress warnings for just
   the macro.  I cannot insert _Pragma() in the middle of the statement because
   there is no way to enter blank lines before the generated code. */
#   if defined __GNUC__
#
#   pragma message "-Wunused-value off: compile-me-hack.include.h included"
#   pragma GCC diagnostic ignored "-Wunused-value"
#   pragma GCC diagnostic push
#
#   define Output(str, out, log)                                    \
    log_stream; ignore = ::nil();                                   \
      do {                                                          \
        log << "Utils::output macro hack called: " << str << "\n";  \
      } while (false)
#
#   endif
////////////////////////////////////////////////////////////////////////////////
# endif
