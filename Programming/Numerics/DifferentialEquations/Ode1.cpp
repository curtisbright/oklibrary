// Oliver Kullmann, 6.2.2021 (Swansea)
/* Copyright 2021 Oliver Kullmann
This file is part of the OKlibrary. OKlibrary is free software; you can redistribute
it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation and included in this library; either version 3 of the
License, or any later version. */

/*

  USAGE:

  > Ode1 min_x max_x num_int num_subint [iN] [option=+g|-g]

TODOS:

0. For the current example: check accuracy of solution
   - We have for sol(x) = c*exp(sin(x)) - x*x:
       sol(x) = fma(c, expm1(sin(x)), c-x*x);
     perhaps this is more precise?
   - Just using the above for sol doesn't seem to change much (only tiny
     differences). But one should do a precise comparison between the
     two solution-functions.
   - One should also compare with computations with higher precision.

1. Implement -h
   - DONE (disabled the glut-interference)
     Check how these options integrate with the glut-commandline-handling.

2. Enable plotting of arbitrary functions (many of them)
   - In Ode1.fun one specifies the functions which go into window 1/2 (or even
     more windows).
   - These functions yield "plots", the vector of points and accompanying
     parameters.
   - One parameter is the colour, and there should be defaults for automatic
     handling. Perhaps a fixed list of (named) colours.
   - Another parameter is for the lines y=0 resp. x=0; here we need to handle
     the colour -- we can't just always use white for these lines.
     One possibility is to just use the same colour as for the points.
   - For each window one has a list of plots (each in its own scale by default,
     only x-axis is shared).
   - Actually, even the x-axis does not need to be shared -- everything is just
     rescaled.
   - For the solution of the differential equation and its accuracy one has
     standard options to include them into these lists.
   - The statistics-output just applies the standard statistics to all of
     these lists, per window.
   - Then perhaps the output goes into files, one per window.

4. Basic documentation
   - In this file.
   - In a docus-subdirectory.

6. Make also second window resizable

7. Statistics:
   - Count exact zeros.
   - Count sign-changes.
   - As an option for the menu and commandline: print approximate zeros.
    - In ascending order.
    - For sign changes without zero the average of the two x-values where
      the sign changes, with a leading "~".
    - For single exact zeros the x-value.
    - For intervals of exact zeros the interval of x-values.

8. Options for the menu
   - For both windows.
   - Turning a grid on/off (absolute, for the [-1,+1]-ranges).
   - Lines y=0 and x=0 (on/off): perhaps this is global, for each
     plot its own lines.

9. Application tests

*/

#include <iostream>

#include <cmath>

#include <GL/glew.h>
#include <GL/glut.h>

#include <ProgramOptions/Environment.hpp>
#include <Numerics/FloatingPoint.hpp>
#include <Visualisation/Graphics/Plot.hpp>

#include "Ode.hpp"

namespace Ode1 {

  enum class GraphO {with=0, without=1}; // MUST correspond to Registration
  constexpr int GraphOsize = 2;
  constexpr GraphO default_grapho = GraphO::with;
}
namespace Environment {
  template <>
  struct RegistrationPolicies<Ode1::GraphO> {
    static constexpr int size = Ode1::GraphOsize;
    static constexpr std::array<const char*, size> string
    {"+g", "-g"};
  };
}
namespace Ode1 {
  std::ostream& operator <<(std::ostream& out, const GraphO g) {
    switch (g) {
    case GraphO::with : return out << "with-graphs";
    default : return out << "without-graphs";}
  }

  enum class Error {
    pnumber = 1,
    option = 2,

  };

}

namespace {

  const Environment::ProgramInfo proginfo{
        "0.4.3",
        "6.3.2021",
        __FILE__,
        "Oliver Kullmann",
        "https://github.com/OKullmann/oklibrary/blob/master/Programming/Numerics/DifferentialEquations/Ode1.cpp",
        "GPL v3"};

  namespace FP = FloatingPoint;
  using namespace Ode;
  using namespace Ode1;

  const std::string error = "ERROR[" + proginfo.prg + "]: ";

  GraphO to_GraphO(const std::string_view arg) noexcept {
    if (arg.empty()) return GraphO::with;
    const std::optional<GraphO> o = Environment::read<GraphO>(arg);
    if (not o) {
      std::cerr << error << "Invalid option-parameter: \"" << arg << "\".\n";
      std::exit(int(Error::option));
    }
    return o.value();
  }

#include "Ode1.fun"

  int window1, window2;
  RK_t* rk;

  void display() noexcept {
    glutSetWindow(window1);
    Plot::Draw D1(rk->points(), rk->xmin(),rk->xmax(), rk->ymin(),rk->ymax());
    D1.flush();

    glutSetWindow(window2);
    D1.flush();
    D1.yzero();
    Plot::Draw D2(rk->accuracies(), rk->xmin(),rk->xmax(), rk->accmin(),rk->accmax());
    D2.cred=0; D2.cgreen=0; D2.cblue=1; D2.graph();
  }

  void menu_handler(const int v) noexcept {
    if (v == 0) glutDisplayFunc(display);
    else if (v == 1) {
      glutDestroyWindow(window1); glutDestroyWindow(window2);
      std::exit(0);
    }
    glutPostRedisplay();
  }

  void create_menu() noexcept {
    [[maybe_unused]] const auto menu = glutCreateMenu(menu_handler);
    glutAddMenuEntry("Run", 0);
    glutAddMenuEntry("Exit", 1);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
  }

}

int main(const int argc, const char* const argv[]) {

  if (Environment::version_output(std::cout, proginfo, argc, argv)) return 0;
  if (argc < 5 or argc > 7) {
    std::cerr << error << "Four to six input parameters are required:\n"
      " - x_min, x_max,\n"
      " - the numbers of intervals and subintervals,\n"
      " - optionally the initial number of intervals,\n"
      " - optionally the graph-option.\n";
    return int(Error::pnumber);
  }

  const FP::float80 xmin = FP::to_float80(argv[1]),
    xmax = FP::to_float80(argv[2]);
  const FP::UInt_t N = FP::toUInt(argv[3]),
    ssi = FP::toUInt(argv[4]);
  const bool with_iN = argc >= 6 and not std::string_view(argv[5]).empty();
  const FP::UInt_t iN = with_iN ? FP::toUInt(argv[5]) : 0;
  const GraphO go = argc <= 6 ? GraphO::with : to_GraphO(argv[6]);

  rk = new RK_t(x0,y0h,F,sol); // GCC BUG 10.1.0 "y0 is ambiguous"
  if (with_iN) rk->interval(xmin,true, xmax,true, N, ssi, iN);
  else rk->interval(xmin,true, xmax,true, N, ssi);
  rk->update_stats(); rk->update_accuracies();

  FP::fullprec_floatg<Float_t>(std::cout);
  std::cout << *rk; std::cout.flush();

  if (go == GraphO::without) return 0;

  {int argc = 1; char* argv[1] = {(char*) "Ode1"};
   glutInit(&argc, argv);
  }

  glutInitWindowSize(800, 800);
  glutInitWindowPosition(100, 2000);
  glutInitDisplayMode(GLUT_SINGLE);
  window1 = glutCreateWindow("Solution");
  glutInitWindowPosition(1100, 2000);
  window2 = glutCreateWindow("Accuracy");
  glutSetWindow(window1);

  create_menu();
  glewInit();
  display();
  glutMainLoop();

}
