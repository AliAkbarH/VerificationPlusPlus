#include <stdio.h>     /* for printf */
#include <stdlib.h>
#include <getopt.h>

#include<iostream>
#include<z3.h>
using namespace std;

#include <scma.h>

void
print_usage() {
  cout << "usage: sc [options]" << endl;
  //help for theory file
  cout << "  -t | --theory filename*: pass a theory file name." << endl;
  cout << "      filename is a requred parameter." << endl;
  cout << "      if option is not used, simple build in vocab is used for demo." << endl;
  cout << endl;
  //help for gui
  cout << "  -g | --gui: use  the gui interface." << endl;
  cout << endl;
  //help for build vocab
  cout << "  -b | --build-vocab: build vocab from the rules and declarations in the theory file." << endl;
  cout << "     -t must be present." << endl;
  cout << endl;
  //help for inject exists
  cout << "  -e | --inject-exists: if passed the injected quantified variables are existential." << endl;
  cout << endl;

  //help for quantifier free
  cout << "  -f | --quantifier-free: if passed a decidable quantifier free vocab \ni"
    "    with enough expressive power for `array property' union `list property' \n"
    "    is generated and used." << endl;
  cout << endl;

  cout << "  -h | --help: prints this help." << endl;
  cout << endl;
}

int
SpecCheckVocab:: compute_options(int argc, char ** argv) {//GUI add int argc, char ** argv
  int c;
  int digit_optind = 0;

  theoryFileName = "";
  isGui = false;
  genVocab = false;
  injectLocal = false;
  useQuantFree = false;

  while (1) {
    int this_option_optind = optind ? optind : 1;
    int option_index = 0;

    enum{ OPT_NONE, OPT_THEORY, OPT_GUI, OPT_BUILD_VOCAB,
      OPT_INJECT_LOCAL, OPT_QUANT_FREE,

      OPT_HELP, 
      OPT_LAST};

    static struct option long_options[] = {
      {"theory",  required_argument, 0,  OPT_THEORY },
      {"gui",     no_argument,       0,  OPT_GUI},
      {"build-vocab", no_argument,   0,  OPT_BUILD_VOCAB },
      {"inject-exists", no_argument, 0,  OPT_INJECT_LOCAL},
      {"quantifier-free",no_argument,0,  OPT_QUANT_FREE},
      {"help",no_argument,0,  OPT_HELP},
      {0,         0,                 0,  0 }
    };

    //t is for theory
    //g is for gui
    //b is for build vocab
    //e is for inject-exists
    //f is for quantifier free
    //h is for help
    c = getopt_long(argc, argv, "t:gbefh",
        long_options, &option_index);
    if (c == -1)
      break;

    switch (c) {
      case 't':
        option_index = 0;
      case OPT_THEORY:
        if (optarg) {
          theoryFileName = optarg;
        }
        break;
      case 'g':
        option_index = 1;
        //isGui = true;
      case OPT_GUI:
        isGui = true;
        break;
      case 'b':
        option_index = 2;
      case OPT_BUILD_VOCAB:
        genVocab = true;
        break;
      case 'e':
        option_index = 3;
      case OPT_INJECT_LOCAL:
        injectLocal = true;
        break;
      case 'f':
        option_index = 4;
      case OPT_QUANT_FREE:
        useQuantFree = true;
        break;
      case 'h':
        option_index = 5;
      case OPT_HELP:
        print_usage();
        exit(0);
        break;
      case 0:
        printf("option %s", long_options[option_index].name);
        if (optarg)
          printf(" with arg %s", optarg);
        printf("\n");
        break;
      case '?':
        break;

      default:
        printf("?? getopt returned character code 0%o ??\n", c);
    }
  }

  if (optind < argc) {
    printf("non-option ARGV-elements: ");
    while (optind < argc)
      printf("%s ", argv[optind++]);
    printf("\n");
  }
  return 0;
}
