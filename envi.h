/*
 Copyright 2011 Hoyoung Lee.

 This file is part of ENVIewer.

 This program is free software; you can redistribute it and/or modify it
 under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, please visit www.gnu.org.
*/

#ifndef __ENVI_H__
#define __ENVI_H__

#ifdef _MSC_VER
#include <limits.h>
typedef unsigned __int8 uint8_t;
typedef unsigned __int32 uint32_t;
#define UINT8_MAX UCHAR_MAX
//#define UINT32_MAX U
#else
#include <stdint.h>
#endif

typedef struct {
  char *description;
  uint32_t samples; // number of columns
  uint32_t lines; // number of rows
  uint32_t bands; // number of bands (channels, planes)
  uint8_t header_offset; //
  char *file_type;
  uint8_t data_type; // following types are supported
  /* 1: 1 byte uint,
   * 2: 2 bytes sint,
   * 3: 4 bytes sint,
   * 4: 4 bytes float,
   * 5: 8 bytes double,
   * 9: 2x8 bytes complex number made up from 2 doubles,
   * 12: 2 bytes unsigned int */
  char interleave[3]; // permutations of dimensions in binary data
  /* BSQ: Band Sequenial - X[0..band][0..row][0..col]
   * BIL: Band Interleave by Line - X[0..row][0..band][0..col]
   * BIP: Band Interleave by Pixel - X[0..row][0..col][0..band] */
  char *sensor_type;
  uint8_t byte_order; // 0: little-endian, 1: big-endian
  char **wavelengths;
} envi_header_t;

typedef struct {
  envi_header_t *header;
  void *image;
} envi_t;

#ifdef __cplusplus
extern "C" {
#endif

  envi_header_t *envi_header_new(void);
  void envi_header_destroy(envi_header_t *hdr);
  void envi_header_set_description(envi_header_t *hdr, const char *desc);
  void envi_header_set_dimension(envi_header_t *hdr, uint32_t samples, uint32_t lines, uint32_t bands);
  void envi_header_set_file_type(envi_header_t *hdr, const char *filetype);
  void envi_header_set_sensor_type(envi_header_t *hdr, const char *sensortype);
  void envi_header_set_wavelength(envi_header_t *hdr, uint32_t i, const char *content);
  int envi_header_read_file(const char *filename, envi_header_t *hdr);
  int envi_header_write_file(envi_header_t *hdr, const char *filename);
  int envi_header_get_bpp(envi_header_t *hdr);
  int envi_header_get_size_of_image(envi_header_t *hdr);
  int envi_header_get_size_of_images(envi_header_t *hdr);

  envi_t *envi_new(void);
  void envi_destroy(envi_t *envi);
  int envi_read_files(const char *headerfile, const char *imagefile, envi_t *envi);
  int envi_write_files(envi_t *envi, const char *headerfile, const char *imagefile);
  void envi_get_value(void *value, int x, int y, int z, envi_t *envi);
  int envi_get_image(void *image, int band, envi_t *envi);
#define envi_get_width(envi) (envi->header->samples)
#define envi_get_height(envi) (envi->header->lines)
#define envi_get_depth(envi) (envi->header->bands)

#ifdef __cplusplus
}
#endif

#endif /* __ENVI_H__ */

