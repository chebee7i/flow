/*******************************************************************************
 Directory: Reads and stores the contents of a directory.
 Copyright (c) 2006-2008 Jordan Van Aalsburg
 *******************************************************************************/
#ifndef DIRECTORY_READER_H
#define DIRECTORY_READER_H

#include <errno.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <set>
#include <iterator>
#include <iostream>

class Directory
{
   public:
      Directory()
      {
      }

      Directory(const std::string& dir)
      {
         read(dir);
      }

      void addExtensionFilter(std::string ext)
      {
         filters.insert(ext);
      }
      void read(const std::string& dir=".")
      {
         DIR *pdir;
         struct dirent *pent;

         pdir=opendir(dir.c_str());
         if (!pdir)
         {
            std::cerr << "opendir() failure. terminating." << std::endl;
            exit(1);
         }
         errno=0;
         while ((pent=readdir(pdir)))
         {
            std::string file=pent->d_name;

            if (filters.size() == 0)
               dirlist.push_back(file);
            else
            {
               for (f_itr filter=filters.begin(); filter != filters.end(); ++filter)
               {
                  unsigned int ext_size=(*filter).size();

                  if (file.size() < ext_size)
                     continue;

                  std::string file_ext=
                        file.substr(file.size() - ext_size, ext_size);

                  if (*filter == file_ext)
                     dirlist.push_back(file);
               }
            }
         }
         if (errno)
            std::cerr << "readdir() failure. terminating." << std::endl;

         closedir(pdir);
      }
      const std::vector<std::string>& contents() const
      {
         return dirlist;
      }
      void printDirectory() const
      {
         std::vector<std::string>::const_iterator file;
         for (file=dirlist.begin(); file != dirlist.end(); ++file)
            std::cout << *file << std::endl;
      }

   protected:
      std::vector<std::string> dirlist;
      std::set<std::string> filters;

      typedef std::set<std::string>::const_iterator f_itr;
};

#endif
