#include "MyASTTool.h"

TypeTheoryOutput ParseFunction(int argc, char **argv)
{
  TypeTheoryOutput *output = new TypeTheoryOutput();
  if (argc < 3)
  {
    outs() << "Usage MyASTTool source funcName";
  }
  else
  {
    int argc1 = argc - 1;
    const char **argv1 = new const char *[argc1];
    for (int i = 0; i < argc1; i++)
    {
      argv1[i] = argv[i];
    }
    CommonOptionsParser OptionsParser(argc1, argv1, MyToolCategory);
    ClangTool Tool(OptionsParser.getCompilations(),
                   OptionsParser.getSourcePathList());

    Tool.run(newTTFrontendActionFactory(argv[argc1], *output).get());
  }

  return *output;
}

void PrintTypeTheoryOutput(TypeTheoryOutput tt)
{

  if (!tt.Variables.empty())
  {
    outs() << "Variables:\n\t";
    for (int i = 0; i < tt.Variables.size(); i++)
    {
      if (!tt.Variables[i].isBOp && !tt.Variables[i].isUOp)
      {
        outs() << tt.Variables[i].Name << ": " << tt.Variables[i].Type
               << " " << ((tt.Variables[i].isInput) ? "Input " : "") << ((tt.Variables[i].isOutput) ? "Output " : "") << "\n\t";
      }
    }
  }
  outs() << "\n";
  if (!tt.BOperations.empty())
  {
    outs() << "Binary Operations:\n\t";
    for (int i = 0; i < tt.BOperations.size(); i++)
    {
      outs() << tt.BOperations[i].Name << "\n\t";
    }
  }

  outs() << "\n";
  if (!tt.UOperations.empty())
  {
    outs() << "Unary Operations:\n\t";
    for (int i = 0; i < tt.UOperations.size(); i++)
    {
      outs() << tt.UOperations[i].Name << "\n\t";
    }
  }
  outs() << "\n";
  if (!tt.ReturnedVariables.empty())
  {
    outs() << "Returned Variables:\n\t";
    for (int i = 0; i < tt.ReturnedVariables.size(); i++)
    {
      outs() << tt.ReturnedVariables[i].Name << "\n\t";
    }
  }
  outs() << "\n";
}