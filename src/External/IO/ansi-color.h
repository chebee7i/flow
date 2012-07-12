/*******************************************************************************
 ansi-color: Simple classes for printing color text in a terminal.
 Copyright (c) 2006-2008 Jordan Van Aalsburg
 *******************************************************************************/
#ifndef   	ANSI_COLOR_H_
#define   	ANSI_COLOR_H_

#include <iostream>

namespace ansi
{

/** Foreground and background colors.
 */
enum Color
{
   BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE, DEFAULT
};

/** Font style options.
 */
enum Style
{
   NORMAL, BOLD, UNDERLINE, BLINK, INVERSE, STRIKE
};

/** Base ANSI color class.
 *
 *  The color class provides the ability to change the color and
 *  style of the text provided that the terminal supports this
 *  behavior. The user may specify both a foreground and background
 *  color as well as a style. See the related enums for possible
 *  options.
 *
 *  Typically the user will utilize the common derived color classes
 *  which are definded below. However, in some situations it may be
 *  desirable to use the color class directly. In these situations
 *  the general usage is as follows:
 *  \code
 *     ansi::color custom;
 *     custom.setForegroundColor(ansi::WHITE);
 *     custom.setBackgroundColor(ansi::GREEN);
 *     custom.setStyle(ansi::BOLD);
 *
 *     std::cout << custom << "CUSTOM COLOR" << ansi::normal << std::endl;
 *  \endcode
 */
class color
{
   public:
      color()
      {
         setBackgroundColor(DEFAULT);
         setForegroundColor(DEFAULT);
         setStyle(NORMAL);
      }

      std::ostream& operator()(std::ostream& os) const
      {
         os << "\033[" << style << ";" << foreground << ";" << background
               << "m";
         return os;
      }

      void setStyle(Style property)
      {
         if (property == NORMAL)
            style=0;
         else if (property == BOLD)
            style=1;
         else if (property == UNDERLINE)
            style=4;
         else if (property == BLINK)
            style=5;
         else if (property == INVERSE)
            style=7;
         else if (property == STRIKE)
            style=9;
      }

      void setForegroundColor(Color type)
      {
         if (type == BLACK)
            foreground=30;
         else if (type == RED)
            foreground=31;
         else if (type == GREEN)
            foreground=32;
         else if (type == YELLOW)
            foreground=33;
         else if (type == BLUE)
            foreground=34;
         else if (type == MAGENTA)
            foreground=35;
         else if (type == CYAN)
            foreground=36;
         else if (type == WHITE)
            foreground=37;
         else if (type == DEFAULT)
            foreground=39;
      }

      void setBackgroundColor(Color type)
      {
         if (type == BLACK)
            background=40;
         else if (type == RED)
            background=41;
         else if (type == GREEN)
            background=42;
         else if (type == YELLOW)
            background=43;
         else if (type == BLUE)
            background=44;
         else if (type == MAGENTA)
            background=45;
         else if (type == CYAN)
            background=46;
         else if (type == WHITE)
            background=47;
         else if (type == DEFAULT)
            background=49;
      }

   private:
      short style;
      short foreground;
      short background;
};

/** ostream operator.
 */
std::ostream& operator<<(std::ostream& os, const color& c)
{
   return c(os);
}

/** ANSI closure method.
 *
 *  The normal method must be called to terminate the ansi::color
 *  session and return the terminal to its original state.
 *  For example
 *  \code
 *    std::cout << ansi::red() << "ERROR" << ansi::endl;
 *  \endcode
 */
std::ostream& normal(std::ostream& os)
{
   os << "\033[0m";
   return os;
}

std::ostream& endl(std::ostream& os)
{
   os << ansi::normal << std::endl;
   return os;
}

//
// Common color classes are defined below for convienence.
//

class red: public color
{
   public:
      red(Style property=NORMAL)
      {
         setStyle(property);
         setForegroundColor(RED);
      }
};

class green: public color
{
   public:
      green(Style property=NORMAL)
      {
         setStyle(property);
         setForegroundColor(GREEN);
      }
};

class yellow: public color
{
   public:
      yellow(Style property=NORMAL)
      {
         setStyle(property);
         setForegroundColor(YELLOW);
      }
};

class blue: public color
{
   public:
      blue(Style property=NORMAL)
      {
         setStyle(property);
         setForegroundColor(BLUE);
      }
};

class magenta: public color
{
   public:
      magenta(Style property=NORMAL)
      {
         setStyle(property);
         setForegroundColor(MAGENTA);
      }
};

class cyan: public color
{
   public:
      cyan(Style property=NORMAL)
      {
         setStyle(property);
         setForegroundColor(CYAN);
      }
};

class white: public color
{
   public:
      white(Style property=NORMAL)
      {
         setStyle(property);
         setForegroundColor(WHITE);
      }
};

//
// Useful status states
//

std::ostream& STATUS_OK(std::ostream& os)
{
   os << ansi::green(ansi::BOLD) << "OK" << ansi::normal;
   return os;
}

std::ostream& STATUS_WARN(std::ostream& os)
{
   os << ansi::yellow(ansi::BOLD) << "WARN" << ansi::normal;
   return os;
}

std::ostream& STATUS_FAIL(std::ostream& os)
{
   os << ansi::red(ansi::BOLD) << "FAIL" << ansi::normal;
   return os;
}

// I'm not sure which format I like more.

namespace status
{
std::ostream& OK(std::ostream& os)
{
   os << ansi::green(ansi::BOLD) << "OK" << ansi::normal;
   return os;
}

std::ostream& WARN(std::ostream& os)
{
   os << ansi::yellow(ansi::BOLD) << "WARN" << ansi::normal;
   return os;
}

std::ostream& FAIL(std::ostream& os)
{
   os << ansi::red(ansi::BOLD) << "FAIL" << ansi::normal;
   return os;
}
} // namespace::status


} // namespace::ansi


#endif 	    /* !ANSI-COLOR_H_ */
