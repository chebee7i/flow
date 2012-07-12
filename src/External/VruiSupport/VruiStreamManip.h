/*******************************************************************************
 VruiStreamManip: Classes for restricting output by render node.
 Copyright (c) 2006-2008 Jordan Van Aalsburg
 *******************************************************************************/
 
#ifndef VRUISTREAMMANIP_H
#define VRUISTREAMMANIP_H
 
#include <iostream>
#include <string>
#include <sstream>
//#include <IO/ansi-color.h>

#include <Vrui/Vrui.h>

template<typename T>
std::string toString(const T& any)
{
   std::stringstream ss;
   ss << any;
   return ss.str();
}

namespace node
{
class filter
{
   public:
      filter(std::ostream& os) :
         stream(os)
      {
         label="[" + toString(Vrui::getNodeIndex()) + "] ";
      }

      ~filter()
      {
      }

      std::ostream& operator()(void)
      {
#if 0
         if (Vrui::isMaster())
         return stream << ansi::red() << label << ansi::normal;
         return stream << label;
#endif

         return stream << label;
      }

   protected:
      std::ostream& stream;
      std::string label;
};
}

namespace master
{
class filter
{
   public:
      filter(std::ostream& os) :
         stream(os), null(NULL)
      {
      }

      ~filter()
      {
      }

      std::ostream& operator()(void)
      {
         if (Vrui::isMaster())
            return stream;
         return null;
      }

   protected:
      std::ostream& stream;
      std::ostream null;
};
}

namespace debug
{
class filter
{
   public:
      filter(std::ostream& os) :
         stream(os), null(NULL)
      {
      }

      ~filter()
      {
      }

      std::ostream& operator()(void)
      {
         if (_debug)
            return stream;
         return null;
      }

      static void enable()
      {
         _debug=true;
      }
      static void disable()
      {
         _debug=false;
      }

      static bool isDebugEnabled()
      {
         return _debug;
      }

   protected:
      std::ostream& stream;
      std::ostream null;

      static bool _debug;
};
}

#endif
