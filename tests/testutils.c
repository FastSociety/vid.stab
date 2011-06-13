
static int readNumber (const char* filename, FILE *f)
{
  int c,n=0;
  for(;;) {
    c = fgetc(f);
    if (c==EOF) 
      ds_log_error("TEST", "unexpected end of file in '%s'", filename);
    if (c >= '0' && c <= '9') n = n*10 + (c - '0');
    else {
      ungetc (c,f);
      return n;
    }
  }
}


static void skipWhiteSpace (const char* filename, FILE *f)
{
  int c,d;
  for(;;) {
    c = fgetc(f);
    if (c==EOF) 
      ds_log_error("TEST", "unexpected end of file in '%s'", filename);

    // skip comments
    if (c == '#') {
      do {
	d = fgetc(f);
	if (d==EOF) 
	  ds_log_error("TEST", "unexpected end of file in '%s'", filename);
      } while (d != '\n');
      continue;
    }

    if (c > ' ') {
      ungetc (c,f);
      return;
    }
  }
}

int loadPGMImage(const char* filename, char ** framebuffer, DSFrameInfo* fi)
{
  FILE *f = fopen (filename,"rb");
  if (!f) { 
    ds_log_error("TEST", "Can't open image file '%s'", filename);
    return 0;
  }

  // read in header
  if (fgetc(f) != 'P' || fgetc(f) != '2')
    ds_log_error("TEST","image file ist not binary PGM (no P5 header) '%s'", filename);
  skipWhiteSpace (filename,f);

  // read in image parameters
  fi->width = readNumber (filename,f);
  skipWhiteSpace (filename,f);
  fi->height = readNumber (filename,f);
  skipWhiteSpace (filename,f);
  int max_value = readNumber (filename,f);

  // check values
  if (fi->width < 1 || fi->height < 1)
    ds_log_error("TEST", "bad image file '%s'", filename);
  if (max_value != 255)
    ds_log_error("TEST", "image file '%s' must have color range 255", filename);

  // read either nothing, LF (10), or CR,LF (13,10)
  int c = fgetc(f);
  if (c == 10) {
    // LF
  }
  else if (c == 13) {
    // CR
    c = fgetc(f);
    if (c != 10) ungetc (c,f);
  }
  else ungetc (c,f);


  // read in rest of data
  char* image_data = (char*) ds_malloc(fi->width*fi->height*3);
  *framebuffer = image_data;
  if (fread( image_data, fi->width*fi->height, 1, f) != 1){
    ds_log_error("TEST", "Can't read data from image file '%s'", filename);
    return 0;
  } 
  fclose (f);
  return 1;
}


int storePGMImage(const char* filename, char * image_data, DSFrameInfo fi ) {
  FILE *f = fopen (filename,"wb");
  if (!f) { 
    ds_log_error("TEST", "Can't open image file '%s'",  filename);
    return 0;
  }

  // write header
  fprintf(f,"P5\n");
  fprintf(f,"# CREATOR test suite of vid.stab\n");
  fprintf(f,"%i %i\n", fi.width, fi.height);
  fprintf(f,"255\n");

  // write data
  if (fwrite( image_data, fi.width*fi.height, 1, f) != 1){
    ds_log_error("TEST", "Can't write to image file '%s'", filename);
    return 0;
  } 
  fclose (f);
  return 1;
}
