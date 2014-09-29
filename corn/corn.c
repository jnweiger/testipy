/*
 * corn.c -- a corner detector inspired by sobel.
 *
 * (C) 2014 jw@owncloud.com
 * distribute under GPLv2 or ask.
 *
 * 2014-09-27, jw - initial draught
 */


#include <stdio.h>
#include <png.h>
#include <zlib.h>
#include <malloc.h>	// malloc(), free()
#include <stdlib.h>	// exit()
#include <string.h>	// memcpy()

#define IMG_MARGIN 5		// margin=2 supports 5x5 matrix folds.

struct img
{
  int margin_t, margin_b;	// additional rows/columns around the image
  int margin_l, margin_r;	// additional rows/columns around the image
  unsigned char *r, *b, *g;	// image data to be used (excludes margins)
  unsigned char *mr, *mb, *mg;	// ptrs malloc()/free(). Includes margins.
  int w, h;			// useable image size. (excluding margins)
  int row_stride;		// offset to pixel above, below.

  png_structp png_ptr;		// not sure if we need them. 
  png_infop info_ptr;		// keep them around for a while
};

#define IMG_OFF(img, x, y) ((img)->row_stride*(y)+(x))
// these macros return pointers, you have to dereference them!
#define IMG_R(img, x, y) ((img)->r + IMG_OFF((img), (x), (y)))
#define IMG_G(img, x, y) ((img)->g + IMG_OFF((img), (x), (y)))
#define IMG_B(img, x, y) ((img)->b + IMG_OFF((img), (x), (y)))

struct img *img_alloc6(int w, int h, int mt, int mb, int ml, int mr)
{
  struct img *img = (struct img *)calloc(1, sizeof(struct img));

  img->margin_t = mt;
  img->margin_b = mb;
  img->margin_l = ml;
  img->margin_r = mr;
  img->w = w;
  img->h = h;
  img->row_stride = w + img->margin_r + img->margin_l;

  int rows = h + img->margin_b + img->margin_t;

  img->mr = (unsigned char *)malloc(rows * img->row_stride);
  img->mg = (unsigned char *)malloc(rows * img->row_stride);
  img->mb = (unsigned char *)malloc(rows * img->row_stride);

  int off = IMG_OFF(img, img->margin_l, img->margin_t);

  img->r = img->mr + off;
  img->g = img->mg + off;
  img->b = img->mb + off;
  return img;
}

struct img *img_alloc(int w, int h, int margin)
{
  return img_alloc6(w, h, margin, margin, margin, margin);
}

// de-allocates a full image, including row data, as returned by img_alloc6()
// Or if (mr, mg, mb) are NULL, a shallow reference image
// as returned by img_crop();
void img_free(struct img *img)
{
  // spoil post mortem poking
  img->r = (unsigned char *)0xdeadbeef;
  img->g = (unsigned char *)0xdeadbeef;
  img->b = (unsigned char *)0xdeadbeef;
  img->h = 0;
  img->w = 0;
  img->row_stride = 0;
  if (img->mr) free((void *)(img->mr));
  if (img->mg) free((void *)(img->mg));
  if (img->mb) free((void *)(img->mb));

  if (img->png_ptr)
    png_destroy_read_struct(&(img->png_ptr), &(img->info_ptr), (png_infopp)NULL);
  free((void *)img);
}

// Replicate the edge pixels to fill the outside margins.
// This is optimized for readability, not for speed.
// -> don't call it in tight loops.
void img_fill_margins(struct img *img)
{
  int x, y;
  int x0, y0;

  // the top margin, including the corners.
  for (y = -img->margin_t; y<0; y++)
    {
      y0 = 0;
      for (x = -img->margin_l; x < img->w+img->margin_r; x++)
        {
	  x0 = (x < 0) ? 0 : x;
	  if (x0 >= img->w) x0 = img->w - 1;
	  *IMG_R(img, x, y) = *IMG_R(img, x0, y0);
	  *IMG_G(img, x, y) = *IMG_G(img, x0, y0);
	  *IMG_B(img, x, y) = *IMG_B(img, x0, y0);
	}
    }

  // the bottom margin, including the corners.
  for (y = img->h; y < img->h + img->margin_b; y++)
    {
      y0 = img->h - 1;
      for (x = -img->margin_l; x < img->w+img->margin_r; x++)
        {
	  x0 = (x < 0) ? 0 : x;
	  if (x0 >= img->w) x0 = img->w - 1;
	  *IMG_R(img, x, y) = *IMG_R(img, x0, y0);
	  *IMG_G(img, x, y) = *IMG_G(img, x0, y0);
	  *IMG_B(img, x, y) = *IMG_B(img, x0, y0);
	}
    }

  for (y = 0; y < img->h; y++)
    {
      // left margin
      for (x = -img->margin_l; x < 0; x++)
        {
	  *IMG_R(img, x, y) = *IMG_R(img, 0, y);
	  *IMG_G(img, x, y) = *IMG_G(img, 0, y);
	  *IMG_B(img, x, y) = *IMG_B(img, 0, y);
	}
      // right margin
      for (x = img->w; x < img->w+img->margin_r; x++)
        {
	  *IMG_R(img, x, y) = *IMG_R(img, img->w-1, y);
	  *IMG_G(img, x, y) = *IMG_G(img, img->w-1, y);
	  *IMG_B(img, x, y) = *IMG_B(img, img->w-1, y);
	}
    }
}

struct img *img_load_png(char *filename, int margins)
{
  FILE *ifp = NULL;
  png_structp png_ptr;
  png_infop info_ptr = NULL;
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (setjmp(png_jmpbuf(png_ptr)))
    {
      png_destroy_read_struct(&png_ptr, &info_ptr, 0);
      if (ifp) (void)fclose(ifp);
      fprintf(stderr, "%s: Corrupted PNG file", filename);
      exit(1);
    }
  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
    {
      png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
      fprintf(stderr, "%s: Could not allocate PNG info struct", filename);
      exit(1);
    }
  
  ifp = fopen(filename, "rb");
  if (!ifp)
    {
      perror(filename);
      png_error(png_ptr, "PNG file open failed");
    }
  png_init_io(png_ptr, ifp);

#if PNG_LIBPNG_VER >= 10500 && PNG_LIBPNG_VER < 10502
  /* There is a bug in 1.5 before 1.5.2 which causes png_read_png to
   * whine most terribly on interlaced images, this stops it:
   */
  (void)png_set_interlace_handling(png_ptr);
#endif
  png_read_png(png_ptr, info_ptr, 
    PNG_TRANSFORM_STRIP_16 |
    PNG_TRANSFORM_STRIP_ALPHA | 
    PNG_TRANSFORM_EXPAND |
    PNG_TRANSFORM_GRAY_TO_RGB /* requires libpng 1.4 or later */, 0);

  png_bytepp rows = png_get_rows(png_ptr, info_ptr);
  int h = png_get_image_height(png_ptr, info_ptr);
  int w = png_get_image_width (png_ptr, info_ptr);

  
  struct img *img = img_alloc(w, h, margins);
  img->png_ptr = png_ptr;
  img->info_ptr = info_ptr;

  int x, y;
  for (y = 0; y < h; y++)
    {
      png_bytep row = *rows++;
      unsigned char *r = IMG_R(img, 0, y);
      unsigned char *g = IMG_G(img, 0, y);
      unsigned char *b = IMG_B(img, 0, y);
      for (x = 0; x < w; x++)
        {
	  *r++ = *row++;
	  *g++ = *row++;
	  *b++ = *row++;
	}
    }
  png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
  fclose(ifp);

  img_fill_margins(img);
  return img;
}

// Cut out a region from an image.
//
// if margins were specified during img_alloc(), negative x, y and larger w,h
// than originally provided, may work. Note that you should call 
// img_fill_margins() before actually reading the margins.
//
// If the new coordinates extend beyond what was alloceted, NULL is returned.
// The operation is extremly fast, we only adjust pointers. No memory is freed.
// If p is NULL, new struct img is allocated and returned.
// otherwise p is updated to refer to the new region.
// Note that p shares the image data with img. The mr, mg, mb members of p 
// remain untouched (NULL if internally alloced), so that img_free() can be
// used with p.
// if p == img, inplace crop is done. 

struct img *img_crop(struct img *img, struct img *p, int x, int y, int w, int h)
{
  if (w < 0 || h < 0) return NULL;
  if (x   <         -img->margin_l) return NULL;
  if (x+w > img->w + img->margin_r) return NULL;
  if (y   <         -img->margin_t) return NULL;
  if (y+h > img->h + img->margin_b) return NULL;
  if (!p) p = (struct img *)calloc(1, sizeof(struct img));

  int hh = img->margin_t + img->margin_b + img->h;

  p->margin_l = img->margin_l + x;
  p->margin_t = img->margin_t + y;
  p->margin_r = img->row_stride - p->margin_l - w;
  p->margin_b = hh - p->margin_t - h;

  p->w = w;
  p->h = h;
  p->row_stride = img->row_stride;	// harmless if img == p

  int off = IMG_OFF(img, p->margin_l, p->margin_t);

  p->r = img->mr + off;
  p->g = img->mg + off;
  p->b = img->mb + off;

  return p;
}

// create a new image, with the same properties as img.
// if pixels is nonzero, the colors are copied, otherwise
// the image is created all black (which is faster).
// 
// additional data like png_ptr, info_ptr are not copied.

struct img *img_clone(struct img *img, int pixels)
{
  struct img *p = (struct img *)calloc(1, sizeof(struct img));
  p->margin_t = img->margin_t;
  p->margin_b = img->margin_b;
  p->margin_l = img->margin_l;
  p->margin_r = img->margin_r;

  p->w = img->w;
  p->h = img->h;
  p->row_stride = img->row_stride;

  int area = (p->margin_t+p->margin_b+p->h) * p->row_stride;
  if (pixels)
    {
      p->mr = (unsigned char *)malloc(area);
      p->mg = (unsigned char *)malloc(area);
      p->mb = (unsigned char *)malloc(area);
      memcpy(p->mr, img->mr, area);
      memcpy(p->mg, img->mg, area);
      memcpy(p->mb, img->mb, area);
    }
  else
    {
      p->mr = (unsigned char *)calloc(1, area);
      p->mg = (unsigned char *)calloc(1, area);
      p->mb = (unsigned char *)calloc(1, area);
    }

  int off = IMG_OFF(p, p->margin_l, p->margin_t);

  p->r = p->mr + off;
  p->g = p->mg + off;
  p->b = p->mb + off;

  return p;
}

void img_write_ppm(struct img *img, char *filename)
{
  FILE *fp = fopen(filename, "wb");
  if (!fp)
    {
      perror(filename);
      fprintf(stderr, "img_write_ppm: cannot write\n");
      exit(1);
    }
  fprintf(fp, "P6\n%d %d\n255\n", img->w, img->h);

  int y, x;
  for (y = 0; y < img->h; y++)
    {
      unsigned char *r = IMG_R(img, 0, y);
      unsigned char *g = IMG_G(img, 0, y);
      unsigned char *b = IMG_B(img, 0, y);
      for (x = 0; x < img->w; x++)
        {
	  fputc(*r++, fp);
	  fputc(*g++, fp);
	  fputc(*b++, fp);
	}
    }
  if (fclose(fp))
    {
      perror(filename);
      fprintf(stderr, "img_write_ppm: could not write\n");
      exit(1);
    }
}

#ifndef MIN
# define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
# define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

// _img_Delta2() adds up both, first and second derivative.
// return values are in [0..510]
//
// call with stride=img->row_stride for vertical deltas.
// call with stride=1 for horizontal deltas.
//
static int inline _img_Delta2(unsigned char *r, int stride)
{
  int a = r[-stride] - r[0];
  int b = r[0] - r[stride];

  if ((a < 0 && b >= 0) ||
      (b < 0 && a >= 0))
    return abs(a-b);	      	// different sign
  return MAX(abs(a), abs(b));	// same sign
}

struct img *img_corn_3x3(struct img *img)
{
  struct img *p = img_clone(img, 0);
  int x, y;
  /*
 in 1d:

0 9 9 => 9, 9 0 0 => 9, 0 0 9 => 9, 9 9 0 => 9
0 5 9 => 5, 0 5 0 => 10, 0 9 0 => 18, 3 4 5 => 1, 6 5 4 => 1


Gx[x,y] : a = [x-1,y] - [x,y]; b = [x,y] - [x+1,y]
Gx[x,y] = (sgn(a) != sgn(b)) ? abs(a - b) : max(abs(a),abs(b))

 in 2d: we use min( (3*Gy[x-1]+10*G[x]+3*G[x+1]),
                    (3*G[y-1]+10*G[y]+3*G[y+1]) ) / (2 * 16);
 The min() function helps us to ignore horizontal/vertical bars
  and emphasizes diagonals/corners.

 If we used max() instead, the result would be similar to 
  Sobel or Scharr operators, but with less bleeding.

0 9 0       9 9 9       9 0 9	    0 9 0
0 9 0 => 0, 0 0 0 => 0, 0 9 0 => 9, 9 0 9 => 9
0 9 0	    9 9 9       9 0 9       0 9 0

   */

  int stride = img->row_stride;
  for (y = 0; y < img->h; y++)
    {
      unsigned char *r = IMG_R(p, 0, y);
      unsigned char *g = IMG_G(p, 0, y);
      unsigned char *b = IMG_B(p, 0, y);
      unsigned char *r0 = IMG_R(img, 0, y);
      unsigned char *g0 = IMG_G(img, 0, y);
      unsigned char *b0 = IMG_B(img, 0, y);

      for (x = 0; x < img->w; x++)
        {
#if 0
          if ((x == 474) && (y == 173)) 
            printf("%d\n", _img_Delta2(r0       ,1));
#endif
          int vr = 0;
          int vg = 0;
          int vb = 0;
          int vv = 0;

	  int s1 = 3 * _img_Delta2(r0-1,stride) + 
	          10 * _img_Delta2(r0  ,stride) +
		   3 * _img_Delta2(r0+1,stride);
	  int s2 = 3 * _img_Delta2(r0-stride,1) + 
	          10 * _img_Delta2(r0       ,1) +
		   3 * _img_Delta2(r0+stride,1);
	  s2 = (s1 * s2) >> 14; vr = MIN(s2, 255);
	  // vr = MAX(s1, s2) >> 5;	// x>>5 aka x/(2*(3+10+3))
	  // vr = MIN(s1, s2) >> 5;	// x>>5 aka x/(2*(3+10+3))
	  // vr = (unsigned int)(0.5 * s1);
	  vv = vr;
	  r0++;

          if (vv < 255)
	    {
		  s1 = 3 * _img_Delta2(g0-1,stride) + 
		      10 * _img_Delta2(g0  ,stride) +
		       3 * _img_Delta2(g0+1,stride);
		  s2 = 3 * _img_Delta2(g0-stride,1) + 
		      10 * _img_Delta2(g0       ,1) +
		       3 * _img_Delta2(g0+stride,1);
	      s2 = (s1 * s2) >> 14; vg = MIN(s2, 255);
	      // vg = MAX(s1, s2) >> 5;	// x>>5 aka x/(2*(3+10+3))
	      // vg = MIN(s1, s2) >> 5;	// x>>5 aka x/(2*(3+10+3))
	      // vg = (unsigned int)(0.5 * s1);
	      if (vv < vg) vv = vg;
	    }
	  g0++;

          if (vv < 255)
	    {
		  s1 = 3 * _img_Delta2(b0-1,stride) + 
		      10 * _img_Delta2(b0  ,stride) +
		       3 * _img_Delta2(b0+1,stride);
		  s2 = 3 * _img_Delta2(b0-stride,1) + 
		      10 * _img_Delta2(b0       ,1) +
		       3 * _img_Delta2(b0+stride,1);
	      s2 = (s1 * s2) >> 14; vb = MIN(s2, 255);
	      // vb = MAX(s1, s2) >> 5;	// x>>5 aka x/(2*(3+10+3))
	      // vb = MIN(s1, s2) >> 5;	// x>>5 aka x/(2*(3+10+3))
	      // vb = (unsigned int)(0.5 * s1);
	      if (vv < vb) vv = vb;
	    }
	  b0++;

          // grayscale output, vv=max(r,g,b)
          *r++ = vv;
	  *g++ = vv;
	  *b++ = vv;
	}
    }

  return p;
}

// read png image, process, and write a png image.
int main(int ac, char **av)
{
  char *infile = av[1];
  char *outfile = av[2];

  if (!infile || !outfile)
    {
      fprintf(stderr, "Usage:\n %s INFILE.png OUTFILE.png\n", av[0]);
      exit(0);
    }

  struct img *img = img_load_png(infile, IMG_MARGIN);

#if 0
  struct img *p = img_crop(img, NULL, -img->margin_l, -img->margin_t,
    img->w + img->margin_l + img->margin_r,
    img->h + img->margin_t + img->margin_b);
#else
  struct img *p = img_corn_3x3(img);
#endif
  img_write_ppm(p, outfile);

  return 0;
}

