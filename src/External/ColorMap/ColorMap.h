/*******************************************************************************
 ColorMap: Simple color map classes.
 Copyright (c) 2006-2008 Jordan Van Aalsburg
 *******************************************************************************/
#ifndef COLOR_MAP_H
#define COLOR_MAP_H

class ColorMap
{
   public:
      ColorMap()
      {
      }
      virtual ~ColorMap()
      {
      }

      const float* getColor(int index) const
      {
         return colorMap[index];
      }

   protected:
      float colorMap[256][3];
      virtual void initializeColorMap() = 0;
};

class BlueRedColorMap: public ColorMap
{
   public:
      BlueRedColorMap()
      {
         initializeColorMap();
      }
      virtual ~BlueRedColorMap()
      {
      }

      void initializeColorMap()
      {
         {
            for (int i=0; i < 32; i++)
            {
               colorMap[i][0]=0.0;
               colorMap[i][1]=0.0;
               colorMap[i][2]=0.5 + i * 0.5 / 32.0;
            }
            for (int i=32; i < 96; i++)
            {
               colorMap[i][0]=0.0;
               colorMap[i][1]=(i - 32) / 64.0;
               colorMap[i][2]=1.0;
            }
            for (int i=96; i < 160; i++)
            {
               colorMap[i][0]=(i - 96) / 64.0;
               colorMap[i][1]=1.0;
               colorMap[i][2]=1.0 - (i - 96) / 64.0;
            }
            for (int i=160; i < 224; i++)
            {
               colorMap[i][0]=1.0;
               colorMap[i][1]=1.0 - (i - 160) * 1.0 / 64.0;
               colorMap[i][2]=0.0;
            }
            for (int i=224; i < 256; i++)
            {
               colorMap[i][0]=1.0 - (i - 224) * 0.5 / 32.0;
               colorMap[i][1]=0.0;
               colorMap[i][2]=0.0;
            }
         }
      }
};

#endif
