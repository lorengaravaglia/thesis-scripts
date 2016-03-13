#!/usr/bin/env python
# Software License Agreement (BSD License)
#
# Copyright (c) 2012, Philipp Wagner
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above
#    copyright notice, this list of conditions and the following
#    disclaimer in the documentation and/or other materials provided
#    with the distribution.
#  * Neither the name of the author nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

import sys, math, Image

def Distance(p1,p2):
  dx = p2[0] - p1[0]
  dy = p2[1] - p1[1]
  return math.sqrt(dx*dx+dy*dy)

def ScaleRotateTranslate(image, angle, center = None, new_center = None, scale = None, resample=Image.BICUBIC):
  if (scale is None) and (center is None):
    return image.rotate(angle=angle, resample=resample)
  nx,ny = x,y = center
  sx=sy=1.0
  if new_center:
    (nx,ny) = new_center
  if scale:
    (sx,sy) = (scale, scale)
  cosine = math.cos(angle)
  sine = math.sin(angle)
  a = cosine/sx
  b = sine/sx
  c = x-nx*a-ny*b
  d = -sine/sy
  e = cosine/sy
  f = y-nx*d-ny*e
  return image.transform(image.size, Image.AFFINE, (a,b,c,d,e,f), resample=resample)

def CropFace(image, eye_left=(0,0), eye_right=(0,0), offset_pct=(0.2,0.2), dest_sz = (70,70)):
  # calculate offsets in original image
  offset_h = math.floor(float(offset_pct[0])*dest_sz[0])
  offset_v = math.floor(float(offset_pct[1])*dest_sz[1])
  # get the direction
  eye_direction = (eye_right[0] - eye_left[0], eye_right[1] - eye_left[1])
  # calc rotation angle in radians
  rotation = -math.atan2(float(eye_direction[1]),float(eye_direction[0]))
  # distance between them
  dist = Distance(eye_left, eye_right)
  # calculate the reference eye-width
  reference = dest_sz[0] - 2.0*offset_h
  # scale factor
  scale = float(dist)/float(reference)
  # rotate original around the left eye
  image = ScaleRotateTranslate(image, center=eye_left, angle=rotation)
  # crop the rotated image
  crop_xy = (eye_left[0] - scale*offset_h, eye_left[1] - scale*offset_v)
  crop_size = (dest_sz[0]*scale, dest_sz[1]*scale)
  image = image.crop((int(crop_xy[0]), int(crop_xy[1]), int(crop_xy[0]+crop_size[0]), int(crop_xy[1]+crop_size[1])))
  # resize it
  image = image.resize(dest_sz, Image.ANTIALIAS)
  return image

if __name__ == "__main__":
  image =  Image.open("01.jpg")
  CropFace(image, eye_left=(306,238), eye_right=(391,242), offset_pct=(0.1,0.1), dest_sz=(200,200)).save("01_10_10_200_200.jpg")
  CropFace(image, eye_left=(306,238), eye_right=(391,242), offset_pct=(0.2,0.2), dest_sz=(200,200)).save("01_20_20_200_200.jpg")
  CropFace(image, eye_left=(306,238), eye_right=(391,242), offset_pct=(0.3,0.3), dest_sz=(200,200)).save("01_30_30_200_200.jpg")
  image =  Image.open("02.jpg")
  CropFace(image, eye_left=(327,222), eye_right=(409,230), offset_pct=(0.1,0.1), dest_sz=(200,200)).save("02_10_10_200_200.jpg")
  CropFace(image, eye_left=(327,222), eye_right=(409,230), offset_pct=(0.2,0.2), dest_sz=(200,200)).save("02_20_20_200_200.jpg")
  CropFace(image, eye_left=(327,222), eye_right=(409,230), offset_pct=(0.3,0.3), dest_sz=(200,200)).save("02_30_30_200_200.jpg")
  image =  Image.open("03.jpg")
  CropFace(image, eye_left=(278,244), eye_right=(365,243), offset_pct=(0.1,0.1), dest_sz=(200,200)).save("03_10_10_200_200.jpg")
  CropFace(image, eye_left=(278,244), eye_right=(365,243), offset_pct=(0.2,0.2), dest_sz=(200,200)).save("03_20_20_200_200.jpg")
  CropFace(image, eye_left=(278,244), eye_right=(365,243), offset_pct=(0.3,0.3), dest_sz=(200,200)).save("03_30_30_200_200.jpg")
  image =  Image.open("04.jpg")
  CropFace(image, eye_left=(353,239), eye_right=(431,244), offset_pct=(0.1,0.1), dest_sz=(200,200)).save("04_10_10_200_200.jpg")
  CropFace(image, eye_left=(353,239), eye_right=(431,244), offset_pct=(0.2,0.2), dest_sz=(200,200)).save("04_20_20_200_200.jpg")
  CropFace(image, eye_left=(353,239), eye_right=(431,244), offset_pct=(0.3,0.3), dest_sz=(200,200)).save("04_30_30_200_200.jpg")
  image =  Image.open("05.jpg")
  CropFace(image, eye_left=(352,228), eye_right=(431,229), offset_pct=(0.1,0.1), dest_sz=(200,200)).save("05_10_10_200_200.jpg")
  CropFace(image, eye_left=(352,228), eye_right=(431,229), offset_pct=(0.2,0.2), dest_sz=(200,200)).save("05_20_20_200_200.jpg")
  CropFace(image, eye_left=(352,228), eye_right=(431,229), offset_pct=(0.3,0.3), dest_sz=(200,200)).save("05_30_30_200_200.jpg")
  image =  Image.open("06.jpg")
  CropFace(image, eye_left=(322,215), eye_right=(402,216), offset_pct=(0.1,0.1), dest_sz=(200,200)).save("06_10_10_200_200.jpg")
  CropFace(image, eye_left=(322,215), eye_right=(402,216), offset_pct=(0.2,0.2), dest_sz=(200,200)).save("06_20_20_200_200.jpg")
  CropFace(image, eye_left=(322,215), eye_right=(402,216), offset_pct=(0.3,0.3), dest_sz=(200,200)).save("06_30_30_200_200.jpg")
  # CropFace(image, eye_left=(252,364), eye_right=(2,366), offset_pct=(0.2,0.2)).save("arnie_20_20_70_70.jpg")
  image =  Image.open("07.jpg")
  CropFace(image, eye_left=(315,265), eye_right=(403,265), offset_pct=(0.1,0.1), dest_sz=(200,200)).save("07_10_10_200_200.jpg")
  CropFace(image, eye_left=(315,265), eye_right=(403,265), offset_pct=(0.2,0.2), dest_sz=(200,200)).save("07_20_20_200_200.jpg")
  CropFace(image, eye_left=(315,265), eye_right=(403,265), offset_pct=(0.3,0.3), dest_sz=(200,200)).save("07_30_30_200_200.jpg")
  image =  Image.open("08.jpg")
  CropFace(image, eye_left=(356,247), eye_right=(440,273), offset_pct=(0.1,0.1), dest_sz=(200,200)).save("08_10_10_200_200.jpg")
  CropFace(image, eye_left=(356,247), eye_right=(440,273), offset_pct=(0.2,0.2), dest_sz=(200,200)).save("08_20_20_200_200.jpg")
  CropFace(image, eye_left=(356,247), eye_right=(440,273), offset_pct=(0.3,0.3), dest_sz=(200,200)).save("08_30_30_200_200.jpg")
  image =  Image.open("09.jpg")
  CropFace(image, eye_left=(278,264), eye_right=(359,238), offset_pct=(0.1,0.1), dest_sz=(200,200)).save("09_10_10_200_200.jpg")
  CropFace(image, eye_left=(278,264), eye_right=(359,238), offset_pct=(0.2,0.2), dest_sz=(200,200)).save("09_20_20_200_200.jpg")
  CropFace(image, eye_left=(278,264), eye_right=(359,238), offset_pct=(0.3,0.3), dest_sz=(200,200)).save("09_30_30_200_200.jpg")
  image =  Image.open("10.jpg")
  CropFace(image, eye_left=(298,240), eye_right=(387,243), offset_pct=(0.1,0.1), dest_sz=(200,200)).save("10_10_10_200_200.jpg")
  CropFace(image, eye_left=(298,240), eye_right=(387,243), offset_pct=(0.2,0.2), dest_sz=(200,200)).save("10_20_20_200_200.jpg")
  CropFace(image, eye_left=(298,240), eye_right=(387,243), offset_pct=(0.3,0.3), dest_sz=(200,200)).save("10_30_30_200_200.jpg")
  image =  Image.open("11.jpg")
  CropFace(image, eye_left=(269,236), eye_right=(348,213), offset_pct=(0.1,0.1), dest_sz=(200,200)).save("11_10_10_200_200.jpg")
  CropFace(image, eye_left=(269,236), eye_right=(348,213), offset_pct=(0.2,0.2), dest_sz=(200,200)).save("11_20_20_200_200.jpg")
  CropFace(image, eye_left=(269,236), eye_right=(348,213), offset_pct=(0.3,0.3), dest_sz=(200,200)).save("11_30_30_200_200.jpg")
  image =  Image.open("12.jpg")
  CropFace(image, eye_left=(306,249), eye_right=(394,245), offset_pct=(0.1,0.1), dest_sz=(200,200)).save("12_10_10_200_200.jpg")
  CropFace(image, eye_left=(306,249), eye_right=(394,245), offset_pct=(0.2,0.2), dest_sz=(200,200)).save("12_20_20_200_200.jpg")
  CropFace(image, eye_left=(306,249), eye_right=(394,245), offset_pct=(0.3,0.3), dest_sz=(200,200)).save("12_30_30_200_200.jpg")
  image =  Image.open("13.jpg")
  CropFace(image, eye_left=(309,235), eye_right=(397,234), offset_pct=(0.1,0.1), dest_sz=(200,200)).save("13_10_10_200_200.jpg")
  CropFace(image, eye_left=(309,235), eye_right=(397,234), offset_pct=(0.2,0.2), dest_sz=(200,200)).save("13_20_20_200_200.jpg")
  CropFace(image, eye_left=(309,235), eye_right=(397,234), offset_pct=(0.3,0.3), dest_sz=(200,200)).save("13_30_30_200_200.jpg")
  image =  Image.open("14.jpg")
  CropFace(image, eye_left=(296,236), eye_right=(378,235), offset_pct=(0.1,0.1), dest_sz=(200,200)).save("14_10_10_200_200.jpg")
  CropFace(image, eye_left=(296,236), eye_right=(378,235), offset_pct=(0.2,0.2), dest_sz=(200,200)).save("14_20_20_200_200.jpg")
  CropFace(image, eye_left=(296,236), eye_right=(378,235), offset_pct=(0.3,0.3), dest_sz=(200,200)).save("14_30_30_200_200.jpg")
  image =  Image.open("15.jpg")
  CropFace(image, eye_left=(336,219), eye_right=(422,226), offset_pct=(0.1,0.1), dest_sz=(200,200)).save("15_10_10_200_200.jpg")
  CropFace(image, eye_left=(336,219), eye_right=(422,226), offset_pct=(0.2,0.2), dest_sz=(200,200)).save("15_20_20_200_200.jpg")
  CropFace(image, eye_left=(336,219), eye_right=(422,226), offset_pct=(0.3,0.3), dest_sz=(200,200)).save("15_30_30_200_200.jpg")

