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

#include <stdio.h>
//#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include "envi.h"

#ifndef strncasecmp
#define strncasecmp strnicmp
#endif

#if defined(_WIN32) || defined(_WIN64) 
#define strcasecmp _stricmp 
#define strncasecmp _strnicmp 
#endif

#ifndef min
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif

#ifndef max
#define max(x, y) ((x) < (y) ? (y) : (x))
#endif

envi_header_t *envi_header_new(void)
{
  envi_header_t *hdr = NULL;

  hdr = (envi_header_t *)malloc(sizeof(*hdr));
  assert(hdr);
  memset(hdr, 0, sizeof(*hdr));

  return hdr;
}

void envi_header_destroy(envi_header_t *hdr)
{
  uint32_t i;

  assert(hdr);

  if (hdr->description) free(hdr->description);
  if (hdr->file_type) free(hdr->file_type);
  if (hdr->sensor_type) free(hdr->sensor_type);

  for (i = 0; i < hdr->bands; i++) {
    if (hdr->wavelengths[i])
      free(hdr->wavelengths[i]);
  }
  if (hdr->wavelengths) free(hdr->wavelengths);

  free(hdr);
}

void envi_header_set_description(envi_header_t *hdr, const char *desc)
{
  assert(hdr);
  assert(desc);

  if (hdr->description) free(hdr->description);
  hdr->description = _strdup(desc);
}

void envi_header_set_dimension(envi_header_t *hdr, uint32_t samples, uint32_t lines, uint32_t bands)
{
  uint32_t i;

  assert(hdr);

  hdr->samples = samples;
  hdr->lines = lines;

  for (i = 0; i < hdr->bands; i++) {
    if (hdr->wavelengths[i]) free(hdr->wavelengths[i]);
  }
  if (hdr->wavelengths) free(hdr->wavelengths);

  hdr->bands = bands;
  hdr->wavelengths = (char **)malloc(bands * sizeof(char *));
  memset(hdr->wavelengths, 0, bands * sizeof(char *));
}

void envi_header_set_file_type(envi_header_t *hdr, const char *filetype)
{
  assert(hdr);
  assert(filetype);

  if (hdr->file_type) free(hdr->file_type);
  hdr->file_type = _strdup(filetype);
}

void envi_header_set_sensor_type(envi_header_t *hdr, const char *sensortype)
{
  assert(hdr);
  assert(sensortype);

  if (hdr->sensor_type) free(hdr->sensor_type);
  hdr->sensor_type = _strdup(sensortype);
}

void envi_header_set_wavelength(envi_header_t *hdr, uint32_t i, const char *content)
{
  assert(hdr);
  assert(i < hdr->bands);
  assert(content);
  assert(hdr->wavelengths);

  if (i >= hdr->bands) return;
  
  if (hdr->wavelengths[i]) free(hdr->wavelengths[i]);
  hdr->wavelengths[i] = _strdup(content);
}

static char *skipleading(const char *str)
{
  assert(str);

  while ((*str != '\0') && (*str <= ' ')) str++;
  return (char *)str;
}

static char *skiptrailing(const char *str, const char *base)
{
  assert(str);
  assert(base);

  while (str > base && *(str - 1) <= ' ') str--;
  return (char *)str;
}

static char *striptrailing(char *str)
{
  char *ptr = skiptrailing(strchr(str, '\0'), str);
  assert(ptr);

  *ptr = '\0';

  return ptr;
}

static char *fgetkeys(char *str, int str_size, const char *key, FILE *fd)
{
  char *sp, *ep, *s;
  int len;
  char buf[256];

  assert(fd);
  assert(key);

  len = strlen(key);

  do {
    if (fgets(buf, 256, fd) == NULL) return NULL;
    sp = skipleading(buf);
    //if (*sp == ';' || *sp == '#') break;
    ep = strchr(sp, '=');
  } while (ep == NULL ||
	   ((int)(skiptrailing(ep, sp) - sp) != len ||
	    strncasecmp(sp, key, len) != 0));

  assert(ep);
  assert(*ep == '=');


  sp = skipleading(ep + 1);

  // Array
  if (*sp == '{') {
    s = str;
    sp = skipleading(sp + 1);
    do {
      ep = strchr(sp, '}');
      if (ep) {
	ep = skiptrailing(ep, sp);
	len = min(str_size, ep - sp);
	strncpy_s(s, len, sp, len);
	s += len;
	break;
      } else {
	ep = skiptrailing(strchr(sp, '\0'), sp);
	if (ep > sp) {
	  len = min(str_size, ep - sp);
	  strncpy_s(s, len, sp, len);
	  s += len;
	  str_size -= len;
	}
      }
      if (fgets(buf, 256, fd) == NULL) break;
      sp = skipleading(buf);
    } while (1);
    *s = '\0';
  } else {
    ep = striptrailing(sp);
    if (ep > sp)
      strncpy_s(str, str_size, sp, str_size);
  }

  return str;
}

int envi_header_read_file(const char *filename, envi_header_t *hdr)
{
  FILE *fd;
  char buf[8192], *s, *sp, *ep;
  uint32_t samples, lines, bands;
  int i;

  assert(filename);
  assert(hdr);

  //if ((fd = fopen_s(filename, "rb")) == NULL) {
  fopen_s(&fd, filename, "rb");
  if (fd == NULL) {
    printf("%d@%s: \"%s\" file open error!\n", __LINE__,  __FILE__, filename);
    return -1;
  }

  if (fgetkeys(buf, 8192, "description", fd) != NULL) {
    envi_header_set_description(hdr, buf);
    printf("%s\n", hdr->description);
  }

  rewind(fd);
  if (fgetkeys(buf, 8192, "samples", fd) != NULL) {
    samples = strtoul(buf, NULL, 10);
    printf("%d\n", samples);
  }

  rewind(fd);
  if (fgetkeys(buf, 8192, "lines", fd) != NULL) {
    lines = strtoul(buf, NULL, 10);
    printf("%d\n", lines);
  }

  rewind(fd);
  if (fgetkeys(buf, 8192, "bands", fd) != NULL) {
    bands = strtoul(buf, NULL, 10);
    printf("%d\n", bands);
  }

  envi_header_set_dimension(hdr, samples, lines, bands);

  rewind(fd);
  if (fgetkeys(buf, 8192, "header offset", fd) != NULL) {
    hdr->header_offset = atoi(buf);
    printf("%d\n", hdr->header_offset);
  }

  rewind(fd);
  if (fgetkeys(buf, 8192, "file type", fd) != NULL) {
    envi_header_set_file_type(hdr, buf);
    printf("%s\n", hdr->file_type);
  }

  rewind(fd);
  if (fgetkeys(buf, 8192, "data type", fd) != NULL) {
    hdr->data_type = atoi(buf);
    printf("%d\n", hdr->data_type);
  }

  rewind(fd);
  if (fgetkeys(buf, 8192, "interleave", fd) != NULL) {
    hdr->interleave[0] = buf[0];
    hdr->interleave[1] = buf[1];
    hdr->interleave[2] = buf[2];
    
    printf("%c%c%c\n",
	   hdr->interleave[0],
	   hdr->interleave[1],
	   hdr->interleave[2]);
    
  }

  rewind(fd);
  if (fgetkeys(buf, 8192, "sensor type", fd) != NULL) {
    envi_header_set_sensor_type(hdr, buf);
    printf("%s\n", hdr->sensor_type);
  }

  rewind(fd);
  if (fgetkeys(buf, 8192, "byte order", fd) != NULL) {
    hdr->byte_order = atoi(buf);
    printf("%d\n", hdr->byte_order);
  }

  rewind(fd);
  if ((sp = fgetkeys(buf, 8192, "wavelengths", fd)) != NULL) {
    for (i = 0; i < hdr->bands; i++) {
      sp = skipleading(sp);
      ep = strchr(sp, ',');
      if (ep) *ep = '\0';
      envi_header_set_wavelength(hdr, i, sp);
      printf("%s\n", hdr->wavelengths[i]);
      sp = ep + 1;
    }
  } else {
    rewind(fd);
	if ((sp = fgetkeys(buf, 8192, "band names", fd)) != NULL) {
      for (i = 0; i < hdr->bands; i++) {
        sp = skipleading(sp);
        ep = strchr(sp, ',');
        if (ep) *ep = '\0';
        envi_header_set_wavelength(hdr, i, sp);
        printf("%s\n", hdr->wavelengths[i]);
        sp = ep + 1;
      }
    }
  }

  fclose(fd);

  return 0;
}

int envi_header_write_file(envi_header_t *hdr, const char *filename)
{
  FILE *fd;
  char buf[8192], *s, *sp, *ep;
  uint32_t samples, lines, bands;
  int i;

  assert(filename);
  assert(hdr);

  //if ((fd = fopen(filename, "wb")) == NULL) {
  fopen_s(&fd, filename, "wb");
  if (fd == NULL) {
    printf("%d@%s: \"%s\" file open error!\n", __LINE__,  __FILE__, filename);
    return -1;
  }

  fputs("ENVI\n", fd);

  fputs("description = {\n", fd);
  fprintf(fd, "  %s }\n", hdr->description);

  fprintf(fd, "samples = %d\n", hdr->samples);
  fprintf(fd, "lines = %d\n", hdr->lines);
  fprintf(fd, "bands = %d\n", hdr->bands);

  fprintf(fd, "header offset = %d\n", hdr->header_offset);
  fprintf(fd, "file type = %s\n", hdr->file_type);
  fprintf(fd, "data type = %d\n", hdr->data_type);
  fprintf(fd, "interleave = %c%c%c\n",
	  hdr->interleave[0],
	  hdr->interleave[1],
	  hdr->interleave[2]);
  fprintf(fd, "sensor type = %s\n", hdr->sensor_type);
  fprintf(fd, "byte order = %d\n", hdr->byte_order);

  fputs("wavelengths = {\n", fd);
  for (i = 0; i < hdr->bands; i++) {
    fprintf(fd, " %s%s",
	    hdr->wavelengths[i],
	    (i == hdr->bands - 1 ? "}\n" : (i % 8 == 7 ? ",\n" : ",")));
  }

  fclose(fd);

  return 0;
}

int envi_header_get_bpp(envi_header_t *hdr)
{
  int bpp = 0;

  assert(hdr);

  switch (hdr->data_type) {
  case 1: bpp = 8; break; // uint8_t
  case 2: bpp = 16; break; // int16_t
  case 3: bpp = 32; break; // int32_t
  case 4: bpp = 32; break; // float
  case 5: bpp = 64; break; // double
  case 6: bpp = 64; break; // complex (float, float)
  case 9: bpp = 128; break; // complex (double, double)
  case 12: bpp = 16; break; // uint16_t
  case 13: bpp = 32; break; // uint32_t
  case 14: bpp = 64; break; // int64_t
  case 15: bpp = 64; break; // uint64_t
  default: bpp = 0; break;
  }
  return bpp;
}

int envi_header_get_size_of_image(envi_header_t *hdr)
{
	assert(hdr);

	return hdr->lines * (((hdr->samples * envi_header_get_bpp(hdr)) + 31) & ~31) / 8;
}

int envi_header_get_size_of_images(envi_header_t *hdr)
{
  assert(hdr);

  return hdr->bands * envi_header_get_size_of_image(hdr);
}

envi_t *envi_new(void)
{
  envi_t *envi;

  envi= malloc(sizeof(*envi));
  assert(envi);

  envi->header = envi_header_new();
  envi->image = NULL;

  return envi;
}

void envi_destroy(envi_t *envi)
{
  assert(envi);

  envi_header_destroy(envi->header);
  if (envi->image) free(envi->image);
  free(envi);
}

int envi_read_files(const char *headerfile, const char *imagefile, envi_t *envi)
{
  FILE *fd;
  int i, j, imagesize;
  char *buf;

  assert(envi);
  assert(envi->header);

  envi_header_read_file(headerfile, envi->header);

  //imagesize = envi_header_get_size_of_images(envi->header);
  imagesize = envi->header->bands * 
	  envi->header->lines * 
	  envi->header->samples * 
	  envi_header_get_bpp(envi->header) / 8;
  printf("image size: %d\n", imagesize);
  if (envi->image) free(envi->image);
  envi->image = (void *)malloc(imagesize);
  assert(envi->image);
  memset(envi->image, 0, imagesize);

  //if ((fd = fopen(imagefile, "rb")) == NULL) {
  fopen_s(&fd, imagefile, "rb");
  if (fd == NULL) {
    fprintf(stderr, "%s:%d (%s): \"%s\" file open error!\n", __FILE__, __LINE__, __FUNCTION__, imagefile);
    goto __error_return;
  }

//  fseek(fd, 0L, SEEK_END);
//  printf("real image size: %d\n", ftell(fd));
//  fseek(fd, 0L, SEEK_SET);

#if 1
  buf = (char *)envi->image;
  for (i = 0; i < imagesize; ) {
    j = fread(&buf[i], 1, 4096, fd);
    i += j;
    if (j == 0) {
      i += fread(&buf[i], 1, imagesize-i, fd);
      break;
    }
  }
#else
  if (fread(envi->image, 1, imagesize, fd) != imagesize) {
    fprintf(stderr, "%s:%d (%s): \"%s\" file read error!\n", __FILE__, __LINE__, __FUNCTION__, imagefile);
    goto __error_return;
  }
#endif

  fclose(fd);

  return 0;

 __error_return:;

  if (fd) fclose(fd);
  free(envi->image);
  envi->image = NULL;

  return -1;
}

int envi_write_files(envi_t *envi, const char *headerfile, const char *imagefile)
{
  FILE *fd;
  int imagesize;

  assert(envi);
  assert(envi->header);
  assert(envi->image);

  envi_header_write_file(envi->header, headerfile);

  imagesize = envi_header_get_size_of_images(envi->header);

  //if ((fd = fopen(imagefile, "wb")) == NULL) {
  fopen_s(&fd, imagefile, "wb");
  if (fd == NULL) {
    fprintf(stderr, "%s:%d (%s): \"%s\" file open error!\n", __FILE__, __LINE__, __FUNCTION__, imagefile);
    goto __error_return;
  }

  if (fwrite(envi->image, 1, imagesize, fd) != imagesize) {
    fprintf(stderr, "%s:%d (%s): \"%s\" file write error!\n", __FILE__, __LINE__, __FUNCTION__, imagefile);
    goto __error_return;
  }

  fclose(fd);

  return 0;

 __error_return:;

  if (fd) fclose(fd);

  return -1;
}

void envi_get_value(void *value, int x, int y, int z, envi_t *envi)
{
	int blocksize, pitch, byteperpixel;
	uint8_t *buf;

	assert(value);
	assert(envi);
	assert(x >= 0 && x < envi->header->samples);
	assert(y >= 0 && y < envi->header->lines);
	assert(z >= 0 && z < envi->header->bands);
	assert(envi->header->interleave[0] == 'B');

	buf = (uint8_t *)envi->image;

	if (envi->header->interleave[1] == 'S' &&
		envi->header->interleave[2] == 'Q') { // BSQ
		byteperpixel = envi_header_get_bpp(envi->header) / 8;
		pitch = envi->header->samples * byteperpixel;
		blocksize = envi->header->lines * pitch;
		memcpy(value, buf + z * blocksize + y * pitch + x * byteperpixel, byteperpixel);
	} else if (envi->header->interleave[1] == 'I' &&
		envi->header->interleave[2] == 'L') { // BIL
		byteperpixel = envi_header_get_bpp(envi->header) / 8;
		pitch = envi->header->samples * byteperpixel;
		blocksize = envi->header->bands * pitch;
		memcpy(value, buf + y * blocksize + z * pitch + x * byteperpixel, byteperpixel);
	} else if (envi->header->interleave[1] == 'I' &&
		envi->header->interleave[2] == 'P') { // BIP
		byteperpixel = envi_header_get_bpp(envi->header) / 8;
		pitch = envi->header->bands * byteperpixel;
		blocksize = envi->header->samples * pitch;
		memcpy(value, buf + y * blocksize + x * pitch + z * byteperpixel, byteperpixel);
	} else {
		assert(0);
	}
}

#if 0
int envi_get_image(void *image, int band, envi_t *envi)
{
	int x, y, w, h, bpp;
	int byteperpixel, pitch;

	assert(image);
	assert(envi);

	w = envi->header->samples;
	h = envi->header->lines;
	bpp = envi_header_get_bpp(envi->header);

	pitch =  (((w * bpp) + 31) & ~31) / 8;
	byteperpixel = bpp / 8;

	//printf("width(%d), height(%d), byteperpixel(%d)\n", w, h, byteperpixel);
	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			envi_get_value((uint8_t *)image + y * pitch + x * byteperpixel, x, y, band, envi);
		}
	}

	return 0;
}
#endif

int envi_get_image(void *image, int band, envi_t *envi)
{
	int x, y, w, h, bpp;
	int byteperpixel, pitch1, pitch2, blocksize;
	uint8_t *buf;

	assert(image);
	assert(envi);
	assert(envi->header->interleave[0] == 'B');

	w = envi->header->samples;
	h = envi->header->lines;
	bpp = envi_header_get_bpp(envi->header);

	pitch1 =  (((w * bpp) + 31) & ~31) / 8;
	byteperpixel = bpp / 8;

	/////////////////////////////////////////////////////////////////////////////////////////
	buf = (uint8_t *)envi->image;

	if (envi->header->interleave[1] == 'S' &&
		envi->header->interleave[2] == 'Q') { // BSQ
		//byteperpixel = envi_header_get_bpp(envi->header) / 8;
		pitch2 = envi->header->samples * byteperpixel;
		blocksize = envi->header->lines * pitch2;
		for (y = 0; y < h; y++)
			memcpy((uint8_t *)image + y * pitch1, buf + band * blocksize + y * pitch2, byteperpixel * w);
	} else if (envi->header->interleave[1] == 'I' &&
		envi->header->interleave[2] == 'L') { // BIL
		//byteperpixel = envi_header_get_bpp(envi->header) / 8;
		pitch2 = envi->header->samples * byteperpixel;
		blocksize = envi->header->bands * pitch2;
		for (y = 0; y < h; y++) {
			memcpy((uint8_t *)image + y * pitch1, buf + y * blocksize + band * pitch2, byteperpixel * w);
		}
	} else if (envi->header->interleave[1] == 'I' &&
		envi->header->interleave[2] == 'P') { // BIP
		//byteperpixel = envi_header_get_bpp(envi->header) / 8;
		pitch2 = envi->header->bands * byteperpixel;
		blocksize = envi->header->samples * pitch2;
		for (y = 0; y < h; y++) {
			for (x = 0; x < w; x++) {
				memcpy((uint8_t *)image + y * pitch1 + x * byteperpixel, buf + y * blocksize + x * pitch2 + band * byteperpixel, byteperpixel);
			}
		}
	} else {
		assert(0);
	}

	return 0;
}

/*
int main(int argc, char *argv[])
{
  envi_t *envi;

  envi = envi_new();

  envi_read("BatteryB_60Min.HDR", "BatteryB_60Min.IMG", envi);

  envi_write(envi, "BatteryB_60Min.BAK.HDR", "BatteryB_60Min.BAK.IMG");

  envi_destroy(envi);

  return 0;
}
*/
