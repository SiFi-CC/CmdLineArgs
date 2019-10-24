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

#include <TString.h>
#include <TSystem.h>

#include "CmdLineOption.hh"

class TEnv;

enum ParameterSource { kSql, kFile, kImportExport, kFileImport };

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

  static const PositionalArgs& GetPositionalArguments() { return fgPositional; }
  static const void SetPositionalText(const TString& text) { fPosText = text; }

  TEnv* GetEnv();
  static void ClearOptions();
  static CmdLineOption* Find(const char* name);
  static Bool_t CheckCmdLineSpecial(int argc, char** argv, int i);
  static void PrintHelp();
  static void Print();

protected:
  friend CmdLineOption::~CmdLineOption();
  friend void CmdLineOption::Init(const char* name, const char* cmd,
                                  const char* help);
  void InsertOption(CmdLineOption* opt);
  void RemoveOption(CmdLineOption* opt) { fgList->Remove(opt); }

private:
  static CmdLineConfig* inst;
  static TEnv* fgEnv; // general environment
  TString name;

  static TList* fgList;               // list of command line options
  static PositionalArgs fgPositional; // list of positional arguments

  static TString fPosText;

  ClassDef(CmdLineConfig, 0); // LCOV_EXCL_LINE
};

#endif
