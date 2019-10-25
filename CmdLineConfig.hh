/******************************************************************************
 *
 * $Id$
 *
 * Environment:
 *    Software development for ANKE detector system at COSY
 *
 * Author List:
 *    Volker Hejny                Original author
 *    Rafał Lalik                 Modifications, creation of CmdLineArgs library
 *
 * Copyright Information:
 *    Copyright (C) 2002          Institut für Kernphysik
 *                                Forschungszentrum Jülich
 *    Copyright (C) 2018          Rafał Lalik, Jagiellonian University Kraków
 *
 *****************************************************************************/

/*!
  \file   CmdLineConfig.hh
  \brief  Defines a number of common configuration options

  <long description>

  \author Volker Hejny
  \date   2002-07-29
*/

#ifndef _CMDLINECONFIG_HH
#define _CMDLINECONFIG_HH

#include <list>
#include <map>

#include <TString.h>
#include <TSystem.h>

#include "CmdLineArg.hh"
#include "CmdLineOption.hh"

class TEnv;

enum ParameterSource { kSql, kFile, kImportExport, kFileImport };

typedef std::map<std::string, CmdLineArg*> Positional;
typedef std::vector<CmdLineArg*> Greedy;

class CmdLineConfig {
protected:
  CmdLineConfig();
  CmdLineConfig(const char* name);
  virtual ~CmdLineConfig();

public:
  static CmdLineConfig* instance(const char* name = nullptr);

  void ReadCmdLine(int argc, char** argv);

  // special treatment for the following parameters are needed

  static ParameterSource GetParameterSource();
  static ParameterSource GetParameterSourceType(const char* objectname);
  static const TString GetParameterSource(const char* objectname);
  static void SetParameterSource(const char* objectname, const char* source);

  static ParameterSource GetParameterDrain();
  static ParameterSource GetParameterDrainType(const char* objectname);
  static const TString GetParameterDrain(const char* objectname);
  static void SetParameterDrain(const char* objectname, const char* source);

  static const TString GetResource(const char* path, const char* file,
                                   EAccessMode mode = kFileExists);

  static const void SetPositionalText(const TString& text) { fPosText = text; }
  static const Positional& GetPositionalArguments() { return fgArgs; }
  static const Greedy& GetGreedyArguments() { return fgGreedy; }

  TEnv* GetEnv();
  static void ClearOptions();
  static void RestoreDefaults();
  static CmdLineOption* FindOption(const char* name);
  static CmdLineArg* FindArgument(const char* name);
  static Bool_t CheckCmdLineSpecial(int argc, char** argv, int i);
  static void PrintHelp(int argc, char** argv);
  static void Print();

protected:
  friend CmdLineOption::~CmdLineOption();
  friend void CmdLineOption::Init(const char* name, const char* cmd,
                                  const char* help);

  friend CmdLineArg::~CmdLineArg();
  friend void CmdLineArg::Init(const char* name, const char* help, bool greedy);

  void Insert(CmdLineOption* opt);
  void Remove(CmdLineOption* opt) { fgOpts.erase(opt->fName); }

  void Insert(CmdLineArg* opt);
  void Remove(CmdLineArg* opt) { fgArgs.erase(opt->fName.Data()); }

private:
  static CmdLineConfig* inst;
  static TEnv* fgEnv; // general environment
  TString name;

  typedef std::map<TString, CmdLineOption*> Options;
  static Options fgOpts;      // list of command line options
  static Positional fgArgs;   // list of command line arguments
  static Greedy fgGreedy;     // list of command line greedy arguments
  static CmdLineArg* fGreedy; // greedy argument reference
  static Int_t fGreedyPosition;

  static TString fPosText;

  typedef std::list<std::string> ListMap;
  static ListMap _map_opts, _map_args;

  ClassDef(CmdLineConfig, 0); // LCOV_EXCL_LINE
};

#endif
