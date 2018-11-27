/******************************************************************************
 *
 * $Id$
 *
 * Environment:
 *    Software development for ANKE detector system at COSY
 *
 * Author List:
 *    Volker Hejny                Original author
 *
 * Copyright Information:
 *    Copyright (C) 2002          Institut für Kernphysik
 *                                Forschungszentrum Jülich
 *
 *****************************************************************************/

/*!
  \file   CmdLineConfig.hh
  \brief  Defines a number of common configuration options

  <long description>

  \author Volker Hejny
  \date   2002-07-29
*/

#ifndef _SORTERCONFIG_HH
#define _SORTERCONFIG_HH

#include <TEnv.h>
#include <TString.h>
#include <TSystem.h>

#include "CmdLineOption.hh"

typedef TEnv RSEnv;

enum ParameterSource { kSql, kFile, kImportExport, kFileImport };

class CmdLineConfig {
public:
  CmdLineConfig();
  virtual ~CmdLineConfig();

  static void ReadCmdLine(int argc, char** argv);

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

protected:
private:
  ClassDef(CmdLineConfig, 0)
};

extern CmdLineConfig* gCmdLineConfig;

#endif
