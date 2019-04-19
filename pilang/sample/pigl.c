#include "eval.h"

#include <GL/gl.h>
#include <GL/glut.h>

#include <stdio.h>

static int64_t int_failsafe(result_t maybe) {
  return maybe.success ? maybe.value.ivalue : 0;
}

static double float_failsafe(result_t maybe) {
  return maybe.success ? maybe.value.fvalue : 0.0;
}

const char* *describe_ffi_funcs(void) {
  static const char* ffi_funcs[] = {
    // GLUT functions
    "ffi_glut_init",
    "ffi_glut_init_window_size",
    "ffi_glut_create_window",
    "ffi_glut_display_func",
    "ffi_glut_main_loop",

    // GL functions
    "ffi_gl_clear",
    "ffi_gl_color",
    "ffi_gl_vertex",
    "ffi_gl_flush",
    "ffi_gl_clear_color",
    "ffi_gl_begin",
    "ffi_gl_end",

    // Fetching constants
    "ffi_gl_const",
    "ffi_glut_const",
    NULL
  };
  return ffi_funcs;
}

const char* *describe_ffi_aliases(void) {
  static const char* ffi_aliases[] = {
    "glutinit",
    "glutinitwindsize",
    "glutcrtwind",
    "glutdispfn",
    "glutmainloop",

    "glclear",
    "glcolor",
    "glvertex",
    "glflush",
    "glclearcolor",
    "glbegin",
    "glend",

    "glconst",
    "glutconst",
    NULL
  };
  return ffi_aliases;
}

#define EXPECT_ARG_COUNT(ARGS, MIN, MAX, FNAME, RETV) \
  if (list_size(&ARGS) < MIN) { \
    eprintf("e: %s requires at least %d arguments\n", FNAME, MIN); \
    return RETV; \
  } \
  if (list_size(&ARGS) > MAX) { \
    eprintf("w: %s requires only %d arguments\n", FNAME, MAX); \
  }

plvalue_t ffi_glut_init(list_t args) {
  plvalue_t ret = create_temp();
  ret.type = JT_UNDEFINED;
  EXPECT_ARG_COUNT(args, 0, 0, "glutinit", ret);
  
  int argc = 1;
  char* argv[] = { "pilang" };
  
  glutInit(&argc, argv);
  glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
  
  ret.type = JT_STR;
  ret.value.svalue = create_string("success");
  return ret;
}

plvalue_t ffi_glut_init_window_size(list_t args) {
  plvalue_t ret = create_temp();
  ret.type = JT_UNDEFINED;
  EXPECT_ARG_COUNT(args, 2, 2, "glutinitwindsize", ret);
  
  iter_t it = list_begin(&args);
  plvalue_t *sv1 = (plvalue_t*)iter_deref(it);
  it = iter_next(it);
  plvalue_t *sv2 = (plvalue_t*)iter_deref(it);
  
  result_t r1 = fetch_int(auto_deref(*sv1));
  result_t r2 = fetch_int(auto_deref(*sv2));
  
  if (!r1.success || !r2.success) {
    return ret;
  }
  
  int64_t w = r1.value.ivalue,
          h = r2.value.ivalue;
          
  glutInitWindowSize(w, h);
  
  ret.type = JT_STR;
  ret.value.svalue = create_string("success");
  return ret;
}

plvalue_t ffi_glut_create_window(list_t args) {
  plvalue_t ret = create_temp();
  ret.type = JT_UNDEFINED;
  EXPECT_ARG_COUNT(args, 1, 1, "glutcrtwind", ret);
  
  plvalue_t *sv = (plvalue_t*)iter_deref(list_begin(&args));
  result_t r = fetch_str(auto_deref(*sv));
  if (!r.success) {
    return ret;
  }

  strhdl_t s = r.value.svalue;
  glutCreateWindow(get_string(s));
  
  ret.type = JT_INT;
  ret.value.ivalue = 0;
  return ret;
}

static strhdl_t glut_display_func_name;

static void display_intern(void) {
  list_t args;
  create_list(&args, malloc, free);
  udfunction_call(glut_display_func_name, args, get_host_env().stack);
  destroy_list(&args);
}

plvalue_t ffi_glut_display_func(list_t args) {
  plvalue_t ret = create_temp();
  ret.type = JT_UNDEFINED;
  EXPECT_ARG_COUNT(args, 1, 1, "glutdispfunc", ret);
  
  plvalue_t *sv = (plvalue_t*)iter_deref(list_begin(&args));
  result_t r = fetch_str(auto_deref(*sv));
  if (!r.success) {
    return ret;
  }

  glut_display_func_name = r.value.svalue;
  glutDisplayFunc(display_intern);
  
  ret.type = JT_INT;
  ret.value.ivalue = 0;
  return ret;
}

plvalue_t ffi_glut_main_loop(list_t args) {
  plvalue_t ret = create_temp();
  ret.type = JT_UNDEFINED;
  EXPECT_ARG_COUNT(args, 0, 0, "glutmainloop", ret);
  
  glutMainLoop();
  
  ret.type = JT_INT;
  ret.value.ivalue = 0;
  return ret;
}

plvalue_t ffi_gl_clear(list_t args) {
  plvalue_t ret = create_temp();
  ret.type = JT_UNDEFINED;
  EXPECT_ARG_COUNT(args, 1, 99, "glclear", ret);
  
  int64_t final = 0;
  for (iter_t it = list_begin(&args);
       !iter_eq(it, list_end(&args));
       it = iter_next(it)) {
    plvalue_t *kv = (plvalue_t*)iter_deref(it);
    final |= int_failsafe(fetch_int(auto_deref(*kv)));
  }
  
  glClear(final);
  
  ret.type = JT_INT;
  ret.value.ivalue = 0;
  return ret;
}

plvalue_t ffi_gl_color(list_t args) {
  plvalue_t ret = create_temp();
  ret.type = JT_UNDEFINED;
  EXPECT_ARG_COUNT(args, 3, 3, "glcolor", ret);
  
  double r, g, b;
  iter_t it = list_begin(&args);
  r = float_failsafe(
        fetch_float(auto_deref(*(plvalue_t*)iter_deref(it))));
  it = iter_next(it);
  g = float_failsafe(
        fetch_float(auto_deref(*(plvalue_t*)iter_deref(it))));
  it = iter_next(it);
  b = float_failsafe(
        fetch_float(auto_deref(*(plvalue_t*)iter_deref(it))));
  
  glColor3f(r, g, b);
  
  ret.type = JT_INT;
  ret.value.ivalue = 0;
  return ret;
}

plvalue_t ffi_gl_vertex(list_t args) {
  plvalue_t ret = create_temp();
  ret.type = JT_UNDEFINED;
  EXPECT_ARG_COUNT(args, 2, 2, "glvertex", ret);
  
  double x, y;
  iter_t it = list_begin(&args);
  x = float_failsafe(
        fetch_float(auto_deref(*(plvalue_t*)iter_deref(it))));
  it = iter_next(it);
  y = float_failsafe(
        fetch_float(auto_deref(*(plvalue_t*)iter_deref(it))));
    
  glVertex2f(x, y);
  
  ret.type = JT_INT;
  ret.value.ivalue = 0;
  return ret;
}

plvalue_t ffi_gl_flush(list_t args) {
  plvalue_t ret = create_temp();
  ret.type = JT_UNDEFINED;
  EXPECT_ARG_COUNT(args, 0, 0, "glflush", ret);

  glFlush();
  
  ret.type = JT_INT;
  ret.value.ivalue = 0;
  return ret;
}

plvalue_t ffi_gl_clear_color(list_t args) {
  plvalue_t ret = create_temp();
  ret.type = JT_UNDEFINED;
  EXPECT_ARG_COUNT(args, 4, 4, "glclearcolor", ret);
  
  double r, g, b, a;
  iter_t it = list_begin(&args);
  r = float_failsafe(
        fetch_float(auto_deref(*(plvalue_t*)iter_deref(it))));
  it = iter_next(it);
  g = float_failsafe(
        fetch_float(auto_deref(*(plvalue_t*)iter_deref(it))));
  it = iter_next(it);
  b = float_failsafe(
        fetch_float(auto_deref(*(plvalue_t*)iter_deref(it))));
  it = iter_next(it);
  a = float_failsafe(
        fetch_float(auto_deref(*(plvalue_t*)iter_deref(it))));
  
  glClearColor(r, g, b, a);
  
  ret.type = JT_INT;
  ret.value.ivalue = 0;
  return ret;
}

plvalue_t ffi_gl_begin(list_t args) {
  plvalue_t ret = create_temp();
  ret.type = JT_UNDEFINED;
  EXPECT_ARG_COUNT(args, 1, 1, "glbegin", ret);

  iter_t it = list_begin(&args);
  int64_t mode = int_failsafe(
                   fetch_int(auto_deref(*(plvalue_t*)iter_deref(it))));
  
  glBegin(mode);
  
  ret.type = JT_INT;
  ret.value.ivalue = 0;
  return ret;
}

plvalue_t ffi_gl_end(list_t args) {
  plvalue_t ret = create_temp();
  ret.type = JT_UNDEFINED;
  EXPECT_ARG_COUNT(args, 0, 0, "glend", ret);

  glEnd();
  
  ret.type = JT_INT;
  ret.value.ivalue = 0;
  return ret;
}

plvalue_t ffi_gl_const(list_t args) {
  plvalue_t ret = create_temp();
  ret.type = JT_UNDEFINED;
  EXPECT_ARG_COUNT(args, 1, 1, "glconst", ret);
  
  plvalue_t *sv = (plvalue_t*)iter_deref(list_begin(&args));
  result_t r = fetch_str(auto_deref(*sv));
  if (!r.success) {
    return ret;
  }

  strhdl_t s = r.value.svalue;
  ret.type = JT_INT;
  if (s == create_string("colorbufferbit")) {
    ret.value.ivalue = GL_COLOR_BUFFER_BIT;
  }
  else if (s == create_string("polygon")) {
    ret.value.ivalue = GL_POLYGON;
  }
  else {
    ret.value.ivalue = 0;
  }
  return ret;
}

plvalue_t ffi_glut_const(list_t args) {
  plvalue_t ret = create_temp();
  ret.type = JT_UNDEFINED;
  EXPECT_ARG_COUNT(args, 1, 1, "glutconst", ret);
  
  plvalue_t *sv = (plvalue_t*)iter_deref(list_begin(&args));
  result_t r = fetch_str(auto_deref(*sv));
  if (!r.success) {
    return ret;
  }
  
  strhdl_t s = r.value.svalue;
  ret.type = JT_INT;
  if (s == create_string("single")) {
    ret.value.ivalue = GLUT_SINGLE;
  }
  else if (s == create_string("rgb")) {
    ret.value.ivalue = GLUT_RGB;
  }
  else {
    ret.value.ivalue = 0;
  }
  return ret;
}
